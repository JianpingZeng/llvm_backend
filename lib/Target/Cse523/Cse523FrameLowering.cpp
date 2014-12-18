//===-- Cse523FrameLowering.cpp - Cse523 Frame Information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Cse523 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "Cse523FrameLowering.h"
#include "Cse523InstrBuilder.h"
#include "Cse523InstrInfo.h"
#include "Cse523MachineFunctionInfo.h"
#include "Cse523Subtarget.h"
#include "Cse523TargetMachine.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

// FIXME: completely move here.
extern cl::opt<bool> ForceStackAlign;

bool Cse523FrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
    return !MF.getFrameInfo()->hasVarSizedObjects();
}

/// hasFP - Return true if the specified function should have a dedicated frame
/// pointer register.  This is true if the function has variable sized allocas
/// or if frame pointer elimination is disabled.
bool Cse523FrameLowering::hasFP(const MachineFunction &MF) const {
    const MachineFrameInfo *MFI = MF.getFrameInfo();
    const MachineModuleInfo &MMI = MF.getMMI();
    const TargetRegisterInfo *RegInfo = TM.getRegisterInfo();

    return (MF.getTarget().Options.DisableFramePointerElim(MF) ||
            RegInfo->needsStackRealignment(MF) ||
            MFI->hasVarSizedObjects() ||
            MFI->isFrameAddressTaken() || MFI->hasInlineAsmWithSPAdjust() ||
            MF.getInfo<Cse523MachineFunctionInfo>()->getForceFramePointer() ||
            MMI.callsUnwindInit() || MMI.callsEHReturn());
}

static unsigned getSUBriOpcode(unsigned IsLP64, int64_t Imm) {
    assert(IsLP64);
    return Cse523::SUB64ri32;
}

static unsigned getADDriOpcode(unsigned IsLP64, int64_t Imm) {
    assert(IsLP64);
    return Cse523::ADD64ri32;
}

static unsigned getLEArOpcode(unsigned IsLP64) {
    assert(IsLP64);
    return Cse523::LEA64r;
}

/// findDeadCallerSavedReg - Return a caller-saved register that isn't live
/// when it reaches the "return" instruction. We can then pop a stack object
/// to this register without worry about clobbering it.
static unsigned findDeadCallerSavedReg(MachineBasicBlock &MBB,
        MachineBasicBlock::iterator &MBBI,
        const TargetRegisterInfo &TRI,
        bool Is64Bit) {
    const MachineFunction *MF = MBB.getParent();
    const Function *F = MF->getFunction();
    if (!F || MF->getMMI().callsEHReturn())
        return 0;

    static const uint16_t CallerSavedRegs64Bit[] = {
        Cse523::RAX, Cse523::RDX, Cse523::RCX, Cse523::RSI, Cse523::RDI,
        Cse523::R8,  Cse523::R9,  Cse523::R10, Cse523::R11, 0
    };

    unsigned Opc = MBBI->getOpcode();
    switch (Opc) {
        default: return 0;
        case Cse523::RETQ:
        case Cse523::RETIQ:
        case Cse523::TCRETURNdi64:
        case Cse523::TCRETURNri64:
        case Cse523::TCRETURNmi64:
        case Cse523::EH_RETURN64: {
                                      SmallSet<uint16_t, 8> Uses;
                                      for (unsigned i = 0, e = MBBI->getNumOperands(); i != e; ++i) {
                                          MachineOperand &MO = MBBI->getOperand(i);
                                          if (!MO.isReg() || MO.isDef())
                                              continue;
                                          unsigned Reg = MO.getReg();
                                          if (!Reg)
                                              continue;
                                          for (MCRegAliasIterator AI(Reg, &TRI, true); AI.isValid(); ++AI)
                                              Uses.insert(*AI);
                                      }

                                      const uint16_t *CS = CallerSavedRegs64Bit;
                                      for (; *CS; ++CS)
                                          if (!Uses.count(*CS))
                                              return *CS;
                                  }
    }

    return 0;
}


/// emitSPUpdate - Emit a series of instructions to increment / decrement the
/// stack pointer by a constant value.
static
void emitSPUpdate(MachineBasicBlock &MBB, MachineBasicBlock::iterator &MBBI,
        unsigned StackPtr, int64_t NumBytes,
        bool Is64Bit, bool IsLP64, bool UseLEA,
        const TargetInstrInfo &TII, const TargetRegisterInfo &TRI) {
    bool isSub = NumBytes < 0;
    uint64_t Offset = isSub ? -NumBytes : NumBytes;
    unsigned Opc;
    if (UseLEA)
        Opc = getLEArOpcode(IsLP64);
    else
        Opc = isSub
            ? getSUBriOpcode(IsLP64, Offset)
            : getADDriOpcode(IsLP64, Offset);

    uint64_t Chunk = (1LL << 31) - 1;
    DebugLoc DL = MBB.findDebugLoc(MBBI);

    while (Offset) {
        uint64_t ThisVal = (Offset > Chunk) ? Chunk : Offset;
        if (ThisVal == 8) {
            // Use push / pop instead.
            unsigned Reg = isSub
                ? (unsigned)(Cse523::RAX)
                : findDeadCallerSavedReg(MBB, MBBI, TRI, Is64Bit);
            if (Reg) {
                Opc = isSub ? (Cse523::PUSH64r) : (Cse523::POP64r);
                MachineInstr *MI = BuildMI(MBB, MBBI, DL, TII.get(Opc))
                    .addReg(Reg, getDefRegState(!isSub) | getUndefRegState(isSub));
                if (isSub)
                    MI->setFlag(MachineInstr::FrameSetup);
                Offset -= ThisVal;
                continue;
            }
        }

        MachineInstr *MI = NULL;

        if (UseLEA) {
            MI =  addRegOffset(BuildMI(MBB, MBBI, DL, TII.get(Opc), StackPtr),
                    StackPtr, false, isSub ? -ThisVal : ThisVal);
        } else {
            MI = BuildMI(MBB, MBBI, DL, TII.get(Opc), StackPtr)
                .addReg(StackPtr)
                .addImm(ThisVal);
            MI->getOperand(3).setIsDead(); // The EFLAGS implicit def is dead.
        }

        if (isSub)
            MI->setFlag(MachineInstr::FrameSetup);

        Offset -= ThisVal;
    }
}

/// mergeSPUpdatesUp - Merge two stack-manipulating instructions upper iterator.
static
void mergeSPUpdatesUp(MachineBasicBlock &MBB, MachineBasicBlock::iterator &MBBI,
        unsigned StackPtr, uint64_t *NumBytes = NULL) {
    if (MBBI == MBB.begin()) return;

    MachineBasicBlock::iterator PI = prior(MBBI);
    unsigned Opc = PI->getOpcode();

    // TODO:
    if ((Opc == Cse523::ADD64ri32 || Opc == Cse523::LEA64r) &&
            PI->getOperand(0).getReg() == StackPtr) {
        if (NumBytes)
            *NumBytes += PI->getOperand(2).getImm();
        MBB.erase(PI);
    } else if ((Opc == Cse523::SUB64ri32) &&
            PI->getOperand(0).getReg() == StackPtr) {
        if (NumBytes)
            *NumBytes -= PI->getOperand(2).getImm();
        MBB.erase(PI);
    }
}

