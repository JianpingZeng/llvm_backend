//===-- Cse523MCCodeEmitter.cpp - Convert Cse523 code to machine code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Cse523MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "mccodeemitter"
#include "MCTargetDesc/Cse523MCTargetDesc.h"
#include "MCTargetDesc/Cse523BaseInfo.h"
#include "MCTargetDesc/Cse523FixupKinds.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
class Cse523MCCodeEmitter : public MCCodeEmitter {
  Cse523MCCodeEmitter(const Cse523MCCodeEmitter &) LLVM_DELETED_FUNCTION;
  void operator=(const Cse523MCCodeEmitter &) LLVM_DELETED_FUNCTION;
  const MCInstrInfo &MCII;
  MCContext &Ctx;
public:
  Cse523MCCodeEmitter(const MCInstrInfo &mcii, MCContext &ctx)
    : MCII(mcii), Ctx(ctx) {
  }

  ~Cse523MCCodeEmitter() {}

  bool is64BitMode(const MCSubtargetInfo &STI) const {
      return true;
  }

  bool is32BitMode(const MCSubtargetInfo &STI) const {
      return false;
  }

  bool is16BitMode(const MCSubtargetInfo &STI) const {
      return false;
  }

  /// Is16BitMemOperand - Return true if the specified instruction has
  /// a 16-bit memory operand. Op specifies the operand # of the memoperand.
  bool Is16BitMemOperand(const MCInst &MI, unsigned Op,
                         const MCSubtargetInfo &STI) const {
  //  const MCOperand &BaseReg  = MI.getOperand(Op+Cse523::AddrBaseReg);
  //  const MCOperand &IndexReg = MI.getOperand(Op+Cse523::AddrIndexReg);
  //  const MCOperand &Disp     = MI.getOperand(Op+Cse523::AddrDisp);

  //  if (is16BitMode(STI) && BaseReg.getReg() == 0 &&
  //      Disp.isImm() && Disp.getImm() < 0x10000)
  //    return true;
  //  if ((BaseReg.getReg() != 0 &&
  //       Cse523MCRegisterClasses[Cse523::GR16RegClassID].contains(BaseReg.getReg())) ||
  //      (IndexReg.getReg() != 0 &&
  //       Cse523MCRegisterClasses[Cse523::GR16RegClassID].contains(IndexReg.getReg())))
  //    return true;
    return false;
  }

  unsigned GetCse523RegNum(const MCOperand &MO) const {
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg()) & 0x7;
  }

  // On regular Cse523, both XMM0-XMM7 and XMM8-XMM15 are encoded in the range
  // 0-7 and the difference between the 2 groups is given by the REX prefix.
  // In the VEX prefix, registers are seen sequencially from 0-15 and encoded
  // in 1's complement form, example:
  //
  //  ModRM field => XMM9 => 1
  //  VEX.VVVV    => XMM9 => ~9
  //
  // See table 4-35 of Intel AVX Programming Reference for details.
  unsigned char getVEXRegisterEncoding(const MCInst &MI,
                                       unsigned OpNum) const {
    unsigned SrcReg = MI.getOperand(OpNum).getReg();
    unsigned SrcRegNum = GetCse523RegNum(MI.getOperand(OpNum));
    if (Cse523II::isCse523ExtendedReg(SrcReg))
      SrcRegNum |= 8;

    // The registers represented through VEX_VVVV should
    // be encoded in 1's complement form.
    return (~SrcRegNum) & 0xf;
  }

  unsigned char getWriteMaskRegisterEncoding(const MCInst &MI,
                                             unsigned OpNum) const {
    //assert(Cse523::K0 != MI.getOperand(OpNum).getReg() &&
    //       "Invalid mask register as write-mask!");
    unsigned MaskRegNum = GetCse523RegNum(MI.getOperand(OpNum));
    return MaskRegNum;
  }

  void EmitByte(unsigned char C, unsigned &CurByte, raw_ostream &OS) const {
    OS << (char)C;
    ++CurByte;
  }

  void EmitConstant(uint64_t Val, unsigned Size, unsigned &CurByte,
                    raw_ostream &OS) const {
    // Output the constant in little endian byte order.
    for (unsigned i = 0; i != Size; ++i) {
      EmitByte(Val & 255, CurByte, OS);
      Val >>= 8;
    }
  }

  void EmitImmediate(const MCOperand &Disp, SMLoc Loc,
                     unsigned ImmSize, MCFixupKind FixupKind,
                     unsigned &CurByte, raw_ostream &OS,
                     SmallVectorImpl<MCFixup> &Fixups,
                     int ImmOffset = 0) const;

  inline static unsigned char ModRMByte(unsigned Mod, unsigned RegOpcode,
                                        unsigned RM) {
    assert(Mod < 4 && RegOpcode < 8 && RM < 8 && "ModRM Fields out of range!");
    return RM | (RegOpcode << 3) | (Mod << 6);
  }

  void EmitRegModRMByte(const MCOperand &ModRMReg, unsigned RegOpcodeFld,
                        unsigned &CurByte, raw_ostream &OS) const {
    EmitByte(ModRMByte(3, RegOpcodeFld, GetCse523RegNum(ModRMReg)), CurByte, OS);
  }

  void EmitSIBByte(unsigned SS, unsigned Index, unsigned Base,
                   unsigned &CurByte, raw_ostream &OS) const {
    // SIB byte is in the same format as the ModRMByte.
    EmitByte(ModRMByte(SS, Index, Base), CurByte, OS);
  }


  void EmitMemModRMByte(const MCInst &MI, unsigned Op,
                        unsigned RegOpcodeField,
                        uint64_t TSFlags, unsigned &CurByte, raw_ostream &OS,
                        SmallVectorImpl<MCFixup> &Fixups,
                        const MCSubtargetInfo &STI) const;

  void EncodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const;

  void EmitVEXOpcodePrefix(uint64_t TSFlags, unsigned &CurByte, int MemOperand,
                           const MCInst &MI, const MCInstrDesc &Desc,
                           raw_ostream &OS) const;

  void EmitSegmentOverridePrefix(unsigned &CurByte, unsigned SegOperand,
                                 const MCInst &MI, raw_ostream &OS) const;

  void EmitOpcodePrefix(uint64_t TSFlags, unsigned &CurByte, int MemOperand,
                        const MCInst &MI, const MCInstrDesc &Desc,
                        const MCSubtargetInfo &STI,
                        raw_ostream &OS) const;
};

} // end anonymous namespace


