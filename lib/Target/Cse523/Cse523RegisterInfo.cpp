//===-- Cse523RegisterInfo.cpp - Cse523 Register Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Cse523 implementation of the TargetRegisterInfo class.
// This file is responsible for the frame pointer elimination optimization
// on Cse523.
//
//===----------------------------------------------------------------------===//

#include "Cse523RegisterInfo.h"
#include "Cse523.h"
#include "Cse523InstrBuilder.h"
#include "Cse523MachineFunctionInfo.h"
#include "Cse523Subtarget.h"
#include "Cse523TargetMachine.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#define GET_REGINFO_TARGET_DESC
#include "Cse523GenRegisterInfo.inc"

using namespace llvm;

cl::opt<bool>
ForceStackAlign("force-align-stack",
        cl::desc("Force align the stack to the minimum alignment"
            " needed for the function."),
        cl::init(false), cl::Hidden);

static cl::opt<bool>
EnableBasePointer("cse523-use-base-pointer", cl::Hidden, cl::init(true),
        cl::desc("Enable use of a base pointer for complex stack frames"));

Cse523RegisterInfo::Cse523RegisterInfo(Cse523TargetMachine &tm)
    : Cse523GenRegisterInfo(Cse523::RIP, // Always use RIP for 64 bit
            Cse523_MC::getDwarfRegFlavour(tm.getTargetTriple(), false),
            Cse523_MC::getDwarfRegFlavour(tm.getTargetTriple(), true),
            Cse523::RIP // Always use RIP for 64 bit
    ),
    TM(tm) 
{
    Cse523_MC::InitLLVM2SEHRegisterMapping(this);

    SlotSize = 8;
    StackPtr = Cse523::RSP;
    FramePtr = Cse523::RBP;
    // Use a callee-saved register as the base pointer.  These registers must
    // not conflict with any ABI requirements.  For example, in 32-bit mode PIC
    // requires GOT in the EBX register before function calls via PLT GOT pointer.
    BasePtr = Cse523::RBX;
}

/// getCompactUnwindRegNum - This function maps the register to the number for
/// compact unwind encoding. Return -1 if the register isn't valid.
int Cse523RegisterInfo::getCompactUnwindRegNum(unsigned RegNum, bool isEH) const {
    switch (getLLVMRegNum(RegNum, isEH)) {
        case Cse523::RBX: return 1;
        case Cse523::R12: return 2;
        case Cse523::R13: return 3;
        case Cse523::R14: return 4;
        case Cse523::R15: return 5;
        case Cse523::RBP: return 6;
    }

    return -1;
}

bool
Cse523RegisterInfo::trackLivenessAfterRegAlloc(const MachineFunction &MF) const {
    // ExeDepsFixer and PostRAScheduler require liveness.
    return true;
}

int
Cse523RegisterInfo::getSEHRegNum(unsigned i) const {
    return getEncodingValue(i);
}

const TargetRegisterClass *
Cse523RegisterInfo::getSubClassWithSubReg(const TargetRegisterClass *RC,
        unsigned Idx) const {

    llvm_unreachable("Calls invalid getSubClassWithSubReg()");

    // Forward to TableGen's default version.
    return Cse523GenRegisterInfo::getSubClassWithSubReg(RC, Idx);
}

const TargetRegisterClass *
Cse523RegisterInfo::getMatchingSuperRegClass(const TargetRegisterClass *A,
        const TargetRegisterClass *B,
        unsigned SubIdx) const {

    llvm_unreachable("Calls invalid getMatchingSuperRegClass()");

    return Cse523GenRegisterInfo::getMatchingSuperRegClass(A, B, SubIdx);
}

const TargetRegisterClass*
Cse523RegisterInfo::getLargestLegalSuperClass(const TargetRegisterClass *RC) const {
    // Don't allow super-classes of GR8_NOREX.  This class is only used after
    // extrating sub_8bit_hi sub-registers.  The H sub-registers cannot be copied
    // to the full GR8 register class in 64-bit mode, so we cannot allow the
    // reigster class inflation.
    //
    // The GR8_NOREX class is always used in a way that won't be constrained to a
    // sub-class, so sub-classes like GR8_ABCD_L are allowed to expand to the
    // full GR8 class.
    const TargetRegisterClass *Super = RC;
    TargetRegisterClass::sc_iterator I = RC->getSuperClasses();
    do {
        switch (Super->getID()) {
            case Cse523::GR64RegClassID:
                // Don't return a super-class that would shrink the spill size.
                // That can happen with the vector and float classes.
                if (Super->getSize() == RC->getSize())
                    return Super;
        }
        Super = *I++;
    } while (Super);
    return RC;
}

    const TargetRegisterClass *
Cse523RegisterInfo::getPointerRegClass(const MachineFunction &MF, unsigned Kind) const {

     return &Cse523::GR64RegClass;
}

