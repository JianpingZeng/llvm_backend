//===-- Cse523MCInstLower.cpp - Convert Cse523 MachineInstr to an MCInst --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower Cse523 MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "Cse523AsmPrinter.h"
#include "InstPrinter/Cse523ATTInstPrinter.h"
#include "Cse523COFFMachineModuleInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/CodeGen/StackMaps.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Type.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

namespace {

    /// Cse523MCInstLower - This class is used to lower an MachineInstr into an MCInst.
    class Cse523MCInstLower {
        MCContext &Ctx;
        const MachineFunction &MF;
        const TargetMachine &TM;
        const MCAsmInfo &MAI;
        Cse523AsmPrinter &AsmPrinter;
        public:
        Cse523MCInstLower(const MachineFunction &MF, Cse523AsmPrinter &asmprinter);

        void Lower(const MachineInstr *MI, MCInst &OutMI) const;

        MCSymbol *GetSymbolFromOperand(const MachineOperand &MO) const;
        MCOperand LowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;

        private:
        MachineModuleInfoMachO &getMachOMMI() const;
        Mangler *getMang() const {
            return AsmPrinter.Mang;
        }
    };

} // end anonymous namespace

Cse523MCInstLower::Cse523MCInstLower(const MachineFunction &mf,
        Cse523AsmPrinter &asmprinter)
    : Ctx(mf.getContext()), MF(mf), TM(mf.getTarget()),
    MAI(*TM.getMCAsmInfo()), AsmPrinter(asmprinter) {}

    MachineModuleInfoMachO &Cse523MCInstLower::getMachOMMI() const {
        return MF.getMMI().getObjFileInfo<MachineModuleInfoMachO>();
    }


/// GetSymbolFromOperand - Lower an MO_GlobalAddress or MO_ExternalSymbol
/// operand to an MCSymbol.
MCSymbol *Cse523MCInstLower::
GetSymbolFromOperand(const MachineOperand &MO) const {
    const DataLayout *DL = TM.getDataLayout();
    assert((MO.isGlobal() || MO.isSymbol() || MO.isMBB()) && "Isn't a symbol reference");

    SmallString<128> Name;
    StringRef Suffix;

    switch (MO.getTargetFlags()) {
        case Cse523II::MO_DLLIMPORT:
            // Handle dllimport linkage.
            Name += "__imp_";
            break;
        case Cse523II::MO_DARWIN_STUB:
            Suffix = "$stub";
            break;
        case Cse523II::MO_DARWIN_NONLAZY:
        case Cse523II::MO_DARWIN_NONLAZY_PIC_BASE:
        case Cse523II::MO_DARWIN_HIDDEN_NONLAZY_PIC_BASE:
            Suffix = "$non_lazy_ptr";
            break;
    }

    if (!Suffix.empty())
        Name += DL->getPrivateGlobalPrefix();

    unsigned PrefixLen = Name.size();

    if (MO.isGlobal()) {
        const GlobalValue *GV = MO.getGlobal();
        AsmPrinter.getNameWithPrefix(Name, GV);
    } else if (MO.isSymbol()) {
        getMang()->getNameWithPrefix(Name, MO.getSymbolName());
    } else if (MO.isMBB()) {
        Name += MO.getMBB()->getSymbol()->getName();
    }
    unsigned OrigLen = Name.size() - PrefixLen;

    Name += Suffix;
    MCSymbol *Sym = Ctx.GetOrCreateSymbol(Name);

    StringRef OrigName = StringRef(Name).substr(PrefixLen, OrigLen);

    // If the target flags on the operand changes the name of the symbol, do that
    // before we return the symbol.
    switch (MO.getTargetFlags()) {
        default: break;
        case Cse523II::MO_DARWIN_NONLAZY:
        case Cse523II::MO_DARWIN_NONLAZY_PIC_BASE: {
                                                       MachineModuleInfoImpl::StubValueTy &StubSym =
                                                           getMachOMMI().getGVStubEntry(Sym);
                                                       if (StubSym.getPointer() == 0) {
                                                           assert(MO.isGlobal() && "Extern symbol not handled yet");
                                                           StubSym =
                                                               MachineModuleInfoImpl::
                                                               StubValueTy(AsmPrinter.getSymbol(MO.getGlobal()),
                                                                       !MO.getGlobal()->hasInternalLinkage());
                                                       }
                                                       break;
                                                   }
        case Cse523II::MO_DARWIN_HIDDEN_NONLAZY_PIC_BASE: {
                                                              MachineModuleInfoImpl::StubValueTy &StubSym =
                                                                  getMachOMMI().getHiddenGVStubEntry(Sym);
                                                              if (StubSym.getPointer() == 0) {
                                                                  assert(MO.isGlobal() && "Extern symbol not handled yet");
                                                                  StubSym =
                                                                      MachineModuleInfoImpl::
                                                                      StubValueTy(AsmPrinter.getSymbol(MO.getGlobal()),
                                                                              !MO.getGlobal()->hasInternalLinkage());
                                                              }
                                                              break;
                                                          }
        case Cse523II::MO_DARWIN_STUB: {
                                           MachineModuleInfoImpl::StubValueTy &StubSym =
                                               getMachOMMI().getFnStubEntry(Sym);
                                           if (StubSym.getPointer())
                                               return Sym;

                                           if (MO.isGlobal()) {
                                               StubSym =
                                                   MachineModuleInfoImpl::
                                                   StubValueTy(AsmPrinter.getSymbol(MO.getGlobal()),
                                                           !MO.getGlobal()->hasInternalLinkage());
                                           } else {
                                               StubSym =
                                                   MachineModuleInfoImpl::
                                                   StubValueTy(Ctx.GetOrCreateSymbol(OrigName), false);
                                           }
                                           break;
                                       }
    }

    return Sym;
}

