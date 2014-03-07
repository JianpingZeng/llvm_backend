//===-- Cse523MCExpr.cpp - Cse523 specific MC expression classes --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the implementation of the assembly expression modifiers
// accepted by the Cse523 architecture (e.g. "%hi", "%lo", ...).
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "cse523mcexpr"
#include "Cse523MCExpr.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELF.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Object/ELF.h"


using namespace llvm;

const Cse523MCExpr*
Cse523MCExpr::Create(VariantKind Kind, const MCExpr *Expr,
                      MCContext &Ctx) {
    return new (Ctx) Cse523MCExpr(Kind, Expr);
}



void Cse523MCExpr::PrintImpl(raw_ostream &OS) const
{

  bool closeParen = printVariantKind(OS, Kind);

  const MCExpr *Expr = getSubExpr();
  Expr->print(OS);

  if (closeParen)
    OS << ')';
}

bool Cse523MCExpr::printVariantKind(raw_ostream &OS, VariantKind Kind)
{
  bool closeParen = true;
  switch (Kind) {
  case VK_Cse523_None:     closeParen = false; break;
  case VK_Cse523_LO:       OS << "%lo(";  break;
  case VK_Cse523_HI:       OS << "%hi(";  break;
  case VK_Cse523_H44:      OS << "%h44("; break;
  case VK_Cse523_M44:      OS << "%m44("; break;
  case VK_Cse523_L44:      OS << "%l44("; break;
  case VK_Cse523_HH:       OS << "%hh(";  break;
  case VK_Cse523_HM:       OS << "%hm(";  break;
    // FIXME: use %pc22/%pc10, if system assembler supports them.
  case VK_Cse523_PC22:     OS << "%hi("; break;
  case VK_Cse523_PC10:     OS << "%lo("; break;
    // FIXME: use %got22/%got10, if system assembler supports them.
  case VK_Cse523_GOT22:    OS << "%hi("; break;
  case VK_Cse523_GOT10:    OS << "%lo("; break;
  case VK_Cse523_WPLT30:   closeParen = false; break;
  case VK_Cse523_R_DISP32: OS << "%r_disp32("; break;
  case VK_Cse523_TLS_GD_HI22:   OS << "%tgd_hi22(";   break;
  case VK_Cse523_TLS_GD_LO10:   OS << "%tgd_lo10(";   break;
  case VK_Cse523_TLS_GD_ADD:    OS << "%tgd_add(";    break;
  case VK_Cse523_TLS_GD_CALL:   OS << "%tgd_call(";   break;
  case VK_Cse523_TLS_LDM_HI22:  OS << "%tldm_hi22(";  break;
  case VK_Cse523_TLS_LDM_LO10:  OS << "%tldm_lo10(";  break;
  case VK_Cse523_TLS_LDM_ADD:   OS << "%tldm_add(";   break;
  case VK_Cse523_TLS_LDM_CALL:  OS << "%tldm_call(";  break;
  case VK_Cse523_TLS_LDO_HIX22: OS << "%tldo_hix22("; break;
  case VK_Cse523_TLS_LDO_LOX10: OS << "%tldo_lox10("; break;
  case VK_Cse523_TLS_LDO_ADD:   OS << "%tldo_add(";   break;
  case VK_Cse523_TLS_IE_HI22:   OS << "%tie_hi22(";   break;
  case VK_Cse523_TLS_IE_LO10:   OS << "%tie_lo10(";   break;
  case VK_Cse523_TLS_IE_LD:     OS << "%tie_ld(";     break;
  case VK_Cse523_TLS_IE_LDX:    OS << "%tie_ldx(";    break;
  case VK_Cse523_TLS_IE_ADD:    OS << "%tie_add(";    break;
  case VK_Cse523_TLS_LE_HIX22:  OS << "%tle_hix22(";  break;
  case VK_Cse523_TLS_LE_LOX10:  OS << "%tle_lox10(";  break;
  }
  return closeParen;
}