/// mergeSPUpdatesDown - Merge two stack-manipulating instructions lower iterator.
static
void mergeSPUpdatesDown(MachineBasicBlock &MBB,
        MachineBasicBlock::iterator &MBBI,
        unsigned StackPtr, uint64_t *NumBytes = NULL) {
    // FIXME:  THIS ISN'T RUN!!!
    return;

    if (MBBI == MBB.end()) return;

    MachineBasicBlock::iterator NI = llvm::next(MBBI);
    if (NI == MBB.end()) return;

    unsigned Opc = NI->getOpcode();
    if ((Opc == Cse523::ADD64ri32) &&
            NI->getOperand(0).getReg() == StackPtr) {
        if (NumBytes)
            *NumBytes -= NI->getOperand(2).getImm();
        MBB.erase(NI);
        MBBI = NI;
    } else if ((Opc == Cse523::SUB64ri32) &&
            NI->getOperand(0).getReg() == StackPtr) {
        if (NumBytes)
            *NumBytes += NI->getOperand(2).getImm();
        MBB.erase(NI);
        MBBI = NI;
    }
}

/// mergeSPUpdates - Checks the instruction before/after the passed
/// instruction. If it is an ADD/SUB/LEA instruction it is deleted argument and the
/// stack adjustment is returned as a positive value for ADD/LEA and a negative for
/// SUB.
static int mergeSPUpdates(MachineBasicBlock &MBB,
        MachineBasicBlock::iterator &MBBI,
        unsigned StackPtr,
        bool doMergeWithPrevious) {
    if ((doMergeWithPrevious && MBBI == MBB.begin()) ||
            (!doMergeWithPrevious && MBBI == MBB.end()))
        return 0;

    MachineBasicBlock::iterator PI = doMergeWithPrevious ? prior(MBBI) : MBBI;
    MachineBasicBlock::iterator NI = doMergeWithPrevious ? 0 : llvm::next(MBBI);
    unsigned Opc = PI->getOpcode();
    int Offset = 0;

    // TODO:
    if ((Opc == Cse523::ADD64ri32 || Opc == Cse523::LEA64r) &&
            PI->getOperand(0).getReg() == StackPtr){
        Offset += PI->getOperand(2).getImm();
        MBB.erase(PI);
        if (!doMergeWithPrevious) MBBI = NI;
    } else if ((Opc == Cse523::SUB64ri32) &&
            PI->getOperand(0).getReg() == StackPtr) {
        Offset -= PI->getOperand(2).getImm();
        MBB.erase(PI);
        if (!doMergeWithPrevious) MBBI = NI;
    }

    return Offset;
}

void Cse523FrameLowering::emitCalleeSavedFrameMoves(MachineFunction &MF,
        MCSymbol *Label,
        unsigned FramePtr) const {
    MachineFrameInfo *MFI = MF.getFrameInfo();
    MachineModuleInfo &MMI = MF.getMMI();
    const MCRegisterInfo *MRI = MMI.getContext().getRegisterInfo();

    // Add callee saved registers to move list.
    const std::vector<CalleeSavedInfo> &CSI = MFI->getCalleeSavedInfo();
    if (CSI.empty()) return;

    const Cse523RegisterInfo *RegInfo = TM.getRegisterInfo();
    bool HasFP = hasFP(MF);

    // Calculate amount of bytes used for return address storing.
    int stackGrowth = -RegInfo->getSlotSize();

    // FIXME: This is dirty hack. The code itself is pretty mess right now.
    // It should be rewritten from scratch and generalized sometimes.

    // Determine maximum offset (minimum due to stack growth).
    int64_t MaxOffset = 0;
    for (std::vector<CalleeSavedInfo>::const_iterator
            I = CSI.begin(), E = CSI.end(); I != E; ++I)
        MaxOffset = std::min(MaxOffset,
                MFI->getObjectOffset(I->getFrameIdx()));

    // Calculate offsets.
    int64_t saveAreaOffset = (HasFP ? 3 : 2) * stackGrowth;
    for (std::vector<CalleeSavedInfo>::const_iterator
            I = CSI.begin(), E = CSI.end(); I != E; ++I) {
        int64_t Offset = MFI->getObjectOffset(I->getFrameIdx());
        unsigned Reg = I->getReg();
        Offset = MaxOffset - Offset + saveAreaOffset;

        // Don't output a new machine move if we're re-saving the frame
        // pointer. This happens when the PrologEpilogInserter has inserted an extra
        // "PUSH" of the frame pointer -- the "emitPrologue" method automatically
        // generates one when frame pointers are used. If we generate a "machine
        // move" for this extra "PUSH", the linker will lose track of the fact that
        // the frame pointer should have the value of the first "PUSH" when it's
        // trying to unwind.
        //
        // FIXME: This looks inelegant. It's possibly correct, but it's covering up
        //        another bug. I.e., one where we generate a prolog like this:
        //
        //          pushl  %ebp
        //          movl   %esp, %ebp
        //          pushl  %ebp
        //          pushl  %esi
        //           ...
        //
        //        The immediate re-push of EBP is unnecessary. At the least, it's an
        //        optimization bug. EBP can be used as a scratch register in certain
        //        cases, but probably not when we have a frame pointer.
        if (HasFP && FramePtr == Reg)
            continue;

        unsigned DwarfReg = MRI->getDwarfRegNum(Reg, true);
        MMI.addFrameInst(MCCFIInstruction::createOffset(Label, DwarfReg, Offset));
    }
}

/// usesTheStack - This function checks if any of the users of EFLAGS
/// copies the EFLAGS. We know that the code that lowers COPY of EFLAGS has
/// to use the stack, and if we don't adjust the stack we clobber the first
/// frame index.
/// See Cse523InstrInfo::copyPhysReg.
static bool usesTheStack(const MachineFunction &MF) {
    const MachineRegisterInfo &MRI = MF.getRegInfo();

    for (MachineRegisterInfo::reg_iterator ri = MRI.reg_begin(Cse523::EFLAGS),
            re = MRI.reg_end(); ri != re; ++ri)
        if (ri->isCopy())
            return true;

    return false;
}