MCOperand Cse523MCInstLower::LowerSymbolOperand(const MachineOperand &MO,
        MCSymbol *Sym) const {
    // FIXME: We would like an efficient form for this, so we don't have to do a
    // lot of extra uniquing.
    const MCExpr *Expr = 0;
    MCSymbolRefExpr::VariantKind RefKind = MCSymbolRefExpr::VK_None;

    switch (MO.getTargetFlags()) {
        default: llvm_unreachable("Unknown target flag on GV operand");
        case Cse523II::MO_NO_FLAG:    // No flag.
                 // These affect the name of the symbol, not any suffix.
        case Cse523II::MO_DARWIN_NONLAZY:
        case Cse523II::MO_DLLIMPORT:
        case Cse523II::MO_DARWIN_STUB:
                 break;

        case Cse523II::MO_TLVP:      RefKind = MCSymbolRefExpr::VK_TLVP; break;
        case Cse523II::MO_TLVP_PIC_BASE:
                                     Expr = MCSymbolRefExpr::Create(Sym, MCSymbolRefExpr::VK_TLVP, Ctx);
                                     // Subtract the pic base.
                                     Expr = MCBinaryExpr::CreateSub(Expr,
                                             MCSymbolRefExpr::Create(MF.getPICBaseSymbol(),
                                                 Ctx),
                                             Ctx);
                                     break;
        case Cse523II::MO_SECREL:    RefKind = MCSymbolRefExpr::VK_SECREL; break;
        case Cse523II::MO_TLSGD:     RefKind = MCSymbolRefExpr::VK_TLSGD; break;
        case Cse523II::MO_TLSLD:     RefKind = MCSymbolRefExpr::VK_TLSLD; break;
        case Cse523II::MO_TLSLDM:    RefKind = MCSymbolRefExpr::VK_TLSLDM; break;
        case Cse523II::MO_GOTTPOFF:  RefKind = MCSymbolRefExpr::VK_GOTTPOFF; break;
        case Cse523II::MO_INDNTPOFF: RefKind = MCSymbolRefExpr::VK_INDNTPOFF; break;
        case Cse523II::MO_TPOFF:     RefKind = MCSymbolRefExpr::VK_TPOFF; break;
        case Cse523II::MO_DTPOFF:    RefKind = MCSymbolRefExpr::VK_DTPOFF; break;
        case Cse523II::MO_NTPOFF:    RefKind = MCSymbolRefExpr::VK_NTPOFF; break;
        case Cse523II::MO_GOTNTPOFF: RefKind = MCSymbolRefExpr::VK_GOTNTPOFF; break;
        case Cse523II::MO_GOTPCREL:  RefKind = MCSymbolRefExpr::VK_GOTPCREL; break;
        case Cse523II::MO_GOT:       RefKind = MCSymbolRefExpr::VK_GOT; break;
        case Cse523II::MO_GOTOFF:    RefKind = MCSymbolRefExpr::VK_GOTOFF; break;
        case Cse523II::MO_PLT:       RefKind = MCSymbolRefExpr::VK_PLT; break;
        case Cse523II::MO_PIC_BASE_OFFSET:
        case Cse523II::MO_DARWIN_NONLAZY_PIC_BASE:
        case Cse523II::MO_DARWIN_HIDDEN_NONLAZY_PIC_BASE:
                                     Expr = MCSymbolRefExpr::Create(Sym, Ctx);
                                     // Subtract the pic base.
                                     Expr = MCBinaryExpr::CreateSub(Expr,
                                             MCSymbolRefExpr::Create(MF.getPICBaseSymbol(), Ctx),
                                             Ctx);
                                     if (MO.isJTI() && MAI.hasSetDirective()) {
                                         // If .set directive is supported, use it to reduce the number of
                                         // relocations the assembler will generate for differences between
                                         // local labels. This is only safe when the symbols are in the same
                                         // section so we are restricting it to jumptable references.
                                         MCSymbol *Label = Ctx.CreateTempSymbol();
                                         AsmPrinter.OutStreamer.EmitAssignment(Label, Expr);
                                         Expr = MCSymbolRefExpr::Create(Label, Ctx);
                                     }
                                     break;
    }

    if (Expr == 0)
        Expr = MCSymbolRefExpr::Create(Sym, RefKind, Ctx);

    if (!MO.isJTI() && !MO.isMBB() && MO.getOffset())
        Expr = MCBinaryExpr::CreateAdd(Expr,
                MCConstantExpr::Create(MO.getOffset(), Ctx),
                Ctx);
    return MCOperand::CreateExpr(Expr);
}


/// \brief Simplify FOO $imm, %{al,ax,eax,rax} to FOO $imm, for instruction with
/// a short fixed-register form.
static void SimplifyShortImmForm(MCInst &Inst, unsigned Opcode) {
    unsigned ImmOp = Inst.getNumOperands() - 1;
    assert(Inst.getOperand(0).isReg() &&
            (Inst.getOperand(ImmOp).isImm() || Inst.getOperand(ImmOp).isExpr()) &&
            ((Inst.getNumOperands() == 3 && Inst.getOperand(1).isReg() &&
              Inst.getOperand(0).getReg() == Inst.getOperand(1).getReg()) ||
             Inst.getNumOperands() == 2) && "Unexpected instruction!");

    // Check whether the destination register can be fixed.
    unsigned Reg = Inst.getOperand(0).getReg();
    if (Reg != Cse523::RAX)
        return;

    // If so, rewrite the instruction.
    MCOperand Saved = Inst.getOperand(ImmOp);
    Inst = MCInst();
    Inst.setOpcode(Opcode);
    Inst.addOperand(Saved);
}

/// \brief If a movsx instruction has a shorter encoding for the used register
/// simplify the instruction to use it instead.
static void SimplifyMOVSX(MCInst &Inst) {
    unsigned NewOpcode = 0;
    unsigned Op0 = Inst.getOperand(0).getReg(), Op1 = Inst.getOperand(1).getReg();
    switch (Inst.getOpcode()) {
        default:
            llvm_unreachable("Unexpected instruction!");
            break;
        //case Cse523::MOVSX16rr8:  // movsbw %al, %ax   --> cbtw
        //    if (Op0 == Cse523::AX && Op1 == Cse523::AL)
        //        NewOpcode = Cse523::CBW;
        //    break;
        //case Cse523::MOVSX32rr16: // movswl %ax, %eax  --> cwtl
        //    if (Op0 == Cse523::EAX && Op1 == Cse523::AX)
        //        NewOpcode = Cse523::CWDE;
        //    break;
        //case Cse523::MOVSX64rr32: // movslq %eax, %rax --> cltq
        //    if (Op0 == Cse523::RAX && Op1 == Cse523::EAX)
        //        NewOpcode = Cse523::CDQE;
        //    break;
    }

    if (NewOpcode != 0) {
        Inst = MCInst();
        Inst.setOpcode(NewOpcode);
    }
}