Cse523MCExpr::VariantKind Cse523MCExpr::parseVariantKind(StringRef name)
{
  return StringSwitch<Cse523MCExpr::VariantKind>(name)
    .Case("lo",  VK_Cse523_LO)
    .Case("hi",  VK_Cse523_HI)
    .Case("h44", VK_Cse523_H44)
    .Case("m44", VK_Cse523_M44)
    .Case("l44", VK_Cse523_L44)
    .Case("hh",  VK_Cse523_HH)
    .Case("hm",  VK_Cse523_HM)
    .Case("pc22",  VK_Cse523_PC22)
    .Case("pc10",  VK_Cse523_PC10)
    .Case("got22", VK_Cse523_GOT22)
    .Case("got10", VK_Cse523_GOT10)
    .Case("r_disp32",   VK_Cse523_R_DISP32)
    .Case("tgd_hi22",   VK_Cse523_TLS_GD_HI22)
    .Case("tgd_lo10",   VK_Cse523_TLS_GD_LO10)
    .Case("tgd_add",    VK_Cse523_TLS_GD_ADD)
    .Case("tgd_call",   VK_Cse523_TLS_GD_CALL)
    .Case("tldm_hi22",  VK_Cse523_TLS_LDM_HI22)
    .Case("tldm_lo10",  VK_Cse523_TLS_LDM_LO10)
    .Case("tldm_add",   VK_Cse523_TLS_LDM_ADD)
    .Case("tldm_call",  VK_Cse523_TLS_LDM_CALL)
    .Case("tldo_hix22", VK_Cse523_TLS_LDO_HIX22)
    .Case("tldo_lox10", VK_Cse523_TLS_LDO_LOX10)
    .Case("tldo_add",   VK_Cse523_TLS_LDO_ADD)
    .Case("tie_hi22",   VK_Cse523_TLS_IE_HI22)
    .Case("tie_lo10",   VK_Cse523_TLS_IE_LO10)
    .Case("tie_ld",     VK_Cse523_TLS_IE_LD)
    .Case("tie_ldx",    VK_Cse523_TLS_IE_LDX)
    .Case("tie_add",    VK_Cse523_TLS_IE_ADD)
    .Case("tle_hix22",  VK_Cse523_TLS_LE_HIX22)
    .Case("tle_lox10",  VK_Cse523_TLS_LE_LOX10)
    .Default(VK_Cse523_None);
}

Cse523::Fixups Cse523MCExpr::getFixupKind(Cse523MCExpr::VariantKind Kind) {
  switch (Kind) {
  default:           assert(0 && "Unhandled Cse523MCExpr::VariantKind");
  case VK_Cse523_LO:       return Cse523::fixup_cse523_lo10;
  case VK_Cse523_HI:       return Cse523::fixup_cse523_hi22;
  case VK_Cse523_H44:      return Cse523::fixup_cse523_h44;
  case VK_Cse523_M44:      return Cse523::fixup_cse523_m44;
  case VK_Cse523_L44:      return Cse523::fixup_cse523_l44;
  case VK_Cse523_HH:       return Cse523::fixup_cse523_hh;
  case VK_Cse523_HM:       return Cse523::fixup_cse523_hm;
  case VK_Cse523_PC22:     return Cse523::fixup_cse523_pc22;
  case VK_Cse523_PC10:     return Cse523::fixup_cse523_pc10;
  case VK_Cse523_GOT22:    return Cse523::fixup_cse523_got22;
  case VK_Cse523_GOT10:    return Cse523::fixup_cse523_got10;
  case VK_Cse523_WPLT30:   return Cse523::fixup_cse523_wplt30;
  case VK_Cse523_TLS_GD_HI22:   return Cse523::fixup_cse523_tls_gd_hi22;
  case VK_Cse523_TLS_GD_LO10:   return Cse523::fixup_cse523_tls_gd_lo10;
  case VK_Cse523_TLS_GD_ADD:    return Cse523::fixup_cse523_tls_gd_add;
  case VK_Cse523_TLS_GD_CALL:   return Cse523::fixup_cse523_tls_gd_call;
  case VK_Cse523_TLS_LDM_HI22:  return Cse523::fixup_cse523_tls_ldm_hi22;
  case VK_Cse523_TLS_LDM_LO10:  return Cse523::fixup_cse523_tls_ldm_lo10;
  case VK_Cse523_TLS_LDM_ADD:   return Cse523::fixup_cse523_tls_ldm_add;
  case VK_Cse523_TLS_LDM_CALL:  return Cse523::fixup_cse523_tls_ldm_call;
  case VK_Cse523_TLS_LDO_HIX22: return Cse523::fixup_cse523_tls_ldo_hix22;
  case VK_Cse523_TLS_LDO_LOX10: return Cse523::fixup_cse523_tls_ldo_lox10;
  case VK_Cse523_TLS_LDO_ADD:   return Cse523::fixup_cse523_tls_ldo_add;
  case VK_Cse523_TLS_IE_HI22:   return Cse523::fixup_cse523_tls_ie_hi22;
  case VK_Cse523_TLS_IE_LO10:   return Cse523::fixup_cse523_tls_ie_lo10;
  case VK_Cse523_TLS_IE_LD:     return Cse523::fixup_cse523_tls_ie_ld;
  case VK_Cse523_TLS_IE_LDX:    return Cse523::fixup_cse523_tls_ie_ldx;
  case VK_Cse523_TLS_IE_ADD:    return Cse523::fixup_cse523_tls_ie_add;
  case VK_Cse523_TLS_LE_HIX22:  return Cse523::fixup_cse523_tls_le_hix22;
  case VK_Cse523_TLS_LE_LOX10:  return Cse523::fixup_cse523_tls_le_lox10;
  }
}

