//===-- Cse523MCCodeEmitter.cpp - Convert Cse523 code to machine code -------===//
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
#include "Cse523MCExpr.h"
#include "MCTargetDesc/Cse523FixupKinds.h"
#include "Cse523MCTargetDesc.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");

namespace {
class Cse523MCCodeEmitter : public MCCodeEmitter {
  Cse523MCCodeEmitter(const Cse523MCCodeEmitter &) LLVM_DELETED_FUNCTION;
  void operator=(const Cse523MCCodeEmitter &) LLVM_DELETED_FUNCTION;
  MCContext &Ctx;

public:
  Cse523MCCodeEmitter(MCContext &ctx): Ctx(ctx) {}

  ~Cse523MCCodeEmitter() {}

  virtual void reset() { }
  void EncodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const;

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  /// getMachineOpValue - Return binary encoding of operand. If the machine
  /// operand requires relocation, record the relocation and return zero.
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  unsigned getCallTargetOpValue(const MCInst &MI, unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;
  unsigned getBranchTargetOpValue(const MCInst &MI, unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

};
} // end anonymous namespace

MCCodeEmitter *llvm::createCse523MCCodeEmitter(const MCInstrInfo &MCII,
                                              const MCRegisterInfo &MRI,
                                              const MCSubtargetInfo &STI,
                                              MCContext &Ctx) {
  return new Cse523MCCodeEmitter(Ctx);
}

void Cse523MCCodeEmitter::
EncodeInstruction(const MCInst &MI, raw_ostream &OS,
                  SmallVectorImpl<MCFixup> &Fixups,
                  const MCSubtargetInfo &STI) const {
  unsigned Bits = getBinaryCodeForInstr(MI, Fixups, STI);

  // Output the constant in big endian byte order.
  for (unsigned i = 0; i != 4; ++i) {
    OS << (char)(Bits >> 24);
    Bits <<= 8;
  }
  unsigned tlsOpNo = 0;
  switch (MI.getOpcode()) {
  default: break;
  //case SP::TLS_CALL:   tlsOpNo = 1; break;
  //case SP::TLS_ADDrr:
  //case SP::TLS_ADDXrr:
  //case SP::TLS_LDrr:
  //case SP::TLS_LDXrr:  tlsOpNo = 3; break;
  }
  if (tlsOpNo != 0) {
    const MCOperand &MO = MI.getOperand(tlsOpNo);
    uint64_t op = getMachineOpValue(MI, MO, Fixups, STI);
    assert(op == 0 && "Unexpected operand value!");
    (void)op; // suppress warning.
  }

  ++MCNumEmitted;  // Keep track of the # of mi's emitted.
}


unsigned Cse523MCCodeEmitter::
getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                  SmallVectorImpl<MCFixup> &Fixups,
                  const MCSubtargetInfo &STI) const {

  if (MO.isReg())
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());

  if (MO.isImm())
    return MO.getImm();

  assert(MO.isExpr());
  const MCExpr *Expr = MO.getExpr();
  if (const Cse523MCExpr *SExpr = dyn_cast<Cse523MCExpr>(Expr)) {
    MCFixupKind Kind = (MCFixupKind)SExpr->getFixupKind();
    Fixups.push_back(MCFixup::Create(0, Expr, Kind));
    return 0;
  }

  int64_t Res;
  if (Expr->EvaluateAsAbsolute(Res))
    return Res;

  assert(0 && "Unhandled expression!");
  return 0;
}

unsigned Cse523MCCodeEmitter::
getCallTargetOpValue(const MCInst &MI, unsigned OpNo,
                     SmallVectorImpl<MCFixup> &Fixups,
                     const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);
  if (MO.isReg() || MO.isImm())
    return getMachineOpValue(MI, MO, Fixups, STI);

//  if (MI.getOpcode() == SP::TLS_CALL) {
//    // No fixups for __tls_get_addr. Will emit for fixups for tls_symbol in
//    // EncodeInstruction.
//#ifndef NDEBUG
//    // Verify that the callee is actually __tls_get_addr.
//    const Cse523MCExpr *SExpr = dyn_cast<Cse523MCExpr>(MO.getExpr());
//    assert(SExpr && SExpr->getSubExpr()->getKind() == MCExpr::SymbolRef &&
//           "Unexpected expression in TLS_CALL");
//    const MCSymbolRefExpr *SymExpr = cast<MCSymbolRefExpr>(SExpr->getSubExpr());
//    assert(SymExpr->getSymbol().getName() == "__tls_get_addr" &&
//           "Unexpected function for TLS_CALL");
//#endif
//    return 0;
//  }

  MCFixupKind fixupKind = (MCFixupKind)Cse523::fixup_cse523_call30;

  if (const Cse523MCExpr *SExpr = dyn_cast<Cse523MCExpr>(MO.getExpr())) {
    if (SExpr->getKind() == Cse523MCExpr::VK_Cse523_WPLT30)
      fixupKind = (MCFixupKind)Cse523::fixup_cse523_wplt30;
  }

  Fixups.push_back(MCFixup::Create(0, MO.getExpr(), fixupKind));

  return 0;
}

unsigned Cse523MCCodeEmitter::
getBranchTargetOpValue(const MCInst &MI, unsigned OpNo,
                  SmallVectorImpl<MCFixup> &Fixups,
                  const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);
  if (MO.isReg() || MO.isImm())
    return getMachineOpValue(MI, MO, Fixups, STI);

  Cse523::Fixups fixup = Cse523::fixup_cse523_br22;
//  if (MI.getOpcode() == SP::BPXCC)
//    fixup = Cse523::fixup_cse523_br19;

  Fixups.push_back(MCFixup::Create(0, MO.getExpr(),
                                   (MCFixupKind)fixup));
  return 0;
}

//#include "Cse523GenMCCodeEmitter.inc"