/// \brief Simplify things like MOV32rm to MOV32o32a.
static void SimplifyShortMoveForm(Cse523AsmPrinter &Printer, MCInst &Inst,
        unsigned Opcode) {
    // Don't make these simplifications in 64-bit mode; other assemblers don't
    // perform them because they make the code larger.
    if (Printer.getSubtarget().is64Bit())
        return;

    bool IsStore = Inst.getOperand(0).isReg() && Inst.getOperand(1).isReg();
    unsigned AddrBase = IsStore;
    unsigned RegOp = IsStore ? 0 : 5;
    unsigned AddrOp = AddrBase + 3;
    assert(Inst.getNumOperands() == 6 && Inst.getOperand(RegOp).isReg() &&
            Inst.getOperand(AddrBase + 0).isReg() && // base
            Inst.getOperand(AddrBase + 1).isImm() && // scale
            Inst.getOperand(AddrBase + 2).isReg() && // index register
            (Inst.getOperand(AddrOp).isExpr() ||     // address
             Inst.getOperand(AddrOp).isImm())&&
            Inst.getOperand(AddrBase + 4).isReg() && // segment
            "Unexpected instruction!");

    // Check whether the destination register can be fixed.
    unsigned Reg = Inst.getOperand(RegOp).getReg();
    if (Reg != Cse523::RAX)
        return;

    // Check whether this is an absolute address.
    // FIXME: We know TLVP symbol refs aren't, but there should be a better way
    // to do this here.
    bool Absolute = true;
    if (Inst.getOperand(AddrOp).isExpr()) {
        const MCExpr *MCE = Inst.getOperand(AddrOp).getExpr();
        if (const MCSymbolRefExpr *SRE = dyn_cast<MCSymbolRefExpr>(MCE))
            if (SRE->getKind() == MCSymbolRefExpr::VK_TLVP)
                Absolute = false;
    }

    if (Absolute &&
            (Inst.getOperand(AddrBase + 0).getReg() != 0 ||
             Inst.getOperand(AddrBase + 2).getReg() != 0 ||
             Inst.getOperand(AddrBase + 1).getImm() != 1))
        return;

    // If so, rewrite the instruction.
    MCOperand Saved = Inst.getOperand(AddrOp);
    MCOperand Seg = Inst.getOperand(AddrBase + 4);
    Inst = MCInst();
    Inst.setOpcode(Opcode);
    Inst.addOperand(Saved);
    Inst.addOperand(Seg);
}

static unsigned getRetOpcode(const Cse523Subtarget &Subtarget)
{
    return Cse523::RETQ;
}

void Cse523MCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const {
    OutMI.setOpcode(MI->getOpcode());

    for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
        const MachineOperand &MO = MI->getOperand(i);

        MCOperand MCOp;
        switch (MO.getType()) {
            default:
                MI->dump();
                llvm_unreachable("unknown operand type");
            case MachineOperand::MO_Register:
                // Ignore all implicit register operands.
                if (MO.isImplicit()) continue;
                MCOp = MCOperand::CreateReg(MO.getReg());
                break;
            case MachineOperand::MO_Immediate:
                MCOp = MCOperand::CreateImm(MO.getImm());
                break;
            case MachineOperand::MO_MachineBasicBlock:
            case MachineOperand::MO_GlobalAddress:
            case MachineOperand::MO_ExternalSymbol:
                MCOp = LowerSymbolOperand(MO, GetSymbolFromOperand(MO));
                break;
            case MachineOperand::MO_JumpTableIndex:
                MCOp = LowerSymbolOperand(MO, AsmPrinter.GetJTISymbol(MO.getIndex()));
                break;
            case MachineOperand::MO_ConstantPoolIndex:
                MCOp = LowerSymbolOperand(MO, AsmPrinter.GetCPISymbol(MO.getIndex()));
                break;
            case MachineOperand::MO_BlockAddress:
                MCOp = LowerSymbolOperand(MO,
                        AsmPrinter.GetBlockAddressSymbol(MO.getBlockAddress()));
                break;
            case MachineOperand::MO_RegisterMask:
                // Ignore call clobbers.
                continue;
        }

        OutMI.addOperand(MCOp);
    }

    // Handle a few special cases to eliminate operand modifiers.