/// emitPrologue - Push callee-saved registers onto the stack, which
/// automatically adjust the stack pointer. Adjust the stack pointer to allocate
/// space for local variables. Also emit labels used by the exception handler to
/// generate the exception handling frames.
void Cse523FrameLowering::emitPrologue(MachineFunction &MF) const {
    MachineBasicBlock &MBB = MF.front(); // Prologue goes in entry BB.
    MachineBasicBlock::iterator MBBI = MBB.begin();
    MachineFrameInfo *MFI = MF.getFrameInfo();
    const Function *Fn = MF.getFunction();
    const Cse523RegisterInfo *RegInfo = TM.getRegisterInfo();
    const Cse523InstrInfo &TII = *TM.getInstrInfo();
    MachineModuleInfo &MMI = MF.getMMI();
    Cse523MachineFunctionInfo *Cse523FI = MF.getInfo<Cse523MachineFunctionInfo>();
    bool needsFrameMoves = MMI.hasDebugInfo() ||
        Fn->needsUnwindTableEntry();
    uint64_t MaxAlign  = MFI->getMaxAlignment(); // Desired stack alignment.
    uint64_t StackSize = MFI->getStackSize();    // Number of bytes to allocate.
    bool HasFP = hasFP(MF);
    bool Is64Bit = STI.is64Bit();
    bool IsLP64 = STI.isTarget64BitLP64();
    bool IsWin64 = STI.isTargetWin64();
    bool UseLEA = STI.useLeaForSP();
    unsigned StackAlign = getStackAlignment();
    unsigned SlotSize = RegInfo->getSlotSize();
    unsigned FramePtr = RegInfo->getFrameRegister(MF);
    unsigned StackPtr = RegInfo->getStackRegister();
    unsigned BasePtr = RegInfo->getBaseRegister();
    DebugLoc DL;

    // If we're forcing a stack realignment we can't rely on just the frame
    // info, we need to know the ABI stack alignment as well in case we
    // have a call out.  Otherwise just make sure we have some alignment - we'll
    // go with the minimum SlotSize.
    if (ForceStackAlign) {
        if (MFI->hasCalls())
            MaxAlign = (StackAlign > MaxAlign) ? StackAlign : MaxAlign;
        else if (MaxAlign < SlotSize)
            MaxAlign = SlotSize;
    }

    // Add RETADDR move area to callee saved frame size.
    int TailCallReturnAddrDelta = Cse523FI->getTCReturnAddrDelta();
    if (TailCallReturnAddrDelta < 0)
        Cse523FI->setCalleeSavedFrameSize(
                Cse523FI->getCalleeSavedFrameSize() - TailCallReturnAddrDelta);

    // If this is cse523-64 and the Red Zone is not disabled, if we are a leaf
    // function, and use up to 128 bytes of stack space, don't have a frame
    // pointer, calls, or dynamic alloca then we do not need to adjust the
    // stack pointer (we fit in the Red Zone). We also check that we don't
    // push and pop from the stack.
    if (Is64Bit && !Fn->getAttributes().hasAttribute(AttributeSet::FunctionIndex,
                Attribute::NoRedZone) &&
            !RegInfo->needsStackRealignment(MF) &&
            !MFI->hasVarSizedObjects() &&                     // No dynamic alloca.
            !MFI->adjustsStack() &&                           // No calls.
            !IsWin64 &&                                       // Win64 has no Red Zone
            !usesTheStack(MF) &&                              // Don't push and pop.
            !MF.getTarget().Options.EnableSegmentedStacks) {  // Regular stack
        uint64_t MinSize = Cse523FI->getCalleeSavedFrameSize();
        if (HasFP) MinSize += SlotSize;
        StackSize = std::max(MinSize, StackSize > 128 ? StackSize - 128 : 0);
        MFI->setStackSize(StackSize);
    }

    // Insert stack pointer adjustment for later moving of return addr.  Only
    // applies to tail call optimized functions where the callee argument stack
    // size is bigger than the callers.
    if (TailCallReturnAddrDelta < 0) {
        MachineInstr *MI =
            BuildMI(MBB, MBBI, DL,
                    TII.get(getSUBriOpcode(IsLP64, -TailCallReturnAddrDelta)),
                    StackPtr)
            .addReg(StackPtr)
            .addImm(-TailCallReturnAddrDelta)
            .setMIFlag(MachineInstr::FrameSetup);
        MI->getOperand(3).setIsDead(); // The EFLAGS implicit def is dead.
    }

    // Mapping for machine moves:
    //
    //   DST: VirtualFP AND
    //        SRC: VirtualFP              => DW_CFA_def_cfa_offset
    //        ELSE                        => DW_CFA_def_cfa
    //
    //   SRC: VirtualFP AND
    //        DST: Register               => DW_CFA_def_cfa_register
    //
    //   ELSE
    //        OFFSET < 0                  => DW_CFA_offset_extended_sf
    //        REG < 64                    => DW_CFA_offset + Reg
    //        ELSE                        => DW_CFA_offset_extended

    uint64_t NumBytes = 0;
    int stackGrowth = -SlotSize;

    if (HasFP) {
        // Calculate required stack adjustment.
        uint64_t FrameSize = StackSize - SlotSize;
        if (RegInfo->needsStackRealignment(MF)) {
            // Callee-saved registers are pushed on stack before the stack
            // is realigned.
            FrameSize -= Cse523FI->getCalleeSavedFrameSize();
            NumBytes = (FrameSize + MaxAlign - 1) / MaxAlign * MaxAlign;
        } else {
            NumBytes = FrameSize - Cse523FI->getCalleeSavedFrameSize();
        }

        // Get the offset of the stack slot for the EBP register, which is
        // guaranteed to be the last slot by processFunctionBeforeFrameFinalized.
        // Update the frame offset adjustment.
        MFI->setOffsetAdjustment(-NumBytes);

        // Save EBP/RBP into the appropriate stack slot.
        BuildMI(MBB, MBBI, DL, TII.get(Cse523::PUSH64r))
            .addReg(FramePtr, RegState::Kill)
            .setMIFlag(MachineInstr::FrameSetup);

        if (needsFrameMoves) {
            // Mark the place where EBP/RBP was saved.
            MCSymbol *FrameLabel = MMI.getContext().CreateTempSymbol();
            BuildMI(MBB, MBBI, DL, TII.get(Cse523::PROLOG_LABEL))
                .addSym(FrameLabel);

            // Define the current CFA rule to use the provided offset.
            assert(StackSize);
            MMI.addFrameInst(
                    MCCFIInstruction::createDefCfaOffset(FrameLabel, 2 * stackGrowth));

            // Change the rule for the FramePtr to be an "offset" rule.
            unsigned DwarfFramePtr = RegInfo->getDwarfRegNum(FramePtr, true);
            MMI.addFrameInst(MCCFIInstruction::createOffset(FrameLabel, DwarfFramePtr,
                        2 * stackGrowth));
        }

        // Update EBP with the new base value.
        BuildMI(MBB, MBBI, DL,
                TII.get(Cse523::MOV64rr), FramePtr)
            .addReg(StackPtr)
            .setMIFlag(MachineInstr::FrameSetup);

        if (needsFrameMoves) {
            // Mark effective beginning of when frame pointer becomes valid.
            MCSymbol *FrameLabel = MMI.getContext().CreateTempSymbol();
            BuildMI(MBB, MBBI, DL, TII.get(Cse523::PROLOG_LABEL))
                .addSym(FrameLabel);

            // Define the current CFA to use the EBP/RBP register.
            unsigned DwarfFramePtr = RegInfo->getDwarfRegNum(FramePtr, true);
            MMI.addFrameInst(
                    MCCFIInstruction::createDefCfaRegister(FrameLabel, DwarfFramePtr));
        }

        // Mark the FramePtr as live-in in every block except the entry.
        for (MachineFunction::iterator I = llvm::next(MF.begin()), E = MF.end();
                I != E; ++I)
            I->addLiveIn(FramePtr);
    } else {
        NumBytes = StackSize - Cse523FI->getCalleeSavedFrameSize();
    }

    // Skip the callee-saved push instructions.
    bool PushedRegs = false;
    int StackOffset = 2 * stackGrowth;

    while (MBBI != MBB.end() &&
             (MBBI->getOpcode() == Cse523::PUSH64r)) {
        PushedRegs = true;
        MBBI->setFlag(MachineInstr::FrameSetup);
        ++MBBI;

        if (!HasFP && needsFrameMoves) {
            // Mark callee-saved push instruction.
            MCSymbol *Label = MMI.getContext().CreateTempSymbol();
            BuildMI(MBB, MBBI, DL, TII.get(Cse523::PROLOG_LABEL)).addSym(Label);

            // Define the current CFA rule to use the provided offset.
            assert(StackSize);
            MMI.addFrameInst(
                    MCCFIInstruction::createDefCfaOffset(Label, StackOffset));
            StackOffset += stackGrowth;
        }
    }

    // Realign stack after we pushed callee-saved registers (so that we'll be
    // able to calculate their offsets from the frame pointer).

    // NOTE: We push the registers before realigning the stack, so
    // vector callee-saved (xmm) registers may be saved w/o proper
    // alignment in this way. However, currently these regs are saved in
    // stack slots (see Cse523FrameLowering::spillCalleeSavedRegisters()), so
    // this shouldn't be a problem.
    if (RegInfo->needsStackRealignment(MF)) {
        assert(0);
        assert(HasFP && "There should be a frame pointer if stack is realigned.");
        //MachineInstr *MI =
        //    BuildMI(MBB, MBBI, DL,
        //            TII.get(Cse523::AND64ri32, StackPtr)
        //    .addReg(StackPtr)
        //    .addImm(-MaxAlign)
        //    .setMIFlag(MachineInstr::FrameSetup);

        // The EFLAGS implicit def is dead.
        //MI->getOperand(3).setIsDead();
    }

    // If there is an SUB32ri of ESP immediately before this instruction, merge
    // the two. This can be the case when tail call elimination is enabled and
    // the callee has more arguments then the caller.
    NumBytes -= mergeSPUpdates(MBB, MBBI, StackPtr, true);

    // If there is an ADD32ri or SUB32ri of ESP immediately after this
    // instruction, merge the two instructions.
    mergeSPUpdatesDown(MBB, MBBI, StackPtr, &NumBytes);

    // Adjust stack pointer: ESP -= numbytes.

    // Windows and cygwin/mingw require a prologue helper routine when allocating
    // more than 4K bytes on the stack.  Windows uses __chkstk and cygwin/mingw
    // uses __alloca.  __alloca and the 32-bit version of __chkstk will probe the
    // stack and adjust the stack pointer in one go.  The 64-bit version of
    // __chkstk is only responsible for probing the stack.  The 64-bit prologue is
    // responsible for adjusting the stack pointer.  Touching the stack at 4K
    // increments is necessary to ensure that the guard pages used by the OS
    // virtual memory manager are allocated in correct sequence.
    if (NumBytes >= 4096 && STI.isOSWindows() && !STI.isTargetMacho()) {
        const char *StackProbeSymbol;

        if (STI.isTargetCygMing()) {
            StackProbeSymbol = "___chkstk_ms";
        } else {
            StackProbeSymbol = "__chkstk";
        }

        // Handle the 64-bit Windows ABI case where we need to call __chkstk.
        // Function prologue is responsible for adjusting the stack pointer.
        BuildMI(MBB, MBBI, DL, TII.get(Cse523::MOV64ri), Cse523::RAX)
            .addImm(NumBytes)
            .setMIFlag(MachineInstr::FrameSetup);

        BuildMI(MBB, MBBI, DL,
                TII.get(Cse523::W64ALLOCA))
            .addExternalSymbol(StackProbeSymbol)
            .addReg(StackPtr,    RegState::Define | RegState::Implicit)
            .addReg(Cse523::EFLAGS, RegState::Define | RegState::Implicit)
            .setMIFlag(MachineInstr::FrameSetup);

        // MSVC x64's __chkstk and cygwin/mingw's ___chkstk_ms do not adjust %rsp
        // themself. It also does not clobber %rax so we can reuse it when
        // adjusting %rsp.
        BuildMI(MBB, MBBI, DL, TII.get(Cse523::SUB64rr), StackPtr)
            .addReg(StackPtr)
            .addReg(Cse523::RAX)
            .setMIFlag(MachineInstr::FrameSetup);
    } else if (NumBytes)
        emitSPUpdate(MBB, MBBI, StackPtr, -(int64_t)NumBytes, Is64Bit, IsLP64,
                UseLEA, TII, *RegInfo);

    // If we need a base pointer, set it up here. It's whatever the value
    // of the stack pointer is at this point. Any variable size objects
    // will be allocated after this, so we can still use the base pointer
    // to reference locals.
    if (RegInfo->hasBasePointer(MF)) {
        // Update the frame pointer with the current stack pointer.
        unsigned Opc = Cse523::MOV64rr;
        BuildMI(MBB, MBBI, DL, TII.get(Opc), BasePtr)
            .addReg(StackPtr)
            .setMIFlag(MachineInstr::FrameSetup);
    }

    if (( (!HasFP && NumBytes) || PushedRegs) && needsFrameMoves) {
        // Mark end of stack pointer adjustment.
        MCSymbol *Label = MMI.getContext().CreateTempSymbol();
        BuildMI(MBB, MBBI, DL, TII.get(Cse523::PROLOG_LABEL))
            .addSym(Label);

        if (!HasFP && NumBytes) {
            // Define the current CFA rule to use the provided offset.
            assert(StackSize);
            MMI.addFrameInst(MCCFIInstruction::createDefCfaOffset(
                        Label, -StackSize + stackGrowth));
        }

        // Emit DWARF info specifying the offsets of the callee-saved registers.
        if (PushedRegs)
            emitCalleeSavedFrameMoves(MF, Label, HasFP ? FramePtr : StackPtr);
    }
}