MCCodeEmitter *llvm::createCse523MCCodeEmitter(const MCInstrInfo &MCII,
                                            const MCRegisterInfo &MRI,
                                            const MCSubtargetInfo &STI,
                                            MCContext &Ctx) {
  return new Cse523MCCodeEmitter(MCII, Ctx);
}

/// isDisp8 - Return true if this signed displacement fits in a 8-bit
/// sign-extended field.
static bool isDisp8(int Value) {
  return Value == (signed char)Value;
}

/// isCDisp8 - Return true if this signed displacement fits in a 8-bit
/// compressed dispacement field.
static bool isCDisp8(uint64_t TSFlags, int Value, int& CValue) {
    return false;
//  assert((TSFlags & Cse523II::EncodingMask) >> Cse523II::EncodingShift == Cse523II::EVEX &&
//         "Compressed 8-bit displacement is only valid for EVEX inst.");
//
//  unsigned CD8E = (TSFlags >> Cse523II::EVEX_CD8EShift) & Cse523II::EVEX_CD8EMask;
//  unsigned CD8V = (TSFlags >> Cse523II::EVEX_CD8VShift) & Cse523II::EVEX_CD8VMask;
//
//  if (CD8V == 0 && CD8E == 0) {
//    CValue = Value;
//    return isDisp8(Value);
//  }
//  
//  unsigned MemObjSize = 1U << CD8E;
//  if (CD8V & 4) {
//    // Fixed vector length
//    MemObjSize *= 1U << (CD8V & 0x3);
//  } else {
//    // Modified vector length
//    bool EVEX_b = (TSFlags >> Cse523II::VEXShift) & Cse523II::EVEX_B;
//    if (!EVEX_b) {
//      unsigned EVEX_LL = ((TSFlags >> Cse523II::VEXShift) & Cse523II::VEX_L) ? 1 : 0;
//      EVEX_LL += ((TSFlags >> Cse523II::VEXShift) & Cse523II::EVEX_L2) ? 2 : 0;
//      assert(EVEX_LL < 3 && "");
//
//      unsigned NumElems = (1U << (EVEX_LL + 4)) / MemObjSize;
//      NumElems /= 1U << (CD8V & 0x3);
//
//      MemObjSize *= NumElems;
//    }
//  }
//
//  unsigned MemObjMask = MemObjSize - 1;
//  assert((MemObjSize & MemObjMask) == 0 && "Invalid memory object size.");
//
//  if (Value & MemObjMask) // Unaligned offset
//    return false;
//  Value /= MemObjSize;
//  bool Ret = (Value == (signed char)Value);
//
//  if (Ret)
//    CValue = Value;
//  return Ret;
}

/// getImmFixupKind - Return the appropriate fixup kind to use for an immediate
/// in an instruction with the specified TSFlags.
static MCFixupKind getImmFixupKind(uint64_t TSFlags) {
  unsigned Size = Cse523II::getSizeOfImm(TSFlags);
  bool isPCRel = Cse523II::isImmPCRel(TSFlags);

  if (Cse523II::isImmSigned(TSFlags)) {
    switch (Size) {
    default: llvm_unreachable("Unsupported signed fixup size!");
    case 4: return MCFixupKind(Cse523::reloc_signed_4byte);
    }
  }
  return MCFixup::getKindForSize(Size, isPCRel);
}

/// Is32BitMemOperand - Return true if the specified instruction has
/// a 32-bit memory operand. Op specifies the operand # of the memoperand.
static bool Is32BitMemOperand(const MCInst &MI, unsigned Op) {
//  const MCOperand &BaseReg  = MI.getOperand(Op+Cse523::AddrBaseReg);
//  const MCOperand &IndexReg = MI.getOperand(Op+Cse523::AddrIndexReg);
//
//  if ((BaseReg.getReg() != 0 &&
//       Cse523MCRegisterClasses[Cse523::GR32RegClassID].contains(BaseReg.getReg())) ||
//      (IndexReg.getReg() != 0 &&
//       Cse523MCRegisterClasses[Cse523::GR32RegClassID].contains(IndexReg.getReg())))
//    return true;
  return false;
}

/// Is64BitMemOperand - Return true if the specified instruction has
/// a 64-bit memory operand. Op specifies the operand # of the memoperand.
#ifndef NDEBUG
static bool Is64BitMemOperand(const MCInst &MI, unsigned Op) {
  const MCOperand &BaseReg  = MI.getOperand(Op+Cse523::AddrBaseReg);
  const MCOperand &IndexReg = MI.getOperand(Op+Cse523::AddrIndexReg);

  if ((BaseReg.getReg() != 0 &&
       Cse523MCRegisterClasses[Cse523::GR64RegClassID].contains(BaseReg.getReg())) ||
      (IndexReg.getReg() != 0 &&
       Cse523MCRegisterClasses[Cse523::GR64RegClassID].contains(IndexReg.getReg())))
    return true;
  return false;
}
#endif

/// StartsWithGlobalOffsetTable - Check if this expression starts with
///  _GLOBAL_OFFSET_TABLE_ and if it is of the form
///  _GLOBAL_OFFSET_TABLE_-symbol. This is needed to support PIC on ELF
/// i386 as _GLOBAL_OFFSET_TABLE_ is magical. We check only simple case that
/// are know to be used: _GLOBAL_OFFSET_TABLE_ by itself or at the start
/// of a binary expression.
enum GlobalOffsetTableExprKind {
  GOT_None,
  GOT_Normal,
  GOT_SymDiff
};
static GlobalOffsetTableExprKind
StartsWithGlobalOffsetTable(const MCExpr *Expr) {
  const MCExpr *RHS = 0;
  if (Expr->getKind() == MCExpr::Binary) {
    const MCBinaryExpr *BE = static_cast<const MCBinaryExpr *>(Expr);
    Expr = BE->getLHS();
    RHS = BE->getRHS();
  }

  if (Expr->getKind() != MCExpr::SymbolRef)
    return GOT_None;

  const MCSymbolRefExpr *Ref = static_cast<const MCSymbolRefExpr*>(Expr);
  const MCSymbol &S = Ref->getSymbol();
  if (S.getName() != "_GLOBAL_OFFSET_TABLE_")
    return GOT_None;
  if (RHS && RHS->getKind() == MCExpr::SymbolRef)
    return GOT_SymDiff;
  return GOT_Normal;
}