ReSimplify:
    switch (OutMI.getOpcode()) {
    //    case Cse523::LEA64_32r:
    //    case Cse523::LEA64r:
    //    case Cse523::LEA16r:
    //    case Cse523::LEA32r:
    //        // LEA should have a segment register, but it must be empty.
    //        assert(OutMI.getNumOperands() == 1+Cse523::AddrNumOperands &&
    //                "Unexpected # of LEA operands");
    //        assert(OutMI.getOperand(1+Cse523::AddrSegmentReg).getReg() == 0 &&
    //                "LEA has segment specified!");
    //        break;

    //    case Cse523::MOV32ri64:
    //        OutMI.setOpcode(Cse523::MOV32ri);
    //        break;

    //        // Commute operands to get a smaller encoding by using VEX.R instead of VEX.B
    //        // if one of the registers is extended, but other isn't.
    //    case Cse523::VMOVAPDrr:
    //    case Cse523::VMOVAPDYrr:
    //    case Cse523::VMOVAPSrr:
    //    case Cse523::VMOVAPSYrr:
    //    case Cse523::VMOVDQArr:
    //    case Cse523::VMOVDQAYrr:
    //    case Cse523::VMOVDQUrr:
    //    case Cse523::VMOVDQUYrr:
    //    case Cse523::VMOVUPDrr:
    //    case Cse523::VMOVUPDYrr:
    //    case Cse523::VMOVUPSrr:
    //    case Cse523::VMOVUPSYrr: {
    //                                 if (!Cse523II::isCse523_64ExtendedReg(OutMI.getOperand(0).getReg()) &&
    //                                         Cse523II::isCse523_64ExtendedReg(OutMI.getOperand(1).getReg())) {
    //                                     unsigned NewOpc;
    //                                     switch (OutMI.getOpcode()) {
    //                                         default: llvm_unreachable("Invalid opcode");
    //                                         case Cse523::VMOVAPDrr:  NewOpc = Cse523::VMOVAPDrr_REV;  break;
    //                                         case Cse523::VMOVAPDYrr: NewOpc = Cse523::VMOVAPDYrr_REV; break;
    //                                         case Cse523::VMOVAPSrr:  NewOpc = Cse523::VMOVAPSrr_REV;  break;
    //                                         case Cse523::VMOVAPSYrr: NewOpc = Cse523::VMOVAPSYrr_REV; break;
    //                                         case Cse523::VMOVDQArr:  NewOpc = Cse523::VMOVDQArr_REV;  break;
    //                                         case Cse523::VMOVDQAYrr: NewOpc = Cse523::VMOVDQAYrr_REV; break;
    //                                         case Cse523::VMOVDQUrr:  NewOpc = Cse523::VMOVDQUrr_REV;  break;
    //                                         case Cse523::VMOVDQUYrr: NewOpc = Cse523::VMOVDQUYrr_REV; break;
    //                                         case Cse523::VMOVUPDrr:  NewOpc = Cse523::VMOVUPDrr_REV;  break;
    //                                         case Cse523::VMOVUPDYrr: NewOpc = Cse523::VMOVUPDYrr_REV; break;
    //                                         case Cse523::VMOVUPSrr:  NewOpc = Cse523::VMOVUPSrr_REV;  break;
    //                                         case Cse523::VMOVUPSYrr: NewOpc = Cse523::VMOVUPSYrr_REV; break;
    //                                     }
    //                                     OutMI.setOpcode(NewOpc);
    //                                 }
    //                                 break;
    //                             }
    //    case Cse523::VMOVSDrr:
    //    case Cse523::VMOVSSrr: {
    //                               if (!Cse523II::isCse523_64ExtendedReg(OutMI.getOperand(0).getReg()) &&
    //                                       Cse523II::isCse523_64ExtendedReg(OutMI.getOperand(2).getReg())) {
    //                                   unsigned NewOpc;
    //                                   switch (OutMI.getOpcode()) {
    //                                       default: llvm_unreachable("Invalid opcode");
    //                                       case Cse523::VMOVSDrr:   NewOpc = Cse523::VMOVSDrr_REV;   break;
    //                                       case Cse523::VMOVSSrr:   NewOpc = Cse523::VMOVSSrr_REV;   break;
    //                                   }
    //                                   OutMI.setOpcode(NewOpc);
    //                               }
    //                               break;
    //                           }

    //                           // TAILJMPr64, CALL64r, CALL64pcrel32 - These instructions have register
    //                           // inputs modeled as normal uses instead of implicit uses.  As such, truncate
    //                           // off all but the first operand (the callee).  FIXME: Change isel.
    //    case Cse523::TAILJMPr64:
    //    case Cse523::CALL64r:
    //    case Cse523::CALL64pcrel32: {
    //                                    unsigned Opcode = OutMI.getOpcode();
    //                                    MCOperand Saved = OutMI.getOperand(0);
    //                                    OutMI = MCInst();
    //                                    OutMI.setOpcode(Opcode);
    //                                    OutMI.addOperand(Saved);
    //                                    break;
    //                                }

        case Cse523::EH_RETURN64: {
                                      OutMI = MCInst();
                                      OutMI.setOpcode(getRetOpcode(AsmPrinter.getSubtarget()));
                                      break;
                                  }

    //                              // TAILJMPd, TAILJMPd64 - Lower to the correct jump instructions.
    //    case Cse523::TAILJMPr:
    //    case Cse523::TAILJMPd:
    //    case Cse523::TAILJMPd64: {
    //                                 unsigned Opcode;
    //                                 switch (OutMI.getOpcode()) {
    //                                     default: llvm_unreachable("Invalid opcode");
    //                                     case Cse523::TAILJMPr: Opcode = Cse523::JMP32r; break;
    //                                     case Cse523::TAILJMPd:
    //                                     case Cse523::TAILJMPd64: Opcode = Cse523::JMP_1; break;
    //                                 }

    //                                 MCOperand Saved = OutMI.getOperand(0);
    //                                 OutMI = MCInst();
    //                                 OutMI.setOpcode(Opcode);
    //                                 OutMI.addOperand(Saved);
    //                                 break;
    //                             }

    //                             // These are pseudo-ops for OR to help with the OR->ADD transformation.  We do
    //                             // this with an ugly goto in case the resultant OR uses EAX and needs the
    //                             // short form.
    //    case Cse523::ADD16rr_DB:   OutMI.setOpcode(Cse523::OR16rr); goto ReSimplify;
    //    case Cse523::ADD32rr_DB:   OutMI.setOpcode(Cse523::OR32rr); goto ReSimplify;
    //    case Cse523::ADD64rr_DB:   OutMI.setOpcode(Cse523::OR64rr); goto ReSimplify;
    //    case Cse523::ADD16ri_DB:   OutMI.setOpcode(Cse523::OR16ri); goto ReSimplify;
    //    case Cse523::ADD32ri_DB:   OutMI.setOpcode(Cse523::OR32ri); goto ReSimplify;
    //    case Cse523::ADD64ri32_DB: OutMI.setOpcode(Cse523::OR64ri32); goto ReSimplify;
    //    case Cse523::ADD16ri8_DB:  OutMI.setOpcode(Cse523::OR16ri8); goto ReSimplify;
    //    case Cse523::ADD32ri8_DB:  OutMI.setOpcode(Cse523::OR32ri8); goto ReSimplify;
    //    case Cse523::ADD64ri8_DB:  OutMI.setOpcode(Cse523::OR64ri8); goto ReSimplify;

    //                               // The assembler backend wants to see branches in their small form and relax
    //                               // them to their large form.  The JIT can only handle the large form because
    //                               // it does not do relaxation.  For now, translate the large form to the
    //                               // small one here.
    //    case Cse523::JMP_4: OutMI.setOpcode(Cse523::JMP_1); break;
    //    case Cse523::JO_4:  OutMI.setOpcode(Cse523::JO_1); break;
    //    case Cse523::JNO_4: OutMI.setOpcode(Cse523::JNO_1); break;
    //    case Cse523::JB_4:  OutMI.setOpcode(Cse523::JB_1); break;
    //    case Cse523::JAE_4: OutMI.setOpcode(Cse523::JAE_1); break;
    //    case Cse523::JE_4:  OutMI.setOpcode(Cse523::JE_1); break;
    //    case Cse523::JNE_4: OutMI.setOpcode(Cse523::JNE_1); break;
    //    case Cse523::JBE_4: OutMI.setOpcode(Cse523::JBE_1); break;
    //    case Cse523::JA_4:  OutMI.setOpcode(Cse523::JA_1); break;
    //    case Cse523::JS_4:  OutMI.setOpcode(Cse523::JS_1); break;
    //    case Cse523::JNS_4: OutMI.setOpcode(Cse523::JNS_1); break;
    //    case Cse523::JP_4:  OutMI.setOpcode(Cse523::JP_1); break;
    //    case Cse523::JNP_4: OutMI.setOpcode(Cse523::JNP_1); break;
    //    case Cse523::JL_4:  OutMI.setOpcode(Cse523::JL_1); break;
    //    case Cse523::JGE_4: OutMI.setOpcode(Cse523::JGE_1); break;
    //    case Cse523::JLE_4: OutMI.setOpcode(Cse523::JLE_1); break;
    //    case Cse523::JG_4:  OutMI.setOpcode(Cse523::JG_1); break;

    //                        // Atomic load and store require a separate pseudo-inst because Acquire
    //                        // implies mayStore and Release implies mayLoad; fix these to regular MOV
    //                        // instructions here
    //    case Cse523::ACQUIRE_MOV8rm:  OutMI.setOpcode(Cse523::MOV8rm); goto ReSimplify;
    //    case Cse523::ACQUIRE_MOV16rm: OutMI.setOpcode(Cse523::MOV16rm); goto ReSimplify;
    //    case Cse523::ACQUIRE_MOV32rm: OutMI.setOpcode(Cse523::MOV32rm); goto ReSimplify;
    //    case Cse523::ACQUIRE_MOV64rm: OutMI.setOpcode(Cse523::MOV64rm); goto ReSimplify;
    //    case Cse523::RELEASE_MOV8mr:  OutMI.setOpcode(Cse523::MOV8mr); goto ReSimplify;
    //    case Cse523::RELEASE_MOV16mr: OutMI.setOpcode(Cse523::MOV16mr); goto ReSimplify;
    //    case Cse523::RELEASE_MOV32mr: OutMI.setOpcode(Cse523::MOV32mr); goto ReSimplify;
    //    case Cse523::RELEASE_MOV64mr: OutMI.setOpcode(Cse523::MOV64mr); goto ReSimplify;

    //                                  // We don't currently select the correct instruction form for instructions
    //                                  // which have a short %eax, etc. form. Handle this by custom lowering, for
    //                                  // now.
    //                                  //
    //                                  // Note, we are currently not handling the following instructions:
    //                                  // MOV64ao8, MOV64o8a
    //                                  // XCHG16ar, XCHG32ar, XCHG64ar
    //    case Cse523::MOV8mr_NOREX:
    //    case Cse523::MOV8mr:     SimplifyShortMoveForm(AsmPrinter, OutMI, Cse523::MOV8ao8); break;
    //    case Cse523::MOV8rm_NOREX:
    //    case Cse523::MOV8rm:     SimplifyShortMoveForm(AsmPrinter, OutMI, Cse523::MOV8o8a); break;
    //    case Cse523::MOV16mr:    SimplifyShortMoveForm(AsmPrinter, OutMI, Cse523::MOV16ao16); break;
    //    case Cse523::MOV16rm:    SimplifyShortMoveForm(AsmPrinter, OutMI, Cse523::MOV16o16a); break;
    //    case Cse523::MOV32mr:    SimplifyShortMoveForm(AsmPrinter, OutMI, Cse523::MOV32ao32); break;
    //    case Cse523::MOV32rm:    SimplifyShortMoveForm(AsmPrinter, OutMI, Cse523::MOV32o32a); break;

    //    case Cse523::ADC8ri:     SimplifyShortImmForm(OutMI, Cse523::ADC8i8);    break;
    //    case Cse523::ADC16ri:    SimplifyShortImmForm(OutMI, Cse523::ADC16i16);  break;
    //    case Cse523::ADC32ri:    SimplifyShortImmForm(OutMI, Cse523::ADC32i32);  break;
    //    case Cse523::ADC64ri32:  SimplifyShortImmForm(OutMI, Cse523::ADC64i32);  break;
    //    case Cse523::ADD8ri:     SimplifyShortImmForm(OutMI, Cse523::ADD8i8);    break;
    //    case Cse523::ADD16ri:    SimplifyShortImmForm(OutMI, Cse523::ADD16i16);  break;
    //    case Cse523::ADD32ri:    SimplifyShortImmForm(OutMI, Cse523::ADD32i32);  break;
    //    case Cse523::ADD64ri32:  SimplifyShortImmForm(OutMI, Cse523::ADD64i32);  break;
    //    case Cse523::AND8ri:     SimplifyShortImmForm(OutMI, Cse523::AND8i8);    break;
    //    case Cse523::AND16ri:    SimplifyShortImmForm(OutMI, Cse523::AND16i16);  break;
    //    case Cse523::AND32ri:    SimplifyShortImmForm(OutMI, Cse523::AND32i32);  break;
    //    case Cse523::AND64ri32:  SimplifyShortImmForm(OutMI, Cse523::AND64i32);  break;
    //    case Cse523::CMP8ri:     SimplifyShortImmForm(OutMI, Cse523::CMP8i8);    break;
    //    case Cse523::CMP16ri:    SimplifyShortImmForm(OutMI, Cse523::CMP16i16);  break;
    //    case Cse523::CMP32ri:    SimplifyShortImmForm(OutMI, Cse523::CMP32i32);  break;
    //    case Cse523::CMP64ri32:  SimplifyShortImmForm(OutMI, Cse523::CMP64i32);  break;
    //    case Cse523::OR8ri:      SimplifyShortImmForm(OutMI, Cse523::OR8i8);     break;
    //    case Cse523::OR16ri:     SimplifyShortImmForm(OutMI, Cse523::OR16i16);   break;
    //    case Cse523::OR32ri:     SimplifyShortImmForm(OutMI, Cse523::OR32i32);   break;
    //    case Cse523::OR64ri32:   SimplifyShortImmForm(OutMI, Cse523::OR64i32);   break;
    //    case Cse523::SBB8ri:     SimplifyShortImmForm(OutMI, Cse523::SBB8i8);    break;
    //    case Cse523::SBB16ri:    SimplifyShortImmForm(OutMI, Cse523::SBB16i16);  break;
    //    case Cse523::SBB32ri:    SimplifyShortImmForm(OutMI, Cse523::SBB32i32);  break;
    //    case Cse523::SBB64ri32:  SimplifyShortImmForm(OutMI, Cse523::SBB64i32);  break;
    //    case Cse523::SUB8ri:     SimplifyShortImmForm(OutMI, Cse523::SUB8i8);    break;
    //    case Cse523::SUB16ri:    SimplifyShortImmForm(OutMI, Cse523::SUB16i16);  break;
    //    case Cse523::SUB32ri:    SimplifyShortImmForm(OutMI, Cse523::SUB32i32);  break;
    //    case Cse523::SUB64ri32:  SimplifyShortImmForm(OutMI, Cse523::SUB64i32);  break;
    //    case Cse523::TEST8ri:    SimplifyShortImmForm(OutMI, Cse523::TEST8i8);   break;
    //    case Cse523::TEST16ri:   SimplifyShortImmForm(OutMI, Cse523::TEST16i16); break;
    //    case Cse523::TEST32ri:   SimplifyShortImmForm(OutMI, Cse523::TEST32i32); break;
    //    case Cse523::TEST64ri32: SimplifyShortImmForm(OutMI, Cse523::TEST64i32); break;
    //    case Cse523::XOR8ri:     SimplifyShortImmForm(OutMI, Cse523::XOR8i8);    break;
    //    case Cse523::XOR16ri:    SimplifyShortImmForm(OutMI, Cse523::XOR16i16);  break;
    //    case Cse523::XOR32ri:    SimplifyShortImmForm(OutMI, Cse523::XOR32i32);  break;
    //    case Cse523::XOR64ri32:  SimplifyShortImmForm(OutMI, Cse523::XOR64i32);  break;

    //                             // Try to shrink some forms of movsx.
    //    case Cse523::MOVSX16rr8:
    //    case Cse523::MOVSX32rr16:
    //    case Cse523::MOVSX64rr32:
    //                             SimplifyMOVSX(OutMI);
    //                             break;
    }
}