void Cse523FrameLowering::emitEpilogue(MachineFunction &MF,
        MachineBasicBlock &MBB) const {
    const MachineFrameInfo *MFI = MF.getFrameInfo();
    Cse523MachineFunctionInfo *Cse523FI = MF.getInfo<Cse523MachineFunctionInfo>();
    const Cse523RegisterInfo *RegInfo = TM.getRegisterInfo();
    const Cse523InstrInfo &TII = *TM.getInstrInfo();
    MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
    assert(MBBI != MBB.end() && "Returning block has no instructions");
    unsigned RetOpcode = MBBI->getOpcode();
    DebugLoc DL = MBBI->getDebugLoc();
    bool Is64Bit = STI.is64Bit();
    bool IsLP64 = STI.isTarget64BitLP64();
    bool UseLEA = STI.useLeaForSP();
    unsigned StackAlign = getStackAlignment();
    unsigned SlotSize = RegInfo->getSlotSize();
    unsigned FramePtr = RegInfo->getFrameRegister(MF);
    unsigned StackPtr = RegInfo->getStackRegister();

    switch (RetOpcode) {
        default:
            llvm_unreachable("Can only insert epilog into returning blocks");
        case Cse523::RETQ:
        case Cse523::RETIQ:
        case Cse523::TCRETURNdi64:
        case Cse523::TCRETURNri64:
        case Cse523::TCRETURNmi64:
        case Cse523::EH_RETURN64:
            break;  // These are ok
    }

    // Get the number of bytes to allocate from the FrameInfo.
    uint64_t StackSize = MFI->getStackSize();
    uint64_t MaxAlign  = MFI->getMaxAlignment();
    unsigned CSSize = Cse523FI->getCalleeSavedFrameSize();
    uint64_t NumBytes = 0;

    // If we're forcing a stack realignment we can't rely on just the frame
    // info, we need to know the ABI stack alignment as well in case we
    // have a call out.  Otherwise just make sure we have some alignment - we'll
    // go with the minimum.
    if (ForceStackAlign) {
        if (MFI->hasCalls())
            MaxAlign = (StackAlign > MaxAlign) ? StackAlign : MaxAlign;
        else
            MaxAlign = MaxAlign ? MaxAlign : 4;
    }

    if (hasFP(MF)) {
        // Calculate required stack adjustment.
        uint64_t FrameSize = StackSize - SlotSize;
        if (RegInfo->needsStackRealignment(MF)) {
            // Callee-saved registers were pushed on stack before the stack
            // was realigned.
            FrameSize -= CSSize;
            NumBytes = (FrameSize + MaxAlign - 1) / MaxAlign * MaxAlign;
        } else {
            NumBytes = FrameSize - CSSize;
        }

        // Pop EBP.
        BuildMI(MBB, MBBI, DL,
                TII.get(Cse523::POP64r), FramePtr);
    } else {
        NumBytes = StackSize - CSSize;
    }

    // Skip the callee-saved pop instructions.
    while (MBBI != MBB.begin()) {
        MachineBasicBlock::iterator PI = prior(MBBI);
        unsigned Opc = PI->getOpcode();

        if (Opc != Cse523::POP64r && Opc != Cse523::DBG_VALUE &&
                !PI->isTerminator())
            break;

        --MBBI;
    }
    MachineBasicBlock::iterator FirstCSPop = MBBI;

    DL = MBBI->getDebugLoc();

    // If there is an ADD32ri or SUB32ri of ESP immediately before this
    // instruction, merge the two instructions.
    if (NumBytes || MFI->hasVarSizedObjects())
        mergeSPUpdatesUp(MBB, MBBI, StackPtr, &NumBytes);

    // If dynamic alloca is used, then reset esp to point to the last callee-saved
    // slot before popping them off! Same applies for the case, when stack was
    // realigned.
    if (RegInfo->needsStackRealignment(MF) || MFI->hasVarSizedObjects()) {
        if (RegInfo->needsStackRealignment(MF))
            MBBI = FirstCSPop;
        if (CSSize != 0) {
            unsigned Opc = getLEArOpcode(IsLP64);
            addRegOffset(BuildMI(MBB, MBBI, DL, TII.get(Opc), StackPtr),
                    FramePtr, false, -CSSize);
        } else {
            unsigned Opc = Cse523::MOV64rr;
            BuildMI(MBB, MBBI, DL, TII.get(Opc), StackPtr)
                .addReg(FramePtr);
        }
    } else if (NumBytes) {
        // Adjust stack pointer back: ESP += numbytes.
        emitSPUpdate(MBB, MBBI, StackPtr, NumBytes, Is64Bit, IsLP64, UseLEA,
                TII, *RegInfo);
    }

    // We're returning from function via eh_return.
    if (RetOpcode == Cse523::EH_RETURN64) {
        MBBI = MBB.getLastNonDebugInstr();
        MachineOperand &DestAddr  = MBBI->getOperand(0);
        assert(DestAddr.isReg() && "Offset should be in register!");
        BuildMI(MBB, MBBI, DL,
                TII.get(Cse523::MOV64rr),
                StackPtr).addReg(DestAddr.getReg());
    } else if (RetOpcode == Cse523::TCRETURNri64 || RetOpcode == Cse523::TCRETURNdi64 ||
            RetOpcode == Cse523::TCRETURNmi64) {
        bool isMem = RetOpcode == Cse523::TCRETURNmi64;
        // Tail call return: adjust the stack pointer and jump to callee.
        MBBI = MBB.getLastNonDebugInstr();
        MachineOperand &JumpTarget = MBBI->getOperand(0);
        MachineOperand &StackAdjust = MBBI->getOperand(isMem ? 5 : 1);
        assert(StackAdjust.isImm() && "Expecting immediate value.");

        // Adjust stack pointer.
        int StackAdj = StackAdjust.getImm();
        int MaxTCDelta = Cse523FI->getTCReturnAddrDelta();
        int Offset = 0;
        assert(MaxTCDelta <= 0 && "MaxTCDelta should never be positive");

        // Incoporate the retaddr area.
        Offset = StackAdj-MaxTCDelta;
        assert(Offset >= 0 && "Offset should never be negative");

        if (Offset) {
            // Check for possible merge with preceding ADD instruction.
            Offset += mergeSPUpdates(MBB, MBBI, StackPtr, true);
            emitSPUpdate(MBB, MBBI, StackPtr, Offset, Is64Bit, IsLP64,
                    UseLEA, TII, *RegInfo);
        }

        // Jump to label or value in register.
        if (RetOpcode == Cse523::TCRETURNdi64) {
            MachineInstrBuilder MIB =
                BuildMI(MBB, MBBI, DL, TII.get(Cse523::TAILJMPd64));
            if (JumpTarget.isGlobal())
                MIB.addGlobalAddress(JumpTarget.getGlobal(), JumpTarget.getOffset(),
                        JumpTarget.getTargetFlags());
            else {
                assert(JumpTarget.isSymbol());
                MIB.addExternalSymbol(JumpTarget.getSymbolName(),
                        JumpTarget.getTargetFlags());
            }
        } else if (RetOpcode == Cse523::TCRETURNmi64) {
            MachineInstrBuilder MIB =
                BuildMI(MBB, MBBI, DL, TII.get(Cse523::TAILJMPm64));
            for (unsigned i = 0; i != 5; ++i)
                MIB.addOperand(MBBI->getOperand(i));
        } else if (RetOpcode == Cse523::TCRETURNri64) {
            BuildMI(MBB, MBBI, DL, TII.get(Cse523::TAILJMPr64)).
                addReg(JumpTarget.getReg(), RegState::Kill);
        }

        MachineInstr *NewMI = prior(MBBI);
        NewMI->copyImplicitOps(MF, MBBI);

        // Delete the pseudo instruction TCRETURN.
        MBB.erase(MBBI);
    } else if ((RetOpcode == Cse523::RETQ || RetOpcode == Cse523::RETIQ) &&
            (Cse523FI->getTCReturnAddrDelta() < 0)) {
        // Add the return addr area delta back since we are not tail calling.
        int delta = -1*Cse523FI->getTCReturnAddrDelta();
        MBBI = MBB.getLastNonDebugInstr();

        // Check for possible merge with preceding ADD instruction.
        delta += mergeSPUpdates(MBB, MBBI, StackPtr, true);
        emitSPUpdate(MBB, MBBI, StackPtr, delta, Is64Bit, IsLP64, UseLEA, TII,
                *RegInfo);
    }
}

