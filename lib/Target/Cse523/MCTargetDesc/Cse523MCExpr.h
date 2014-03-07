//====- Cse523MCExpr.h - Cse523 specific MC expression classes --*- C++ -*-=====//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes Cse523-specific MCExprs, used for modifiers like
// "%hi" or "%lo" etc.,
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CSE523MCEXPR_H
#define LLVM_CSE523MCEXPR_H

#include "Cse523FixupKinds.h"
#include "llvm/MC/MCExpr.h"

namespace llvm {

class StringRef;
class Cse523MCExpr : public MCTargetExpr {
public:
  enum VariantKind {
    VK_Cse523_None,
    VK_Cse523_LO,
    VK_Cse523_HI,
    VK_Cse523_H44,
    VK_Cse523_M44,
    VK_Cse523_L44,
    VK_Cse523_HH,
    VK_Cse523_HM,
    VK_Cse523_PC22,
    VK_Cse523_PC10,
    VK_Cse523_GOT22,
    VK_Cse523_GOT10,
    VK_Cse523_WPLT30,
    VK_Cse523_R_DISP32,
    VK_Cse523_TLS_GD_HI22,
    VK_Cse523_TLS_GD_LO10,
    VK_Cse523_TLS_GD_ADD,
    VK_Cse523_TLS_GD_CALL,
    VK_Cse523_TLS_LDM_HI22,
    VK_Cse523_TLS_LDM_LO10,
    VK_Cse523_TLS_LDM_ADD,
    VK_Cse523_TLS_LDM_CALL,
    VK_Cse523_TLS_LDO_HIX22,
    VK_Cse523_TLS_LDO_LOX10,
    VK_Cse523_TLS_LDO_ADD,
    VK_Cse523_TLS_IE_HI22,
    VK_Cse523_TLS_IE_LO10,
    VK_Cse523_TLS_IE_LD,
    VK_Cse523_TLS_IE_LDX,
    VK_Cse523_TLS_IE_ADD,
    VK_Cse523_TLS_LE_HIX22,
    VK_Cse523_TLS_LE_LOX10
  };

private:
  const VariantKind Kind;
  const MCExpr *Expr;

  explicit Cse523MCExpr(VariantKind _Kind, const MCExpr *_Expr)
    : Kind(_Kind), Expr(_Expr) {}

public:
  /// @name Construction
  /// @{

  static const Cse523MCExpr *Create(VariantKind Kind, const MCExpr *Expr,
                                 MCContext &Ctx);
  /// @}
  /// @name Accessors
  /// @{

  /// getOpcode - Get the kind of this expression.
  VariantKind getKind() const { return Kind; }

  /// getSubExpr - Get the child of this expression.
  const MCExpr *getSubExpr() const { return Expr; }

  /// getFixupKind - Get the fixup kind of this expression.
  Cse523::Fixups getFixupKind() const { return getFixupKind(Kind); }

  /// @}
  void PrintImpl(raw_ostream &OS) const;
  bool EvaluateAsRelocatableImpl(MCValue &Res,
                                 const MCAsmLayout *Layout) const;
  void AddValueSymbols(MCAssembler *) const;
  const MCSection *FindAssociatedSection() const {
    return getSubExpr()->FindAssociatedSection();
  }

  void fixELFSymbolsInTLSFixups(MCAssembler &Asm) const;

  static bool classof(const MCExpr *E) {
    return E->getKind() == MCExpr::Target;
  }

  static bool classof(const Cse523MCExpr *) { return true; }

  static VariantKind parseVariantKind(StringRef name);
  static bool printVariantKind(raw_ostream &OS, VariantKind Kind);
  static Cse523::Fixups getFixupKind(VariantKind Kind);
};

} // end namespace llvm.

#endif