static void LowerTlsAddr(MCStreamer &OutStreamer,
        Cse523MCInstLower &MCInstLowering,
        const MachineInstr &MI,
        const MCSubtargetInfo& STI) {

    assert(0);
    //bool is64Bits = MI.getOpcode() == Cse523::TLS_addr64 ||
    //    MI.getOpcode() == Cse523::TLS_base_addr64;

    //bool needsPadding = MI.getOpcode() == Cse523::TLS_addr64;

    //MCContext &context = OutStreamer.getContext();

    //if (needsPadding)
    //    OutStreamer.EmitInstruction(MCInstBuilder(Cse523::DATA16_PREFIX), STI);

    //MCSymbolRefExpr::VariantKind SRVK;
    //switch (MI.getOpcode()) {
    //    case Cse523::TLS_addr32:
    //    case Cse523::TLS_addr64:
    //        SRVK = MCSymbolRefExpr::VK_TLSGD;
    //        break;
    //    case Cse523::TLS_base_addr32:
    //        SRVK = MCSymbolRefExpr::VK_TLSLDM;
    //        break;
    //    case Cse523::TLS_base_addr64:
    //        SRVK = MCSymbolRefExpr::VK_TLSLD;
    //        break;
    //    default:
    //        llvm_unreachable("unexpected opcode");
    //}

    //MCSymbol *sym = MCInstLowering.GetSymbolFromOperand(MI.getOperand(3));
    //const MCSymbolRefExpr *symRef = MCSymbolRefExpr::Create(sym, SRVK, context);

    //MCInst LEA;
    //if (is64Bits) {
    //    LEA.setOpcode(Cse523::LEA64r);
    //    LEA.addOperand(MCOperand::CreateReg(Cse523::RDI)); // dest
    //    LEA.addOperand(MCOperand::CreateReg(Cse523::RIP)); // base
    //    LEA.addOperand(MCOperand::CreateImm(1));        // scale
    //    LEA.addOperand(MCOperand::CreateReg(0));        // index
    //    LEA.addOperand(MCOperand::CreateExpr(symRef));  // disp
    //    LEA.addOperand(MCOperand::CreateReg(0));        // seg
    //} else if (SRVK == MCSymbolRefExpr::VK_TLSLDM) {
    //    LEA.setOpcode(Cse523::LEA32r);
    //    LEA.addOperand(MCOperand::CreateReg(Cse523::EAX)); // dest
    //    LEA.addOperand(MCOperand::CreateReg(Cse523::EBX)); // base
    //    LEA.addOperand(MCOperand::CreateImm(1));        // scale
    //    LEA.addOperand(MCOperand::CreateReg(0));        // index
    //    LEA.addOperand(MCOperand::CreateExpr(symRef));  // disp
    //    LEA.addOperand(MCOperand::CreateReg(0));        // seg
    //} else {
    //    LEA.setOpcode(Cse523::LEA32r);
    //    LEA.addOperand(MCOperand::CreateReg(Cse523::EAX)); // dest
    //    LEA.addOperand(MCOperand::CreateReg(0));        // base
    //    LEA.addOperand(MCOperand::CreateImm(1));        // scale
    //    LEA.addOperand(MCOperand::CreateReg(Cse523::EBX)); // index
    //    LEA.addOperand(MCOperand::CreateExpr(symRef));  // disp
    //    LEA.addOperand(MCOperand::CreateReg(0));        // seg
    //}
    //OutStreamer.EmitInstruction(LEA, STI);

    //if (needsPadding) {
    //    OutStreamer.EmitInstruction(MCInstBuilder(Cse523::DATA16_PREFIX), STI);
    //    OutStreamer.EmitInstruction(MCInstBuilder(Cse523::DATA16_PREFIX), STI);
    //    OutStreamer.EmitInstruction(MCInstBuilder(Cse523::REX64_PREFIX), STI);
    //}

    //StringRef name = is64Bits ? "__tls_get_addr" : "___tls_get_addr";
    //MCSymbol *tlsGetAddr = context.GetOrCreateSymbol(name);
    //const MCSymbolRefExpr *tlsRef =
    //    MCSymbolRefExpr::Create(tlsGetAddr,
    //            MCSymbolRefExpr::VK_PLT,
    //            context);

    //OutStreamer.EmitInstruction(MCInstBuilder(is64Bits ? Cse523::CALL64pcrel32
    //            : Cse523::CALLpcrel32)
    //        .addExpr(tlsRef), STI);
}