int Cse523FrameLowering::getFrameIndexOffset(const MachineFunction &MF, int FI) const {
    const Cse523RegisterInfo *RegInfo =
        static_cast<const Cse523RegisterInfo*>(MF.getTarget().getRegisterInfo());
    const MachineFrameInfo *MFI = MF.getFrameInfo();
    int Offset = MFI->getObjectOffset(FI) - getOffsetOfLocalArea();
    uint64_t StackSize = MFI->getStackSize();

    if (RegInfo->hasBasePointer(MF)) {
        assert (hasFP(MF) && "VLAs and dynamic stack realign, but no FP?!");
        if (FI < 0) {
            // Skip the saved EBP.
            return Offset + RegInfo->getSlotSize();
        } else {
            assert((-(Offset + StackSize)) % MFI->getObjectAlignment(FI) == 0);
            return Offset + StackSize;
        }
    } else if (RegInfo->needsStackRealignment(MF)) {
        if (FI < 0) {
            // Skip the saved EBP.
            return Offset + RegInfo->getSlotSize();
        } else {
            assert((-(Offset + StackSize)) % MFI->getObjectAlignment(FI) == 0);
            return Offset + StackSize;
        }
        // FIXME: Support tail calls
    } else {
        if (!hasFP(MF))
            return Offset + StackSize;

        // Skip the saved EBP.
        Offset += RegInfo->getSlotSize();

        // Skip the RETADDR move area
        const Cse523MachineFunctionInfo *Cse523FI = MF.getInfo<Cse523MachineFunctionInfo>();
        int TailCallReturnAddrDelta = Cse523FI->getTCReturnAddrDelta();
        if (TailCallReturnAddrDelta < 0)
            Offset -= TailCallReturnAddrDelta;
    }

    return Offset;
}