bool
Cse523MCExpr::EvaluateAsRelocatableImpl(MCValue &Res,
                                       const MCAsmLayout *Layout) const {
  if (!Layout)
    return false;
  return getSubExpr()->EvaluateAsRelocatable(Res, *Layout);
}

static void fixELFSymbolsInTLSFixupsImpl(const MCExpr *Expr, MCAssembler &Asm) {
  switch (Expr->getKind()) {
  case MCExpr::Target:
    llvm_unreachable("Can't handle nested target expr!");
    break;

  case MCExpr::Constant:
    break;

  case MCExpr::Binary: {
    const MCBinaryExpr *BE = cast<MCBinaryExpr>(Expr);
    fixELFSymbolsInTLSFixupsImpl(BE->getLHS(), Asm);
    fixELFSymbolsInTLSFixupsImpl(BE->getRHS(), Asm);
    break;
  }

  case MCExpr::SymbolRef: {
    const MCSymbolRefExpr &SymRef = *cast<MCSymbolRefExpr>(Expr);
    MCSymbolData &SD = Asm.getOrCreateSymbolData(SymRef.getSymbol());
    MCELF::SetType(SD, ELF::STT_TLS);
    break;
  }

  case MCExpr::Unary:
    fixELFSymbolsInTLSFixupsImpl(cast<MCUnaryExpr>(Expr)->getSubExpr(), Asm);
    break;
  }

}

void Cse523MCExpr::fixELFSymbolsInTLSFixups(MCAssembler &Asm) const {
  switch(getKind()) {
  default: return;
  case VK_Cse523_TLS_GD_HI22:
  case VK_Cse523_TLS_GD_LO10:
  case VK_Cse523_TLS_GD_ADD:
  case VK_Cse523_TLS_GD_CALL:
  case VK_Cse523_TLS_LDM_HI22:
  case VK_Cse523_TLS_LDM_LO10:
  case VK_Cse523_TLS_LDM_ADD:
  case VK_Cse523_TLS_LDM_CALL:
  case VK_Cse523_TLS_LDO_HIX22:
  case VK_Cse523_TLS_LDO_LOX10:
  case VK_Cse523_TLS_LDO_ADD:
  case VK_Cse523_TLS_IE_HI22:
  case VK_Cse523_TLS_IE_LO10:
  case VK_Cse523_TLS_IE_LD:
  case VK_Cse523_TLS_IE_LDX:
  case VK_Cse523_TLS_IE_ADD:
  case VK_Cse523_TLS_LE_HIX22:
  case VK_Cse523_TLS_LE_LOX10: break;
  }
  fixELFSymbolsInTLSFixupsImpl(getSubExpr(), Asm);
}

// FIXME: This basically copies MCObjectStreamer::AddValueSymbols. Perhaps
// that method should be made public?
// FIXME: really do above: now that at least three other backends are using it.
static void AddValueSymbolsImpl(const MCExpr *Value, MCAssembler *Asm) {
  switch (Value->getKind()) {
  case MCExpr::Target:
    llvm_unreachable("Can't handle nested target expr!");
    break;

  case MCExpr::Constant:
    break;

  case MCExpr::Binary: {
    const MCBinaryExpr *BE = cast<MCBinaryExpr>(Value);
    AddValueSymbolsImpl(BE->getLHS(), Asm);
    AddValueSymbolsImpl(BE->getRHS(), Asm);
    break;
  }

  case MCExpr::SymbolRef:
    Asm->getOrCreateSymbolData(cast<MCSymbolRefExpr>(Value)->getSymbol());
    break;

  case MCExpr::Unary:
    AddValueSymbolsImpl(cast<MCUnaryExpr>(Value)->getSubExpr(), Asm);
    break;
  }
}

void Cse523MCExpr::AddValueSymbols(MCAssembler *Asm) const {
  AddValueSymbolsImpl(getSubExpr(), Asm);
}