/// \brief Emit the optimal amount of multi-byte nops on Cse523.
static void EmitNops(MCStreamer &OS, unsigned NumBytes, bool Is64Bit, const MCSubtargetInfo &STI) {
    // This works only for 64bit. For 32bit we have to do additional checking if
    // the CPU supports multi-byte nops.
    assert(Is64Bit && "EmitNops only supports Cse523-64");
    assert(0);
    //while (NumBytes) {
    //    unsigned Opc, BaseReg, ScaleVal, IndexReg, Displacement, SegmentReg;
    //    Opc = IndexReg = Displacement = SegmentReg = 0;
    //    BaseReg = Cse523::RAX; ScaleVal = 1;
    //    switch (NumBytes) {
    //        case  0: llvm_unreachable("Zero nops?"); break;
    //        case  1: NumBytes -=  1; Opc = Cse523::NOOP; break;
    //        case  2: NumBytes -=  2; Opc = Cse523::XCHG16ar; break;
    //        case  3: NumBytes -=  3; Opc = Cse523::NOOPL; break;
    //        case  4: NumBytes -=  4; Opc = Cse523::NOOPL; Displacement = 8; break;
    //        case  5: NumBytes -=  5; Opc = Cse523::NOOPL; Displacement = 8;
    //                 IndexReg = Cse523::RAX; break;
    //        case  6: NumBytes -=  6; Opc = Cse523::NOOPW; Displacement = 8;
    //                 IndexReg = Cse523::RAX; break;
    //        case  7: NumBytes -=  7; Opc = Cse523::NOOPL; Displacement = 512; break;
    //        case  8: NumBytes -=  8; Opc = Cse523::NOOPL; Displacement = 512;
    //                 IndexReg = Cse523::RAX; break;
    //        case  9: NumBytes -=  9; Opc = Cse523::NOOPW; Displacement = 512;
    //                 IndexReg = Cse523::RAX; break;
    //        default: NumBytes -= 10; Opc = Cse523::NOOPW; Displacement = 512;
    //                 IndexReg = Cse523::RAX; SegmentReg = Cse523::CS; break;
    //    }

    //    unsigned NumPrefixes = std::min(NumBytes, 5U);
    //    NumBytes -= NumPrefixes;
    //    for (unsigned i = 0; i != NumPrefixes; ++i)
    //        OS.EmitBytes("\x66");

    //    switch (Opc) {
    //        default: llvm_unreachable("Unexpected opcode"); break;
    //        case Cse523::NOOP:
    //                 OS.EmitInstruction(MCInstBuilder(Opc), STI);
    //                 break;
    //        case Cse523::XCHG16ar:
    //                 OS.EmitInstruction(MCInstBuilder(Opc).addReg(Cse523::AX), STI);
    //                 break;
    //        case Cse523::NOOPL:
    //        case Cse523::NOOPW:
    //                 OS.EmitInstruction(MCInstBuilder(Opc).addReg(BaseReg).addImm(ScaleVal)
    //                         .addReg(IndexReg)
    //                         .addImm(Displacement)
    //                         .addReg(SegmentReg), STI);
    //                 break;
    //    }
    //} // while (NumBytes)
}