int Cse523FrameLowering::getFrameIndexReference(const MachineFunction &MF, int FI,
        unsigned &FrameReg) const {
    const Cse523RegisterInfo *RegInfo =
        static_cast<const Cse523RegisterInfo*>(MF.getTarget().getRegisterInfo());
    // We can't calculate offset from frame pointer if the stack is realigned,
    // so enforce usage of stack/base pointer.  The base pointer is used when we
    // have dynamic allocas in addition to dynamic realignment.
    if (RegInfo->hasBasePointer(MF))
        FrameReg = RegInfo->getBaseRegister();
    else if (RegInfo->needsStackRealignment(MF))
        FrameReg = RegInfo->getStackRegister();
    else
        FrameReg = RegInfo->getFrameRegister(MF);
    return getFrameIndexOffset(MF, FI);
}

bool Cse523FrameLowering::spillCalleeSavedRegisters(MachineBasicBlock &MBB,
        MachineBasicBlock::iterator MI,
        const std::vector<CalleeSavedInfo> &CSI,
        const TargetRegisterInfo *TRI) const {
    if (CSI.empty())
        return false;

    DebugLoc DL = MBB.findDebugLoc(MI);

    MachineFunction &MF = *MBB.getParent();

    unsigned SlotSize = STI.is64Bit() ? 8 : 4;
    unsigned FPReg = TRI->getFrameRegister(MF);
    unsigned CalleeFrameSize = 0;

    const TargetInstrInfo &TII = *MF.getTarget().getInstrInfo();
    Cse523MachineFunctionInfo *Cse523FI = MF.getInfo<Cse523MachineFunctionInfo>();

    // Push GPRs. It increases frame size.
    unsigned Opc = Cse523::PUSH64r;
    for (unsigned i = CSI.size(); i != 0; --i) {
        unsigned Reg = CSI[i-1].getReg();
        if (!Cse523::GR64RegClass.contains(Reg))
            continue;
        // Add the callee-saved register as live-in. It's killed at the spill.
        MBB.addLiveIn(Reg);
        if (Reg == FPReg)
            // Cse523RegisterInfo::emitPrologue will handle spilling of frame register.
            continue;
        CalleeFrameSize += SlotSize;
        BuildMI(MBB, MI, DL, TII.get(Opc)).addReg(Reg, RegState::Kill)
            .setMIFlag(MachineInstr::FrameSetup);
    }

    Cse523FI->setCalleeSavedFrameSize(CalleeFrameSize);

    // Make XMM regs spilled. Cse523 does not have ability of push/pop XMM.
    // It can be done by spilling XMMs to stack frame.
    // Note that only Win64 ABI might spill XMMs.
    for (unsigned i = CSI.size(); i != 0; --i) {
        unsigned Reg = CSI[i-1].getReg();
        if (Cse523::GR64RegClass.contains(Reg))
            continue;
        // Add the callee-saved register as live-in. It's killed at the spill.
        MBB.addLiveIn(Reg);
        const TargetRegisterClass *RC = TRI->getMinimalPhysRegClass(Reg);
        TII.storeRegToStackSlot(MBB, MI, Reg, true, CSI[i-1].getFrameIdx(),
                RC, TRI);
    }

    return true;
}

bool Cse523FrameLowering::restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
        MachineBasicBlock::iterator MI,
        const std::vector<CalleeSavedInfo> &CSI,
        const TargetRegisterInfo *TRI) const {
    if (CSI.empty())
        return false;

    DebugLoc DL = MBB.findDebugLoc(MI);

    MachineFunction &MF = *MBB.getParent();
    const TargetInstrInfo &TII = *MF.getTarget().getInstrInfo();

    // Reload XMMs from stack frame.
    for (unsigned i = 0, e = CSI.size(); i != e; ++i) {
        unsigned Reg = CSI[i].getReg();
        if (Cse523::GR64RegClass.contains(Reg))
            continue;
        const TargetRegisterClass *RC = TRI->getMinimalPhysRegClass(Reg);
        TII.loadRegFromStackSlot(MBB, MI, Reg, CSI[i].getFrameIdx(),
                RC, TRI);
    }

    // POP GPRs.
    unsigned FPReg = TRI->getFrameRegister(MF);
    unsigned Opc = Cse523::POP64r;
    for (unsigned i = 0, e = CSI.size(); i != e; ++i) {
        unsigned Reg = CSI[i].getReg();
        if (!Cse523::GR64RegClass.contains(Reg))
            continue;
        if (Reg == FPReg)
            // Cse523RegisterInfo::emitEpilogue will handle restoring of frame register.
            continue;
        BuildMI(MBB, MI, DL, TII.get(Opc), Reg);
    }
    return true;
}

void
Cse523FrameLowering::processFunctionBeforeCalleeSavedScan(MachineFunction &MF,
        RegScavenger *RS) const {
    MachineFrameInfo *MFI = MF.getFrameInfo();
    const Cse523RegisterInfo *RegInfo = TM.getRegisterInfo();
    unsigned SlotSize = RegInfo->getSlotSize();

    Cse523MachineFunctionInfo *Cse523FI = MF.getInfo<Cse523MachineFunctionInfo>();
    int64_t TailCallReturnAddrDelta = Cse523FI->getTCReturnAddrDelta();

    if (TailCallReturnAddrDelta < 0) {
        // create RETURNADDR area
        //   arg
        //   arg
        //   RETADDR
        //   { ...
        //     RETADDR area
        //     ...
        //   }
        //   [EBP]
        MFI->CreateFixedObject(-TailCallReturnAddrDelta,
                TailCallReturnAddrDelta - SlotSize, true);
    }

    if (hasFP(MF)) {
        assert((TailCallReturnAddrDelta <= 0) &&
                "The Delta should always be zero or negative");
        const TargetFrameLowering &TFI = *MF.getTarget().getFrameLowering();

        // Create a frame entry for the EBP register that must be saved.
        int FrameIdx = MFI->CreateFixedObject(SlotSize,
                -(int)SlotSize +
                TFI.getOffsetOfLocalArea() +
                TailCallReturnAddrDelta,
                true);
        assert(FrameIdx == MFI->getObjectIndexBegin() &&
                "Slot for EBP register must be last in order to be found!");
        (void)FrameIdx;
    }

    // Spill the BasePtr if it's used.
    if (RegInfo->hasBasePointer(MF))
        MF.getRegInfo().setPhysRegUsed(RegInfo->getBaseRegister());
}

static bool
HasNestArgument(const MachineFunction *MF) {
    const Function *F = MF->getFunction();
    for (Function::const_arg_iterator I = F->arg_begin(), E = F->arg_end();
            I != E; I++) {
        if (I->hasNestAttr())
            return true;
    }
    return false;
}

/// GetScratchRegister - Get a temp register for performing work in the
/// segmented stack and the Erlang/HiPE stack prologue. Depending on platform
/// and the properties of the function either one or two registers will be
/// needed. Set primary to true for the first register, false for the second.
static unsigned
GetScratchRegister(bool Is64Bit, const MachineFunction &MF, bool Primary) {
    CallingConv::ID CallingConvention = MF.getFunction()->getCallingConv();

    // Erlang stuff.
    if (CallingConvention == CallingConv::HiPE) {
        return Primary ? Cse523::R14 : Cse523::R13;
    }
    return Primary ? Cse523::R11 : Cse523::R12;
}

// The stack limit in the TCB is set to this many bytes above the actual stack
// limit.
static const uint64_t kSplitStackAvailable = 256;