static bool HasSecRelSymbolRef(const MCExpr *Expr) {
  if (Expr->getKind() == MCExpr::SymbolRef) {
    const MCSymbolRefExpr *Ref = static_cast<const MCSymbolRefExpr*>(Expr);
    return Ref->getKind() == MCSymbolRefExpr::VK_SECREL;
  }
  return false;
}

void Cse523MCCodeEmitter::
EmitImmediate(const MCOperand &DispOp, SMLoc Loc, unsigned Size,
              MCFixupKind FixupKind, unsigned &CurByte, raw_ostream &OS,
              SmallVectorImpl<MCFixup> &Fixups, int ImmOffset) const {
  const MCExpr *Expr = NULL;
  if (DispOp.isImm()) {
    // If this is a simple integer displacement that doesn't require a
    // relocation, emit it now.
    if (FixupKind != FK_PCRel_1 &&
        FixupKind != FK_PCRel_2 &&
        FixupKind != FK_PCRel_4) {
      EmitConstant(DispOp.getImm()+ImmOffset, Size, CurByte, OS);
      return;
    }
    Expr = MCConstantExpr::Create(DispOp.getImm(), Ctx);
  } else {
    Expr = DispOp.getExpr();
  }

  // If we have an immoffset, add it to the expression.
  if ((FixupKind == FK_Data_4 ||
       FixupKind == FK_Data_8 ||
       FixupKind == MCFixupKind(Cse523::reloc_signed_4byte))) {
    GlobalOffsetTableExprKind Kind = StartsWithGlobalOffsetTable(Expr);
    if (Kind != GOT_None) {
      assert(ImmOffset == 0);

      FixupKind = MCFixupKind(Cse523::reloc_global_offset_table);
      if (Kind == GOT_Normal)
        ImmOffset = CurByte;
    } else if (Expr->getKind() == MCExpr::SymbolRef) {
      if (HasSecRelSymbolRef(Expr)) {
        FixupKind = MCFixupKind(FK_SecRel_4);
      }
    } else if (Expr->getKind() == MCExpr::Binary) {
      const MCBinaryExpr *Bin = static_cast<const MCBinaryExpr*>(Expr);
      if (HasSecRelSymbolRef(Bin->getLHS())
          || HasSecRelSymbolRef(Bin->getRHS())) {
        FixupKind = MCFixupKind(FK_SecRel_4);
      }
    }
  }

  // If the fixup is pc-relative, we need to bias the value to be relative to
  // the start of the field, not the end of the field.
  if (FixupKind == FK_PCRel_4 ||
      FixupKind == MCFixupKind(Cse523::reloc_riprel_4byte) ||
      FixupKind == MCFixupKind(Cse523::reloc_riprel_4byte_movq_load))
    ImmOffset -= 4;
  if (FixupKind == FK_PCRel_2)
    ImmOffset -= 2;
  if (FixupKind == FK_PCRel_1)
    ImmOffset -= 1;

  if (ImmOffset)
    Expr = MCBinaryExpr::CreateAdd(Expr, MCConstantExpr::Create(ImmOffset, Ctx),
                                   Ctx);

  // Emit a symbolic constant as a fixup and 4 zeros.
  Fixups.push_back(MCFixup::Create(CurByte, Expr, FixupKind, Loc));
  EmitConstant(0, Size, CurByte, OS);
}