// Lower a stackmap of the form:
// <id>, <shadowBytes>, ...
static void LowerSTACKMAP(MCStreamer &OS, StackMaps &SM,
        const MachineInstr &MI, bool Is64Bit, const MCSubtargetInfo& STI) {
    unsigned NumBytes = MI.getOperand(1).getImm();
    SM.recordStackMap(MI);
    // Emit padding.
    // FIXME: These nops ensure that the stackmap's shadow is covered by
    // instructions from the same basic block, but the nops should not be
    // necessary if instructions from the same block follow the stackmap.
    EmitNops(OS, NumBytes, Is64Bit, STI);
}

// Lower a patchpoint of the form:
// [<def>], <id>, <numBytes>, <target>, <numArgs>, <cc>, ...
static void LowerPATCHPOINT(MCStreamer &OS, StackMaps &SM,
        const MachineInstr &MI, bool Is64Bit, const MCSubtargetInfo& STI) {
    assert(Is64Bit && "Patchpoint currently only supports Cse523-64");
    SM.recordPatchPoint(MI);

    PatchPointOpers opers(&MI);
    unsigned ScratchIdx = opers.getNextScratchIdx();
    unsigned EncodedBytes = 0;
    int64_t CallTarget = opers.getMetaOper(PatchPointOpers::TargetPos).getImm();
    if (CallTarget) {
        // Emit MOV to materialize the target address and the CALL to target.
        // This is encoded with 12-13 bytes, depending on which register is used.
        unsigned ScratchReg = MI.getOperand(ScratchIdx).getReg();
        if (Cse523II::isCse523ExtendedReg(ScratchReg))
            EncodedBytes = 13;
        else
            EncodedBytes = 12;
        OS.EmitInstruction(MCInstBuilder(Cse523::MOV64ri).addReg(ScratchReg)
                .addImm(CallTarget), STI);
        OS.EmitInstruction(MCInstBuilder(Cse523::CALL64r).addReg(ScratchReg), STI);
    }
    // Emit padding.
    unsigned NumBytes = opers.getMetaOper(PatchPointOpers::NBytesPos).getImm();
    assert(NumBytes >= EncodedBytes &&
            "Patchpoint can't request size less than the length of a call.");

    EmitNops(OS, NumBytes - EncodedBytes, Is64Bit, STI);
}