void
Cse523FrameLowering::adjustForSegmentedStacks(MachineFunction &MF) const {
    MachineBasicBlock &prologueMBB = MF.front();
    MachineFrameInfo *MFI = MF.getFrameInfo();
    const Cse523InstrInfo &TII = *TM.getInstrInfo();
    uint64_t StackSize;
    bool Is64Bit = STI.is64Bit();
    unsigned TlsReg, TlsOffset;
    DebugLoc DL;

    unsigned ScratchReg = GetScratchRegister(Is64Bit, MF, true);
    assert(!MF.getRegInfo().isLiveIn(ScratchReg) &&
            "Scratch register is live-in");

    if (MF.getFunction()->isVarArg())
        report_fatal_error("Segmented stacks do not support vararg functions.");
    if (!STI.isTargetLinux() && !STI.isTargetDarwin() &&
            !STI.isTargetWin32() && !STI.isTargetFreeBSD())
        report_fatal_error("Segmented stacks not supported on this platform.");

    MachineBasicBlock *allocMBB = MF.CreateMachineBasicBlock();
    MachineBasicBlock *checkMBB = MF.CreateMachineBasicBlock();
    Cse523MachineFunctionInfo *Cse523FI = MF.getInfo<Cse523MachineFunctionInfo>();
    bool IsNested = false;

    // We need to know if the function has a nest argument only in 64 bit mode.
    if (Is64Bit)
        IsNested = HasNestArgument(&MF);

    // The MOV R10, RAX needs to be in a different block, since the RET we emit in
    // allocMBB needs to be last (terminating) instruction.

    for (MachineBasicBlock::livein_iterator i = prologueMBB.livein_begin(),
            e = prologueMBB.livein_end(); i != e; i++) {
        allocMBB->addLiveIn(*i);
        checkMBB->addLiveIn(*i);
    }

    if (IsNested)
        allocMBB->addLiveIn(Cse523::R10);

    MF.push_front(allocMBB);
    MF.push_front(checkMBB);

    // Eventually StackSize will be calculated by a link-time pass; which will
    // also decide whether checking code needs to be injected into this particular
    // prologue.
    StackSize = MFI->getStackSize();

    // When the frame size is less than 256 we just compare the stack
    // boundary directly to the value of the stack pointer, per gcc.
    bool CompareStackPointer = StackSize < kSplitStackAvailable;

    // Read the limit off the current stacklet off the stack_guard location.
    if (STI.isTargetLinux()) {
        TlsReg = Cse523::FS;
        TlsOffset = 0x70;
    } else if (STI.isTargetDarwin()) {
        TlsReg = Cse523::GS;
        TlsOffset = 0x60 + 90*8; // See pthread_machdep.h. Steal TLS slot 90.
    } else if (STI.isTargetFreeBSD()) {
        TlsReg = Cse523::FS;
        TlsOffset = 0x18;
    } else {
        report_fatal_error("Segmented stacks not supported on this platform.");
    }

    if (CompareStackPointer)
        ScratchReg = Cse523::RSP;
    else
        BuildMI(checkMBB, DL, TII.get(Cse523::LEA64r), ScratchReg).addReg(Cse523::RSP)
            .addImm(1).addReg(0).addImm(-StackSize).addReg(0);

    assert(0 && "Need to support CMP64rm!");
    //BuildMI(checkMBB, DL, TII.get(Cse523::CMP64rm)).addReg(ScratchReg)
    //    .addReg(0).addImm(1).addReg(0).addImm(TlsOffset).addReg(TlsReg);

    // This jump is taken if SP >= (Stacklet Limit + Stack Space required).
    // It jumps to normal execution of the function body.
    //BuildMI(checkMBB, DL, TII.get(Cse523::JA_4)).addMBB(&prologueMBB);

    // On 32 bit we first push the arguments size and then the frame size. On 64
    // bit, we pass the stack frame size in r10 and the argument size in r11.

    // Functions with nested arguments use R10, so it needs to be saved across
    // the call to _morestack

    if (IsNested)
        BuildMI(allocMBB, DL, TII.get(Cse523::MOV64rr), Cse523::RAX).addReg(Cse523::R10);

    BuildMI(allocMBB, DL, TII.get(Cse523::MOV64ri), Cse523::R10)
        .addImm(StackSize);
    BuildMI(allocMBB, DL, TII.get(Cse523::MOV64ri), Cse523::R11)
        .addImm(Cse523FI->getArgumentStackSize());
    MF.getRegInfo().setPhysRegUsed(Cse523::R10);
    MF.getRegInfo().setPhysRegUsed(Cse523::R11);

    // __morestack is in libgcc
    BuildMI(allocMBB, DL, TII.get(Cse523::CALL64pcrel32))
        .addExternalSymbol("__morestack");

    if (IsNested)
        BuildMI(allocMBB, DL, TII.get(Cse523::MORESTACK_RET_RESTORE_R10));
    else
        BuildMI(allocMBB, DL, TII.get(Cse523::MORESTACK_RET));

    allocMBB->addSuccessor(&prologueMBB);

    checkMBB->addSuccessor(allocMBB);
    checkMBB->addSuccessor(&prologueMBB);

#ifdef XDEBUG
    MF.verify();
#endif
}