const TargetRegisterClass *
Cse523RegisterInfo::getCrossCopyRegClass(const TargetRegisterClass *RC) const {
    if (RC == &Cse523::CCRRegClass) {
        return &Cse523::GR64RegClass;
    }
    return RC;
}

unsigned
Cse523RegisterInfo::getRegPressureLimit(const TargetRegisterClass *RC,
        MachineFunction &MF) const {

    switch (RC->getID()) {
        default:
            return 0;
        case Cse523::GR64RegClassID:
            return 12;
    }
}

const uint16_t *
Cse523RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {

    // TODO: Verify the return calling conv
    switch (MF->getFunction()->getCallingConv()) {
        case CallingConv::AnyReg:
            return CSR_64_AllRegs_SaveList;
        case CallingConv::PreserveMost:
            return CSR_64_RT_MostRegs_SaveList;
        case CallingConv::PreserveAll:
            return CSR_64_RT_AllRegs_SaveList;
        case CallingConv::Cold:
            return CSR_64_MostRegs_SaveList;
        default:
            break;
    }

    bool CallsEHReturn = MF->getMMI().callsEHReturn();
    if (CallsEHReturn)
        return CSR_64EHRet_SaveList;
    return CSR_64_SaveList;
}

const uint32_t*
Cse523RegisterInfo::getCallPreservedMask(CallingConv::ID CC) const {

    switch (CC) {
        case CallingConv::AnyReg:
            return CSR_64_AllRegs_RegMask;
        case CallingConv::PreserveMost:
            return CSR_64_RT_MostRegs_RegMask;
        case CallingConv::PreserveAll:
            return CSR_64_RT_AllRegs_RegMask;
        case CallingConv::Cold:
            return CSR_64_MostRegs_RegMask;
        default:
            break;
    }

    return CSR_64_RegMask;
}

const uint32_t*
Cse523RegisterInfo::getNoPreservedMask() const {
    return CSR_NoRegs_RegMask;
}

BitVector Cse523RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
    BitVector Reserved(getNumRegs());

    Reserved.set(Cse523::RSP);
    Reserved.set(Cse523::RIP);
    Reserved.set(Cse523::RBP);

    // Set the base-pointer register and its aliases as reserved if needed.
    if (hasBasePointer(MF)) {
        CallingConv::ID CC = MF.getFunction()->getCallingConv();
        const uint32_t* RegMask = getCallPreservedMask(CC);
        if (MachineOperand::clobbersPhysReg(RegMask, getBaseRegister()))
            report_fatal_error(
                    "Stack realignment in presence of dynamic allocas is not supported with"
                    "this calling convention.");

        for (MCSubRegIterator I(getBaseRegister(), this, /*IncludeSelf=*/true);
                I.isValid(); ++I)
            Reserved.set(*I);
    }

    // Mark the segment registers as reserved.
    Reserved.set(Cse523::CS);
    Reserved.set(Cse523::SS);
    Reserved.set(Cse523::DS);
    Reserved.set(Cse523::ES);
    Reserved.set(Cse523::FS);
    Reserved.set(Cse523::GS);

    return Reserved;
}

//===----------------------------------------------------------------------===//
// Stack Frame Processing methods
//===----------------------------------------------------------------------===//

bool Cse523RegisterInfo::hasBasePointer(const MachineFunction &MF) const {
    const MachineFrameInfo *MFI = MF.getFrameInfo();

    if (!EnableBasePointer)
        return false;

    // When we need stack realignment, we can't address the stack from the frame
    // pointer.  When we have dynamic allocas or stack-adjusting inline asm, we
    // can't address variables from the stack pointer.  MS inline asm can
    // reference locals while also adjusting the stack pointer.  When we can't
    // use both the SP and the FP, we need a separate base pointer register.
    bool CantUseFP = needsStackRealignment(MF);
    bool CantUseSP =
        MFI->hasVarSizedObjects() || MFI->hasInlineAsmWithSPAdjust();
    return CantUseFP && CantUseSP;
}

bool Cse523RegisterInfo::canRealignStack(const MachineFunction &MF) const {
    if (MF.getFunction()->hasFnAttribute("no-realign-stack"))
        return false;

    const MachineFrameInfo *MFI = MF.getFrameInfo();
    const MachineRegisterInfo *MRI = &MF.getRegInfo();

    // Stack realignment requires a frame pointer.  If we already started
    // register allocation with frame pointer elimination, it is too late now.
    if (!MRI->canReserveReg(FramePtr))
        return false;

    // If a base pointer is necessary.  Check that it isn't too late to reserve
    // it.
    if (MFI->hasVarSizedObjects())
        return MRI->canReserveReg(BasePtr);
    return true;
}

bool Cse523RegisterInfo::needsStackRealignment(const MachineFunction &MF) const {
    const MachineFrameInfo *MFI = MF.getFrameInfo();
    const Function *F = MF.getFunction();
    unsigned StackAlign = TM.getFrameLowering()->getStackAlignment();
    bool requiresRealignment =
        ((MFI->getMaxAlignment() > StackAlign) ||
         F->getAttributes().hasAttribute(AttributeSet::FunctionIndex,
             Attribute::StackAlignment));

    // If we've requested that we force align the stack do so now.
    if (ForceStackAlign)
        return canRealignStack(MF);

    return requiresRealignment && canRealignStack(MF);
}