void Cse523AsmPrinter::EmitInstruction(const MachineInstr *MI) {
    Cse523MCInstLower MCInstLowering(*MF, *this);
    switch (MI->getOpcode()) {
        case TargetOpcode::DBG_VALUE:
            llvm_unreachable("Should be handled target independently");

            // Emit nothing here but a comment if we can.
    //    case Cse523::Int_MemBarrier:
    //        OutStreamer.emitRawComment("MEMBARRIER");
    //        return;


        case Cse523::EH_RETURN64: {
                                      // Lower these as normal, but add some comments.
                                      unsigned Reg = MI->getOperand(0).getReg();
                                      OutStreamer.AddComment(StringRef("eh_return, addr: %") +
                                              Cse523ATTInstPrinter::getRegisterName(Reg));
                                      break;
                                  }
    //    case Cse523::TAILJMPr:
    //    case Cse523::TAILJMPd:
    //    case Cse523::TAILJMPd64:
    //                              // Lower these as normal, but add some comments.
    //                              OutStreamer.AddComment("TAILCALL");
    //                              break;

    //    case Cse523::TLS_addr32:
        case Cse523::TLS_addr64:
    //    case Cse523::TLS_base_addr32:
        case Cse523::TLS_base_addr64:
                                  return LowerTlsAddr(OutStreamer, MCInstLowering, *MI, getSubtargetInfo());

    //    case Cse523::MOVPC32r: {
    //                               // This is a pseudo op for a two instruction sequence with a label, which
    //                               // looks like:
    //                               //     call "L1$pb"
    //                               // "L1$pb":
    //                               //     popl %esi

    //                               // Emit the call.
    //                               MCSymbol *PICBase = MF->getPICBaseSymbol();
    //                               // FIXME: We would like an efficient form for this, so we don't have to do a
    //                               // lot of extra uniquing.
    //                               EmitToStreamer(OutStreamer, MCInstBuilder(Cse523::CALLpcrel32)
    //                                       .addExpr(MCSymbolRefExpr::Create(PICBase, OutContext)));

    //                               // Emit the label.
    //                               OutStreamer.EmitLabel(PICBase);

    //                               // popl $reg
    //                               EmitToStreamer(OutStreamer, MCInstBuilder(Cse523::POP32r)
    //                                       .addReg(MI->getOperand(0).getReg()));
    //                               return;
    //                           }

    //    case Cse523::ADD32ri: {
    //                              // Lower the MO_GOT_ABSOLUTE_ADDRESS form of ADD32ri.
    //                              if (MI->getOperand(2).getTargetFlags() != Cse523II::MO_GOT_ABSOLUTE_ADDRESS)
    //                                  break;

    //                              // Okay, we have something like:
    //                              //  EAX = ADD32ri EAX, MO_GOT_ABSOLUTE_ADDRESS(@MYGLOBAL)

    //                              // For this, we want to print something like:
    //                              //   MYGLOBAL + (. - PICBASE)
    //                              // However, we can't generate a ".", so just emit a new label here and refer
    //                              // to it.
    //                              MCSymbol *DotSym = OutContext.CreateTempSymbol();
    //                              OutStreamer.EmitLabel(DotSym);

    //                              // Now that we have emitted the label, lower the complex operand expression.
    //                              MCSymbol *OpSym = MCInstLowering.GetSymbolFromOperand(MI->getOperand(2));

    //                              const MCExpr *DotExpr = MCSymbolRefExpr::Create(DotSym, OutContext);
    //                              const MCExpr *PICBase =
    //                                  MCSymbolRefExpr::Create(MF->getPICBaseSymbol(), OutContext);
    //                              DotExpr = MCBinaryExpr::CreateSub(DotExpr, PICBase, OutContext);

    //                              DotExpr = MCBinaryExpr::CreateAdd(MCSymbolRefExpr::Create(OpSym,OutContext),
    //                                      DotExpr, OutContext);

    //                              EmitToStreamer(OutStreamer, MCInstBuilder(Cse523::ADD32ri)
    //                                      .addReg(MI->getOperand(0).getReg())
    //                                      .addReg(MI->getOperand(1).getReg())
    //                                      .addExpr(DotExpr));
    //                              return;
    //                          }

        case TargetOpcode::STACKMAP:
                              return LowerSTACKMAP(OutStreamer, SM, *MI, Subtarget->is64Bit(), getSubtargetInfo());

        case TargetOpcode::PATCHPOINT:
                              return LowerPATCHPOINT(OutStreamer, SM, *MI, Subtarget->is64Bit(), getSubtargetInfo());

        case Cse523::MORESTACK_RET:
                              EmitToStreamer(OutStreamer, MCInstBuilder(getRetOpcode(*Subtarget)));
                              return;

        case Cse523::MORESTACK_RET_RESTORE_R10:
                              // Return, then restore R10.
                              EmitToStreamer(OutStreamer, MCInstBuilder(getRetOpcode(*Subtarget)));
                              EmitToStreamer(OutStreamer, MCInstBuilder(Cse523::MOV64rr)
                                      .addReg(Cse523::R10)
                                      .addReg(Cse523::RAX));
                              return;
    }

    MCInst TmpInst;
    MCInstLowering.Lower(MI, TmpInst);
    EmitToStreamer(OutStreamer, TmpInst);
}