/// Erlang programs may need a special prologue to handle the stack size they
/// might need at runtime. That is because Erlang/OTP does not implement a C
/// stack but uses a custom implementation of hybrid stack/heap architecture.
/// (for more information see Eric Stenman's Ph.D. thesis:
/// http://publications.uu.se/uu/fulltext/nbn_se_uu_diva-2688.pdf)
///
/// CheckStack:
///	  temp0 = sp - MaxStack
///	  if( temp0 < SP_LIMIT(P) ) goto IncStack else goto OldStart
/// OldStart:
///	  ...
/// IncStack:
///	  call inc_stack   # doubles the stack space
///	  temp0 = sp - MaxStack
///	  if( temp0 < SP_LIMIT(P) ) goto IncStack else goto OldStart
void Cse523FrameLowering::adjustForHiPEPrologue(MachineFunction &MF) const {
    const Cse523InstrInfo &TII = *TM.getInstrInfo();
    MachineFrameInfo *MFI = MF.getFrameInfo();
    const unsigned SlotSize = TM.getRegisterInfo()->getSlotSize();
    const bool Is64Bit = STI.is64Bit();
    DebugLoc DL;
    // HiPE-specific values
    const unsigned HipeLeafWords = 24;
    const unsigned CCRegisteredArgs = Is64Bit ? 6 : 5;
    const unsigned Guaranteed = HipeLeafWords * SlotSize;
    unsigned CallerStkArity = MF.getFunction()->arg_size() > CCRegisteredArgs ?
        MF.getFunction()->arg_size() - CCRegisteredArgs : 0;
    unsigned MaxStack = MFI->getStackSize() + CallerStkArity*SlotSize + SlotSize;

    assert(STI.isTargetLinux() &&
            "HiPE prologue is only supported on Linux operating systems.");

    // Compute the largest caller's frame that is needed to fit the callees'
    // frames. This 'MaxStack' is computed from:
    //
    // a) the fixed frame size, which is the space needed for all spilled temps,
    // b) outgoing on-stack parameter areas, and
    // c) the minimum stack space this function needs to make available for the
    //    functions it calls (a tunable ABI property).
    if (MFI->hasCalls()) {
        unsigned MoreStackForCalls = 0;

        for (MachineFunction::iterator MBBI = MF.begin(), MBBE = MF.end();
                MBBI != MBBE; ++MBBI)
            for (MachineBasicBlock::iterator MI = MBBI->begin(), ME = MBBI->end();
                    MI != ME; ++MI) {
                if (!MI->isCall())
                    continue;

                // Get callee operand.
                const MachineOperand &MO = MI->getOperand(0);

                // Only take account of global function calls (no closures etc.).
                if (!MO.isGlobal())
                    continue;

                const Function *F = dyn_cast<Function>(MO.getGlobal());
                if (!F)
                    continue;

                // Do not update 'MaxStack' for primitive and built-in functions
                // (encoded with names either starting with "erlang."/"bif_" or not
                // having a ".", such as a simple <Module>.<Function>.<Arity>, or an
                // "_", such as the BIF "suspend_0") as they are executed on another
                // stack.
                if (F->getName().find("erlang.") != StringRef::npos ||
                        F->getName().find("bif_") != StringRef::npos ||
                        F->getName().find_first_of("._") == StringRef::npos)
                    continue;

                unsigned CalleeStkArity =
                    F->arg_size() > CCRegisteredArgs ? F->arg_size()-CCRegisteredArgs : 0;
                if (HipeLeafWords - 1 > CalleeStkArity)
                    MoreStackForCalls = std::max(MoreStackForCalls,
                            (HipeLeafWords - 1 - CalleeStkArity) * SlotSize);
            }
        MaxStack += MoreStackForCalls;
    }
    assert(0);

    // If the stack frame needed is larger than the guaranteed then runtime checks
    // and calls to "inc_stack_0" BIF should be inserted in the assembly prologue.
    //if (MaxStack > Guaranteed) {
    //    MachineBasicBlock &prologueMBB = MF.front();
    //    MachineBasicBlock *stackCheckMBB = MF.CreateMachineBasicBlock();
    //    MachineBasicBlock *incStackMBB = MF.CreateMachineBasicBlock();

    //    for (MachineBasicBlock::livein_iterator I = prologueMBB.livein_begin(),
    //            E = prologueMBB.livein_end(); I != E; I++) {
    //        stackCheckMBB->addLiveIn(*I);
    //        incStackMBB->addLiveIn(*I);
    //    }

    //    MF.push_front(incStackMBB);
    //    MF.push_front(stackCheckMBB);

    //    unsigned ScratchReg, SPReg, PReg, SPLimitOffset;
    //    unsigned LEAop, CMPop, CALLop;

    //    SPReg = Cse523::RSP;
    //    PReg  = Cse523::RBP;
    //    LEAop = Cse523::LEA64r;
    //    CMPop = Cse523::CMP64rm;
    //    CALLop = Cse523::CALL64pcrel32;
    //    SPLimitOffset = 0x90;

    //    ScratchReg = GetScratchRegister(Is64Bit, MF, true);
    //    assert(!MF.getRegInfo().isLiveIn(ScratchReg) &&
    //            "HiPE prologue scratch register is live-in");

    //    // Create new MBB for StackCheck:
    //    addRegOffset(BuildMI(stackCheckMBB, DL, TII.get(LEAop), ScratchReg),
    //            SPReg, false, -MaxStack);
    //    // SPLimitOffset is in a fixed heap location (pointed by BP).
    //    addRegOffset(BuildMI(stackCheckMBB, DL, TII.get(CMPop))
    //            .addReg(ScratchReg), PReg, false, SPLimitOffset);
    //    BuildMI(stackCheckMBB, DL, TII.get(Cse523::JAE_4)).addMBB(&prologueMBB);

    //    // Create new MBB for IncStack:
    //    BuildMI(incStackMBB, DL, TII.get(CALLop)).
    //        addExternalSymbol("inc_stack_0");
    //    addRegOffset(BuildMI(incStackMBB, DL, TII.get(LEAop), ScratchReg),
    //            SPReg, false, -MaxStack);
    //    addRegOffset(BuildMI(incStackMBB, DL, TII.get(CMPop))
    //            .addReg(ScratchReg), PReg, false, SPLimitOffset);
    //    BuildMI(incStackMBB, DL, TII.get(Cse523::JLE_4)).addMBB(incStackMBB);

    //    stackCheckMBB->addSuccessor(&prologueMBB, 99);
    //    stackCheckMBB->addSuccessor(incStackMBB, 1);
    //    incStackMBB->addSuccessor(&prologueMBB, 99);
    //    incStackMBB->addSuccessor(incStackMBB, 1);
    //}
#ifdef XDEBUG
    MF.verify();
#endif
}

void Cse523FrameLowering::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
        MachineBasicBlock::iterator I) const {
    const Cse523InstrInfo &TII = *TM.getInstrInfo();
    const Cse523RegisterInfo &RegInfo = *TM.getRegisterInfo();
    unsigned StackPtr = RegInfo.getStackRegister();
    bool reseveCallFrame = hasReservedCallFrame(MF);
    int Opcode = I->getOpcode();
    bool isDestroy = Opcode == TII.getCallFrameDestroyOpcode();
    bool IsLP64 = STI.isTarget64BitLP64();
    DebugLoc DL = I->getDebugLoc();
    uint64_t Amount = !reseveCallFrame ? I->getOperand(0).getImm() : 0;
    uint64_t CalleeAmt = isDestroy ? I->getOperand(1).getImm() : 0;
    I = MBB.erase(I);

    if (!reseveCallFrame) {
        // If the stack pointer can be changed after prologue, turn the
        // adjcallstackup instruction into a 'sub ESP, <amt>' and the
        // adjcallstackdown instruction into 'add ESP, <amt>'
        // TODO: consider using push / pop instead of sub + store / add
        if (Amount == 0)
            return;

        // We need to keep the stack aligned properly.  To do this, we round the
        // amount of space needed for the outgoing arguments up to the next
        // alignment boundary.
        unsigned StackAlign = TM.getFrameLowering()->getStackAlignment();
        Amount = (Amount + StackAlign - 1) / StackAlign * StackAlign;

        MachineInstr *New = 0;
        if (Opcode == TII.getCallFrameSetupOpcode()) {
            New = BuildMI(MF, DL, TII.get(getSUBriOpcode(IsLP64, Amount)),
                    StackPtr)
                .addReg(StackPtr)
                .addImm(Amount);
        } else {
            assert(Opcode == TII.getCallFrameDestroyOpcode());

            // Factor out the amount the callee already popped.
            Amount -= CalleeAmt;

            if (Amount) {
                unsigned Opc = getADDriOpcode(IsLP64, Amount);
                New = BuildMI(MF, DL, TII.get(Opc), StackPtr)
                    .addReg(StackPtr).addImm(Amount);
            }
        }

        if (New) {
            // The EFLAGS implicit def is dead.
            New->getOperand(3).setIsDead();

            // Replace the pseudo instruction with a new instruction.
            MBB.insert(I, New);
        }

        return;
    }

    if (Opcode == TII.getCallFrameDestroyOpcode() && CalleeAmt) {
        // If we are performing frame pointer elimination and if the callee pops
        // something off the stack pointer, add it back.  We do this until we have
        // more advanced stack pointer tracking ability.
        unsigned Opc = getSUBriOpcode(IsLP64, CalleeAmt);
        MachineInstr *New = BuildMI(MF, DL, TII.get(Opc), StackPtr)
            .addReg(StackPtr).addImm(CalleeAmt);

        // The EFLAGS implicit def is dead.
        New->getOperand(3).setIsDead();

        // We are not tracking the stack pointer adjustment by the callee, so make
        // sure we restore the stack pointer immediately after the call, there may
        // be spill code inserted between the CALL and ADJCALLSTACKUP instructions.
        MachineBasicBlock::iterator B = MBB.begin();
        while (I != B && !llvm::prior(I)->isCall())
            --I;
        MBB.insert(I, New);
    }
}