void Cse523MCCodeEmitter::EmitMemModRMByte(const MCInst &MI, unsigned Op,
                                        unsigned RegOpcodeField,
                                        uint64_t TSFlags, unsigned &CurByte,
                                        raw_ostream &OS,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const{
  const MCOperand &Disp     = MI.getOperand(Op+Cse523::AddrDisp);
  const MCOperand &Base     = MI.getOperand(Op+Cse523::AddrBaseReg);
  const MCOperand &Scale    = MI.getOperand(Op+Cse523::AddrScaleAmt);
  const MCOperand &IndexReg = MI.getOperand(Op+Cse523::AddrIndexReg);
  unsigned BaseReg = Base.getReg();
  unsigned char Encoding = (TSFlags & Cse523II::EncodingMask) >>
                           Cse523II::EncodingShift;
  bool HasEVEX = (Encoding == Cse523II::EVEX);

  // Handle %rip relative addressing.
  if (BaseReg == Cse523::RIP) {    // [disp32+RIP] in Cse523-64 mode
    assert(is64BitMode(STI) && "Rip-relative addressing requires 64-bit mode");
    assert(IndexReg.getReg() == 0 && "Invalid rip-relative address");
    EmitByte(ModRMByte(0, RegOpcodeField, 5), CurByte, OS);

    unsigned FixupKind = Cse523::reloc_riprel_4byte;

    // movq loads are handled with a special relocation form which allows the
    // linker to eliminate some loads for GOT references which end up in the
    // same linkage unit.

    //if (MI.getOpcode() == Cse523::MOV64rm)
    //  FixupKind = Cse523::reloc_riprel_4byte_movq_load;

    // rip-relative addressing is actually relative to the *next* instruction.
    // Since an immediate can follow the mod/rm byte for an instruction, this
    // means that we need to bias the immediate field of the instruction with
    // the size of the immediate field.  If we have this case, add it into the
    // expression to emit.
    int ImmSize = Cse523II::hasImm(TSFlags) ? Cse523II::getSizeOfImm(TSFlags) : 0;

    EmitImmediate(Disp, MI.getLoc(), 4, MCFixupKind(FixupKind),
                  CurByte, OS, Fixups, -ImmSize);
    return;
  }

  unsigned BaseRegNo = BaseReg ? GetCse523RegNum(Base) : -1U;

  // Determine whether a SIB byte is needed.
  // If no BaseReg, issue a RIP relative instruction only if the MCE can
  // resolve addresses on-the-fly, otherwise use SIB (Intel Manual 2A, table
  // 2-7) and absolute references.

  if (// The SIB byte must be used if there is an index register.
      IndexReg.getReg() == 0 &&
      // The SIB byte must be used if the base is ESP/RSP/R12, all of which
      // encode to an R/M value of 4, which indicates that a SIB byte is
      // present.
      BaseRegNo != N86::ESP &&
      // If there is no base register and we're in 64-bit mode, we need a SIB
      // byte to emit an addr that is just 'disp32' (the non-RIP relative form).
      (!is64BitMode(STI) || BaseReg != 0)) {

    if (BaseReg == 0) {          // [disp32]     in Cse523-32 mode
      EmitByte(ModRMByte(0, RegOpcodeField, 5), CurByte, OS);
      EmitImmediate(Disp, MI.getLoc(), 4, FK_Data_4, CurByte, OS, Fixups);
      return;
    }

    // If the base is not EBP/ESP and there is no displacement, use simple
    // indirect register encoding, this handles addresses like [EAX].  The
    // encoding for [EBP] with no displacement means [disp32] so we handle it
    // by emitting a displacement of 0 below.
    if (Disp.isImm() && Disp.getImm() == 0 && BaseRegNo != N86::EBP) {
      EmitByte(ModRMByte(0, RegOpcodeField, BaseRegNo), CurByte, OS);
      return;
    }

    // Otherwise, if the displacement fits in a byte, encode as [REG+disp8].
    if (Disp.isImm()) {
      if (!HasEVEX && isDisp8(Disp.getImm())) {
        EmitByte(ModRMByte(1, RegOpcodeField, BaseRegNo), CurByte, OS);
        EmitImmediate(Disp, MI.getLoc(), 1, FK_Data_1, CurByte, OS, Fixups);
        return;
      }
      // Try EVEX compressed 8-bit displacement first; if failed, fall back to
      // 32-bit displacement.
      int CDisp8 = 0;
      if (HasEVEX && isCDisp8(TSFlags, Disp.getImm(), CDisp8)) {
        EmitByte(ModRMByte(1, RegOpcodeField, BaseRegNo), CurByte, OS);
        EmitImmediate(Disp, MI.getLoc(), 1, FK_Data_1, CurByte, OS, Fixups,
                      CDisp8 - Disp.getImm());
        return;
      }
    }

    // Otherwise, emit the most general non-SIB encoding: [REG+disp32]
    EmitByte(ModRMByte(2, RegOpcodeField, BaseRegNo), CurByte, OS);
    EmitImmediate(Disp, MI.getLoc(), 4, MCFixupKind(Cse523::reloc_signed_4byte), CurByte, OS,
                  Fixups);
    return;
  }

  // We need a SIB byte, so start by outputting the ModR/M byte first
  assert(IndexReg.getReg() != Cse523::RSP && "Cannot use RSP as index reg!");

  bool ForceDisp32 = false;
  bool ForceDisp8  = false;
  int CDisp8 = 0;
  int ImmOffset = 0;
  if (BaseReg == 0) {
    // If there is no base register, we emit the special case SIB byte with
    // MOD=0, BASE=5, to JUST get the index, scale, and displacement.
    EmitByte(ModRMByte(0, RegOpcodeField, 4), CurByte, OS);
    ForceDisp32 = true;
  } else if (!Disp.isImm()) {
    // Emit the normal disp32 encoding.
    EmitByte(ModRMByte(2, RegOpcodeField, 4), CurByte, OS);
    ForceDisp32 = true;
  } else if (Disp.getImm() == 0 &&
             // Base reg can't be anything that ends up with '5' as the base
             // reg, it is the magic [*] nomenclature that indicates no base.
             BaseRegNo != N86::EBP) {
    // Emit no displacement ModR/M byte
    EmitByte(ModRMByte(0, RegOpcodeField, 4), CurByte, OS);
  } else if (!HasEVEX && isDisp8(Disp.getImm())) {
    // Emit the disp8 encoding.
    EmitByte(ModRMByte(1, RegOpcodeField, 4), CurByte, OS);
    ForceDisp8 = true;           // Make sure to force 8 bit disp if Base=EBP
  } else if (HasEVEX && isCDisp8(TSFlags, Disp.getImm(), CDisp8)) {
    // Emit the disp8 encoding.
    EmitByte(ModRMByte(1, RegOpcodeField, 4), CurByte, OS);
    ForceDisp8 = true;           // Make sure to force 8 bit disp if Base=EBP
    ImmOffset = CDisp8 - Disp.getImm();
  } else {
    // Emit the normal disp32 encoding.
    EmitByte(ModRMByte(2, RegOpcodeField, 4), CurByte, OS);
  }

  // Calculate what the SS field value should be...
  static const unsigned SSTable[] = { ~0U, 0, 1, ~0U, 2, ~0U, ~0U, ~0U, 3 };
  unsigned SS = SSTable[Scale.getImm()];

  if (BaseReg == 0) {
    // Handle the SIB byte for the case where there is no base, see Intel
    // Manual 2A, table 2-7. The displacement has already been output.
    unsigned IndexRegNo;
    if (IndexReg.getReg())
      IndexRegNo = GetCse523RegNum(IndexReg);
    else // Examples: [ESP+1*<noreg>+4] or [scaled idx]+disp32 (MOD=0,BASE=5)
      IndexRegNo = 4;
    EmitSIBByte(SS, IndexRegNo, 5, CurByte, OS);
  } else {
    unsigned IndexRegNo;
    if (IndexReg.getReg())
      IndexRegNo = GetCse523RegNum(IndexReg);
    else
      IndexRegNo = 4;   // For example [ESP+1*<noreg>+4]
    EmitSIBByte(SS, IndexRegNo, GetCse523RegNum(Base), CurByte, OS);
  }

  // Do we need to output a displacement?
  if (ForceDisp8)
    EmitImmediate(Disp, MI.getLoc(), 1, FK_Data_1, CurByte, OS, Fixups, ImmOffset);
  else if (ForceDisp32 || Disp.getImm() != 0)
    EmitImmediate(Disp, MI.getLoc(), 4, MCFixupKind(Cse523::reloc_signed_4byte),
                  CurByte, OS, Fixups);
}

/// EmitVEXOpcodePrefix - AVX instructions are encoded using a opcode prefix
/// called VEX.
void Cse523MCCodeEmitter::EmitVEXOpcodePrefix(uint64_t TSFlags, unsigned &CurByte,
                                           int MemOperand, const MCInst &MI,
                                           const MCInstrDesc &Desc,
                                           raw_ostream &OS) const {
    assert(0);
}

/// DetermineREXPrefix - Determine if the MCInst has to be encoded with a Cse523-64
/// REX prefix which specifies 1) 64-bit instructions, 2) non-default operand
/// size, and 3) use of Cse523-64 extended registers.
static unsigned DetermineREXPrefix(const MCInst &MI, uint64_t TSFlags,
                                   const MCInstrDesc &Desc) {
  unsigned REX = 0;
  if (TSFlags & Cse523II::REX_W)
    REX |= 1 << 3; // set REX.W

  if (MI.getNumOperands() == 0) return REX;

  unsigned NumOps = MI.getNumOperands();
  // FIXME: MCInst should explicitize the two-addrness.
  bool isTwoAddr = NumOps > 1 &&
                      Desc.getOperandConstraint(1, MCOI::TIED_TO) != -1;

  // If it accesses SPL, BPL, SIL, or DIL, then it requires a 0x40 REX prefix.
  unsigned i = isTwoAddr ? 1 : 0;
  for (; i != NumOps; ++i) {
    const MCOperand &MO = MI.getOperand(i);
    if (!MO.isReg()) continue;
    unsigned Reg = MO.getReg();
    if (!Cse523II::isCse523NonExtLowByteReg(Reg)) continue;
    // FIXME: The caller of DetermineREXPrefix slaps this prefix onto anything
    // that returns non-zero.
    REX |= 0x40; // REX fixed encoding prefix
    break;
  }

  switch (TSFlags & Cse523II::FormMask) {
  case Cse523II::MRMSrcReg:
    if (MI.getOperand(0).isReg() &&
        Cse523II::isCse523ExtendedReg(MI.getOperand(0).getReg()))
      REX |= 1 << 2; // set REX.R
    i = isTwoAddr ? 2 : 1;
    for (; i != NumOps; ++i) {
      const MCOperand &MO = MI.getOperand(i);
      if (MO.isReg() && Cse523II::isCse523ExtendedReg(MO.getReg()))
        REX |= 1 << 0; // set REX.B
    }
    break;
  case Cse523II::MRMSrcMem: {
    if (MI.getOperand(0).isReg() &&
        Cse523II::isCse523ExtendedReg(MI.getOperand(0).getReg()))
      REX |= 1 << 2; // set REX.R
    unsigned Bit = 0;
    i = isTwoAddr ? 2 : 1;
    for (; i != NumOps; ++i) {
      const MCOperand &MO = MI.getOperand(i);
      if (MO.isReg()) {
        if (Cse523II::isCse523ExtendedReg(MO.getReg()))
          REX |= 1 << Bit; // set REX.B (Bit=0) and REX.X (Bit=1)
        Bit++;
      }
    }
    break;
  }
  case Cse523II::MRMXm:
  case Cse523II::MRM0m: case Cse523II::MRM1m:
  case Cse523II::MRM2m: case Cse523II::MRM3m:
  case Cse523II::MRM4m: case Cse523II::MRM5m:
  case Cse523II::MRM6m: case Cse523II::MRM7m:
  case Cse523II::MRMDestMem: {
    unsigned e = (isTwoAddr ? Cse523::AddrNumOperands+1 : Cse523::AddrNumOperands);
    i = isTwoAddr ? 1 : 0;
    if (NumOps > e && MI.getOperand(e).isReg() &&
        Cse523II::isCse523ExtendedReg(MI.getOperand(e).getReg()))
      REX |= 1 << 2; // set REX.R
    unsigned Bit = 0;
    for (; i != e; ++i) {
      const MCOperand &MO = MI.getOperand(i);
      if (MO.isReg()) {
        if (Cse523II::isCse523ExtendedReg(MO.getReg()))
          REX |= 1 << Bit; // REX.B (Bit=0) and REX.X (Bit=1)
        Bit++;
      }
    }
    break;
  }
  default:
    if (MI.getOperand(0).isReg() &&
        Cse523II::isCse523ExtendedReg(MI.getOperand(0).getReg()))
      REX |= 1 << 0; // set REX.B
    i = isTwoAddr ? 2 : 1;
    for (unsigned e = NumOps; i != e; ++i) {
      const MCOperand &MO = MI.getOperand(i);
      if (MO.isReg() && Cse523II::isCse523ExtendedReg(MO.getReg()))
        REX |= 1 << 2; // set REX.R
    }
    break;
  }
  return REX;
}

/// EmitSegmentOverridePrefix - Emit segment override opcode prefix as needed
void Cse523MCCodeEmitter::EmitSegmentOverridePrefix(unsigned &CurByte,
                                                 unsigned SegOperand,
                                                 const MCInst &MI,
                                                 raw_ostream &OS) const {
  // Check for explicit segment override on memory operand.
  switch (MI.getOperand(SegOperand).getReg()) {
  default: llvm_unreachable("Unknown segment register!");
  case 0: break;
  case Cse523::CS: EmitByte(0x2E, CurByte, OS); break;
  case Cse523::SS: EmitByte(0x36, CurByte, OS); break;
  case Cse523::DS: EmitByte(0x3E, CurByte, OS); break;
  case Cse523::ES: EmitByte(0x26, CurByte, OS); break;
  case Cse523::FS: EmitByte(0x64, CurByte, OS); break;
  case Cse523::GS: EmitByte(0x65, CurByte, OS); break;
  }
}

/// EmitOpcodePrefix - Emit all instruction prefixes prior to the opcode.
///
/// MemOperand is the operand # of the start of a memory operand if present.  If
/// Not present, it is -1.
void Cse523MCCodeEmitter::EmitOpcodePrefix(uint64_t TSFlags, unsigned &CurByte,
                                        int MemOperand, const MCInst &MI,
                                        const MCInstrDesc &Desc,
                                        const MCSubtargetInfo &STI,
                                        raw_ostream &OS) const {

  // Emit the operand size opcode prefix as needed.
  unsigned char OpSize = (TSFlags & Cse523II::OpSizeMask) >> Cse523II::OpSizeShift;
  if (OpSize == (is16BitMode(STI) ? Cse523II::OpSize32 : Cse523II::OpSize16))
    EmitByte(0x66, CurByte, OS);

  switch (TSFlags & Cse523II::OpPrefixMask) {
  case Cse523II::PD:   // 66
    EmitByte(0x66, CurByte, OS);
    break;
  case Cse523II::XS:   // F3
    EmitByte(0xF3, CurByte, OS);
    break;
  case Cse523II::XD:   // F2
    EmitByte(0xF2, CurByte, OS);
    break;
  }

  // Handle REX prefix.
  // FIXME: Can this come before F2 etc to simplify emission?
  if (is64BitMode(STI)) {
    if (unsigned REX = DetermineREXPrefix(MI, TSFlags, Desc))
      EmitByte(0x40 | REX, CurByte, OS);
  }

  // 0x0F escape code must be emitted just before the opcode.
  switch (TSFlags & Cse523II::OpMapMask) {
  case Cse523II::TB:  // Two-byte opcode map
  case Cse523II::T8:  // 0F 38
  case Cse523II::TA:  // 0F 3A
    EmitByte(0x0F, CurByte, OS);
    break;
  }

  switch (TSFlags & Cse523II::OpMapMask) {
  case Cse523II::T8:    // 0F 38
    EmitByte(0x38, CurByte, OS);
    break;
  case Cse523II::TA:    // 0F 3A
    EmitByte(0x3A, CurByte, OS);
    break;
  }
}

void Cse523MCCodeEmitter::
EncodeInstruction(const MCInst &MI, raw_ostream &OS,
                  SmallVectorImpl<MCFixup> &Fixups,
                  const MCSubtargetInfo &STI) const {
  unsigned Opcode = MI.getOpcode();
  const MCInstrDesc &Desc = MCII.get(Opcode);
  uint64_t TSFlags = Desc.TSFlags;

  // Pseudo instructions don't get encoded.
  if ((TSFlags & Cse523II::FormMask) == Cse523II::Pseudo)
    return;

  unsigned NumOps = Desc.getNumOperands();
  unsigned CurOp = Cse523II::getOperandBias(Desc);

  // Keep track of the current byte being emitted.
  unsigned CurByte = 0;

  // Encoding type for this instruction.
  unsigned char Encoding = (TSFlags & Cse523II::EncodingMask) >>
                           Cse523II::EncodingShift;

  // Determine where the memory operand starts, if present.
  int MemoryOperand = Cse523II::getMemoryOperandNo(TSFlags, Opcode);
  if (MemoryOperand != -1) MemoryOperand += CurOp;

  // Emit the lock opcode prefix as needed.
  if (TSFlags & Cse523II::LOCK)
    EmitByte(0xF0, CurByte, OS);

  // Emit segment override opcode prefix as needed.
  if (MemoryOperand >= 0)
    EmitSegmentOverridePrefix(CurByte, MemoryOperand+Cse523::AddrSegmentReg,
                              MI, OS);

  // Emit the repeat opcode prefix as needed.
  if (TSFlags & Cse523II::REP)
    EmitByte(0xF3, CurByte, OS);

  // Emit the address size opcode prefix as needed.
  bool need_address_override;
  // The AdSize prefix is only for 32-bit and 64-bit modes. Hm, perhaps we
  // should introduce an AdSize16 bit instead of having seven special cases?
  if ((!is16BitMode(STI) && TSFlags & Cse523II::AdSize)
 //    || (is16BitMode(STI) && (MI.getOpcode() == Cse523::JECXZ_32 ||
 //                        MI.getOpcode() == Cse523::MOV8o8a ||
 //                        MI.getOpcode() == Cse523::MOV16o16a ||
 //                        MI.getOpcode() == Cse523::MOV32o32a ||
 //                        MI.getOpcode() == Cse523::MOV8ao8 ||
 //                        MI.getOpcode() == Cse523::MOV16ao16 ||
 //                        MI.getOpcode() == Cse523::MOV32ao32))
  ) {
    need_address_override = true;
  } else if (MemoryOperand < 0) {
    need_address_override = false;
  } else if (is64BitMode(STI)) {
    assert(!Is16BitMemOperand(MI, MemoryOperand, STI));
    need_address_override = Is32BitMemOperand(MI, MemoryOperand);
  } else if (is32BitMode(STI)) {
    assert(!Is64BitMemOperand(MI, MemoryOperand));
    need_address_override = Is16BitMemOperand(MI, MemoryOperand, STI);
  } else {
    assert(is16BitMode(STI));
    assert(!Is64BitMemOperand(MI, MemoryOperand));
    need_address_override = !Is16BitMemOperand(MI, MemoryOperand, STI);
  }

  if (need_address_override)
    EmitByte(0x67, CurByte, OS);

  if (Encoding == 0)
    EmitOpcodePrefix(TSFlags, CurByte, MemoryOperand, MI, Desc, STI, OS);
  else
    EmitVEXOpcodePrefix(TSFlags, CurByte, MemoryOperand, MI, Desc, OS);

  unsigned char BaseOpcode = Cse523II::getBaseOpcodeFor(TSFlags);

  if ((TSFlags >> Cse523II::VEXShift) & Cse523II::Has3DNow0F0FOpcode)
    BaseOpcode = 0x0F;   // Weird 3DNow! encoding.

  unsigned SrcRegNum = 0;
  switch (TSFlags & Cse523II::FormMask) {
  default: errs() << "FORM: " << (TSFlags & Cse523II::FormMask) << "\n";
    llvm_unreachable("Unknown FormMask value in Cse523MCCodeEmitter!");
  case Cse523II::Pseudo:
    llvm_unreachable("Pseudo instruction shouldn't be emitted");
  case Cse523II::RawFrmDstSrc: {
    unsigned siReg = MI.getOperand(1).getReg();
    assert(((siReg == Cse523::RSI && MI.getOperand(0).getReg() == Cse523::RDI)) &&
           "SI and DI register sizes do not match");
    // Emit segment override opcode prefix as needed (not for %ds).
    if (MI.getOperand(2).getReg() != Cse523::DS)
      EmitSegmentOverridePrefix(CurByte, 2, MI, OS);
    CurOp += 3; // Consume operands.
    EmitByte(BaseOpcode, CurByte, OS);
    break;
  }
  case Cse523II::RawFrmSrc: {
    unsigned siReg = MI.getOperand(0).getReg();
    // Emit segment override opcode prefix as needed (not for %ds).
    if (MI.getOperand(1).getReg() != Cse523::DS)
      EmitSegmentOverridePrefix(CurByte, 1, MI, OS);
    CurOp += 2; // Consume operands.
    EmitByte(BaseOpcode, CurByte, OS);
    break;
  }
  case Cse523II::RawFrmDst: {
    unsigned siReg = MI.getOperand(0).getReg();
    // Emit AdSize prefix as needed.
    ++CurOp; // Consume operand.
    EmitByte(BaseOpcode, CurByte, OS);
    break;
  }
  case Cse523II::RawFrm:
    EmitByte(BaseOpcode, CurByte, OS);
    break;
  case Cse523II::RawFrmMemOffs:
    // Emit segment override opcode prefix as needed.
    EmitSegmentOverridePrefix(CurByte, 1, MI, OS);
    EmitByte(BaseOpcode, CurByte, OS);
    EmitImmediate(MI.getOperand(CurOp++), MI.getLoc(),
                  Cse523II::getSizeOfImm(TSFlags), getImmFixupKind(TSFlags),
                  CurByte, OS, Fixups);
    ++CurOp; // skip segment operand
    break;
  case Cse523II::RawFrmImm8:
    EmitByte(BaseOpcode, CurByte, OS);
    EmitImmediate(MI.getOperand(CurOp++), MI.getLoc(),
                  Cse523II::getSizeOfImm(TSFlags), getImmFixupKind(TSFlags),
                  CurByte, OS, Fixups);
    EmitImmediate(MI.getOperand(CurOp++), MI.getLoc(), 1, FK_Data_1, CurByte,
                  OS, Fixups);
    break;
  case Cse523II::RawFrmImm16:
    EmitByte(BaseOpcode, CurByte, OS);
    EmitImmediate(MI.getOperand(CurOp++), MI.getLoc(),
                  Cse523II::getSizeOfImm(TSFlags), getImmFixupKind(TSFlags),
                  CurByte, OS, Fixups);
    EmitImmediate(MI.getOperand(CurOp++), MI.getLoc(), 2, FK_Data_2, CurByte,
                  OS, Fixups);
    break;

  case Cse523II::AddRegFrm:
    EmitByte(BaseOpcode + GetCse523RegNum(MI.getOperand(CurOp++)), CurByte, OS);
    break;

  case Cse523II::MRMDestReg:
    EmitByte(BaseOpcode, CurByte, OS);
    SrcRegNum = CurOp + 1;

    EmitRegModRMByte(MI.getOperand(CurOp),
                     GetCse523RegNum(MI.getOperand(SrcRegNum)), CurByte, OS);
    CurOp = SrcRegNum + 1;
    break;

  case Cse523II::MRMDestMem:
    EmitByte(BaseOpcode, CurByte, OS);
    SrcRegNum = CurOp + Cse523::AddrNumOperands;

    EmitMemModRMByte(MI, CurOp,
                     GetCse523RegNum(MI.getOperand(SrcRegNum)),
                     TSFlags, CurByte, OS, Fixups, STI);
    CurOp = SrcRegNum + 1;
    break;

  case Cse523II::MRMSrcReg:
    EmitByte(BaseOpcode, CurByte, OS);
    SrcRegNum = CurOp + 1;

    EmitRegModRMByte(MI.getOperand(SrcRegNum),
                     GetCse523RegNum(MI.getOperand(CurOp)), CurByte, OS);

    // 2 operands skipped with HasMemOp4, compensate accordingly
    CurOp = SrcRegNum + 1;
    break;

  case Cse523II::MRMSrcMem: {
    int AddrOperands = Cse523::AddrNumOperands;
    unsigned FirstMemOp = CurOp+1;

    EmitByte(BaseOpcode, CurByte, OS);

    EmitMemModRMByte(MI, FirstMemOp, GetCse523RegNum(MI.getOperand(CurOp)),
                     TSFlags, CurByte, OS, Fixups, STI);
    CurOp += AddrOperands + 1;
    break;
  }

  case Cse523II::MRMXr:
  case Cse523II::MRM0r: case Cse523II::MRM1r:
  case Cse523II::MRM2r: case Cse523II::MRM3r:
  case Cse523II::MRM4r: case Cse523II::MRM5r:
  case Cse523II::MRM6r: case Cse523II::MRM7r: {
    EmitByte(BaseOpcode, CurByte, OS);
    uint64_t Form = TSFlags & Cse523II::FormMask;
    EmitRegModRMByte(MI.getOperand(CurOp++),
                     (Form == Cse523II::MRMXr) ? 0 : Form-Cse523II::MRM0r,
                     CurByte, OS);
    break;
  }

  case Cse523II::MRMXm:
  case Cse523II::MRM0m: case Cse523II::MRM1m:
  case Cse523II::MRM2m: case Cse523II::MRM3m:
  case Cse523II::MRM4m: case Cse523II::MRM5m:
  case Cse523II::MRM6m: case Cse523II::MRM7m: {
    EmitByte(BaseOpcode, CurByte, OS);
    uint64_t Form = TSFlags & Cse523II::FormMask;
    EmitMemModRMByte(MI, CurOp, (Form == Cse523II::MRMXm) ? 0 : Form-Cse523II::MRM0m,
                     TSFlags, CurByte, OS, Fixups, STI);
    CurOp += Cse523::AddrNumOperands;
    break;
  }
  case Cse523II::MRM_C0: case Cse523II::MRM_C1: case Cse523II::MRM_C2:
  case Cse523II::MRM_C3: case Cse523II::MRM_C4: case Cse523II::MRM_C8:
  case Cse523II::MRM_C9: case Cse523II::MRM_CA: case Cse523II::MRM_CB:
  case Cse523II::MRM_D0: case Cse523II::MRM_D1: case Cse523II::MRM_D4:
  case Cse523II::MRM_D5: case Cse523II::MRM_D6: case Cse523II::MRM_D8:
  case Cse523II::MRM_D9: case Cse523II::MRM_DA: case Cse523II::MRM_DB:
  case Cse523II::MRM_DC: case Cse523II::MRM_DD: case Cse523II::MRM_DE:
  case Cse523II::MRM_DF: case Cse523II::MRM_E0: case Cse523II::MRM_E1:
  case Cse523II::MRM_E2: case Cse523II::MRM_E3: case Cse523II::MRM_E4:
  case Cse523II::MRM_E5: case Cse523II::MRM_E8: case Cse523II::MRM_E9:
  case Cse523II::MRM_EA: case Cse523II::MRM_EB: case Cse523II::MRM_EC:
  case Cse523II::MRM_ED: case Cse523II::MRM_EE: case Cse523II::MRM_F0:
  case Cse523II::MRM_F1: case Cse523II::MRM_F2: case Cse523II::MRM_F3:
  case Cse523II::MRM_F4: case Cse523II::MRM_F5: case Cse523II::MRM_F6:
  case Cse523II::MRM_F7: case Cse523II::MRM_F8: case Cse523II::MRM_F9:
  case Cse523II::MRM_FA: case Cse523II::MRM_FB: case Cse523II::MRM_FC:
  case Cse523II::MRM_FD: case Cse523II::MRM_FE: case Cse523II::MRM_FF:
    EmitByte(BaseOpcode, CurByte, OS);

    unsigned char MRM;
    switch (TSFlags & Cse523II::FormMask) {
    default: llvm_unreachable("Invalid Form");
    case Cse523II::MRM_C0: MRM = 0xC0; break;
    case Cse523II::MRM_C1: MRM = 0xC1; break;
    case Cse523II::MRM_C2: MRM = 0xC2; break;
    case Cse523II::MRM_C3: MRM = 0xC3; break;
    case Cse523II::MRM_C4: MRM = 0xC4; break;
    case Cse523II::MRM_C8: MRM = 0xC8; break;
    case Cse523II::MRM_C9: MRM = 0xC9; break;
    case Cse523II::MRM_CA: MRM = 0xCA; break;
    case Cse523II::MRM_CB: MRM = 0xCB; break;
    case Cse523II::MRM_D0: MRM = 0xD0; break;
    case Cse523II::MRM_D1: MRM = 0xD1; break;
    case Cse523II::MRM_D4: MRM = 0xD4; break;
    case Cse523II::MRM_D5: MRM = 0xD5; break;
    case Cse523II::MRM_D6: MRM = 0xD6; break;
    case Cse523II::MRM_D8: MRM = 0xD8; break;
    case Cse523II::MRM_D9: MRM = 0xD9; break;
    case Cse523II::MRM_DA: MRM = 0xDA; break;
    case Cse523II::MRM_DB: MRM = 0xDB; break;
    case Cse523II::MRM_DC: MRM = 0xDC; break;
    case Cse523II::MRM_DD: MRM = 0xDD; break;
    case Cse523II::MRM_DE: MRM = 0xDE; break;
    case Cse523II::MRM_DF: MRM = 0xDF; break;
    case Cse523II::MRM_E0: MRM = 0xE0; break;
    case Cse523II::MRM_E1: MRM = 0xE1; break;
    case Cse523II::MRM_E2: MRM = 0xE2; break;
    case Cse523II::MRM_E3: MRM = 0xE3; break;
    case Cse523II::MRM_E4: MRM = 0xE4; break;
    case Cse523II::MRM_E5: MRM = 0xE5; break;
    case Cse523II::MRM_E8: MRM = 0xE8; break;
    case Cse523II::MRM_E9: MRM = 0xE9; break;
    case Cse523II::MRM_EA: MRM = 0xEA; break;
    case Cse523II::MRM_EB: MRM = 0xEB; break;
    case Cse523II::MRM_EC: MRM = 0xEC; break;
    case Cse523II::MRM_ED: MRM = 0xED; break;
    case Cse523II::MRM_EE: MRM = 0xEE; break;
    case Cse523II::MRM_F0: MRM = 0xF0; break;
    case Cse523II::MRM_F1: MRM = 0xF1; break;
    case Cse523II::MRM_F2: MRM = 0xF2; break;
    case Cse523II::MRM_F3: MRM = 0xF3; break;
    case Cse523II::MRM_F4: MRM = 0xF4; break;
    case Cse523II::MRM_F5: MRM = 0xF5; break;
    case Cse523II::MRM_F6: MRM = 0xF6; break;
    case Cse523II::MRM_F7: MRM = 0xF7; break;
    case Cse523II::MRM_F8: MRM = 0xF8; break;
    case Cse523II::MRM_F9: MRM = 0xF9; break;
    case Cse523II::MRM_FA: MRM = 0xFA; break;
    case Cse523II::MRM_FB: MRM = 0xFB; break;
    case Cse523II::MRM_FC: MRM = 0xFC; break;
    case Cse523II::MRM_FD: MRM = 0xFD; break;
    case Cse523II::MRM_FE: MRM = 0xFE; break;
    case Cse523II::MRM_FF: MRM = 0xFF; break;
    }
    EmitByte(MRM, CurByte, OS);
    break;
  }

  // If there is a remaining operand, it must be a trailing immediate.  Emit it
  // according to the right size for the instruction. Some instructions
  // (SSE4a extrq and insertq) have two trailing immediates.
  while (CurOp != NumOps && NumOps - CurOp <= 2) {
    // The last source register of a 4 operand instruction in AVX is encoded
    // in bits[7:4] of a immediate byte.
    EmitImmediate(MI.getOperand(CurOp++), MI.getLoc(),
                Cse523II::getSizeOfImm(TSFlags), getImmFixupKind(TSFlags),
                CurByte, OS, Fixups);
  }

  if ((TSFlags >> Cse523II::VEXShift) & Cse523II::Has3DNow0F0FOpcode)
    EmitByte(Cse523II::getBaseOpcodeFor(TSFlags), CurByte, OS);

#ifndef NDEBUG
  // FIXME: Verify.
  if (/*!Desc.isVariadic() &&*/ CurOp != NumOps) {
    errs() << "Cannot encode all operands of: ";
    MI.dump();
    errs() << '\n';
    abort();
  }
#endif
}