bool Cse523RegisterInfo::hasReservedSpillSlot(const MachineFunction &MF,
        unsigned Reg, int &FrameIdx) const {
    const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();

    if (Reg == FramePtr && TFI->hasFP(MF)) {
        FrameIdx = MF.getFrameInfo()->getObjectIndexBegin();
        return true;
    }
    return false;
}

void
Cse523RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
        int SPAdj, unsigned FIOperandNum,
        RegScavenger *RS) const {
    assert(SPAdj == 0 && "Unexpected");

    MachineInstr &MI = *II;
    MachineFunction &MF = *MI.getParent()->getParent();
    const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();
    int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
    unsigned BasePtr;

    unsigned Opc = MI.getOpcode();
    bool AfterFPPop = false;//Opc == Cse523::TAILJMPm64 || Opc == Cse523::TAILJMPm;
    if (hasBasePointer(MF))
        BasePtr = (FrameIndex < 0 ? FramePtr : getBaseRegister());
    else if (needsStackRealignment(MF))
        BasePtr = (FrameIndex < 0 ? FramePtr : StackPtr);
    else if (AfterFPPop)
        BasePtr = StackPtr;
    else
        BasePtr = (TFI->hasFP(MF) ? FramePtr : StackPtr);

    // This must be part of a four operand memory reference.  Replace the
    // FrameIndex with base register with EBP.  Add an offset to the offset.
    MI.getOperand(FIOperandNum).ChangeToRegister(BasePtr, false);

    // Now add the frame object offset to the offset from EBP.
    int FIOffset;
    if (AfterFPPop) {
        // Tail call jmp happens after FP is popped.
        const MachineFrameInfo *MFI = MF.getFrameInfo();
        FIOffset = MFI->getObjectOffset(FrameIndex) - TFI->getOffsetOfLocalArea();
    } else
        FIOffset = TFI->getFrameIndexOffset(MF, FrameIndex);

    // The frame index format for stackmaps and patchpoints is different from the
    // Cse523 format. It only has a FI and an offset.
    if (Opc == TargetOpcode::STACKMAP || Opc == TargetOpcode::PATCHPOINT) {
        assert(BasePtr == FramePtr && "Expected the FP as base register");
        int64_t Offset = MI.getOperand(FIOperandNum + 1).getImm() + FIOffset;
        MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);
        return;
    }

    if (MI.getOperand(FIOperandNum+3).isImm()) {
        // Offset is a 32-bit integer.
        int Imm = (int)(MI.getOperand(FIOperandNum + 3).getImm());
        int Offset = FIOffset + Imm;
        assert((!Is64Bit || isInt<32>((long long)FIOffset + Imm)) &&
                "Requesting 64-bit offset in 32-bit immediate!");
        MI.getOperand(FIOperandNum + 3).ChangeToImmediate(Offset);
    } else {
        // Offset is symbolic. This is extremely rare.
        uint64_t Offset = FIOffset +
            (uint64_t)MI.getOperand(FIOperandNum+3).getOffset();
        MI.getOperand(FIOperandNum + 3).setOffset(Offset);
    }
}

unsigned Cse523RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
    const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();
    return TFI->hasFP(MF) ? FramePtr : StackPtr;
}

namespace llvm {
    unsigned getCse523SubSuperRegister(unsigned Reg, MVT::SimpleValueType VT,
            bool High) {
        switch (VT) {
            default: llvm_unreachable("Unexpected VT");
            case MVT::i64:
             switch (Reg) {
                 default: llvm_unreachable("Unexpected register");
                 case Cse523::RAX:
                          return Cse523::RAX;
                 case Cse523::RDX:
                          return Cse523::RDX;
                 case Cse523::RCX:
                          return Cse523::RCX;
                 case Cse523::RBX:
                          return Cse523::RBX;
                 case Cse523::RSI:
                          return Cse523::RSI;
                 case Cse523::RDI:
                          return Cse523::RDI;
                 case Cse523::RBP:
                          return Cse523::RBP;
                 case Cse523::RSP:
                          return Cse523::RSP;
                 case Cse523::R8:
                          return Cse523::R8;
                 case Cse523::R9:
                          return Cse523::R9;
                 case Cse523::R10:
                          return Cse523::R10;
                 case Cse523::R11:
                          return Cse523::R11;
                 case Cse523::R12:
                          return Cse523::R12;
                 case Cse523::R13:
                          return Cse523::R13;
                 case Cse523::R14:
                          return Cse523::R14;
                 case Cse523::R15:
                          return Cse523::R15;
             }
        }
    }

    unsigned get512BitSuperRegister(unsigned Reg) {
        llvm_unreachable("Unexpected SIMD register");
    }

}

