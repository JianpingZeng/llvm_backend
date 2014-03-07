//===-- Cse523MachORelocationInfo.cpp ----------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Cse523MCTargetDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRelocationInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Object/MachO.h"

using namespace llvm;
using namespace object;
using namespace MachO;

namespace {
class Cse523_64MachORelocationInfo : public MCRelocationInfo {
public:
  Cse523_64MachORelocationInfo(MCContext &Ctx) : MCRelocationInfo(Ctx) {}

  const MCExpr *createExprForRelocation(RelocationRef Rel) {
    const MachOObjectFile *Obj = cast<MachOObjectFile>(Rel.getObjectFile());

    uint64_t RelType; Rel.getType(RelType);
    symbol_iterator SymI = Rel.getSymbol();

    StringRef SymName; SymI->getName(SymName);
    uint64_t  SymAddr; SymI->getAddress(SymAddr);

    any_relocation_info RE = Obj->getRelocation(Rel.getRawDataRefImpl());
    bool isPCRel = Obj->getAnyRelocationPCRel(RE);

    MCSymbol *Sym = Ctx.GetOrCreateSymbol(SymName);
    // FIXME: check that the value is actually the same.
    if (Sym->isVariable() == false)
      Sym->setVariableValue(MCConstantExpr::Create(SymAddr, Ctx));
    const MCExpr *Expr = 0;

    switch(RelType) {
    case CSE523_RELOC_TLV:
      Expr = MCSymbolRefExpr::Create(Sym, MCSymbolRefExpr::VK_TLVP, Ctx);
      break;
    case CSE523_RELOC_SIGNED_4:
      Expr = MCBinaryExpr::CreateAdd(MCSymbolRefExpr::Create(Sym, Ctx),
                                     MCConstantExpr::Create(4, Ctx),
                                     Ctx);
      break;
    case CSE523_RELOC_SIGNED_2:
      Expr = MCBinaryExpr::CreateAdd(MCSymbolRefExpr::Create(Sym, Ctx),
                                     MCConstantExpr::Create(2, Ctx),
                                     Ctx);
      break;
    case CSE523_RELOC_SIGNED_1:
      Expr = MCBinaryExpr::CreateAdd(MCSymbolRefExpr::Create(Sym, Ctx),
                                     MCConstantExpr::Create(1, Ctx),
                                     Ctx);
      break;
    case CSE523_RELOC_GOT_LOAD:
      Expr = MCSymbolRefExpr::Create(Sym, MCSymbolRefExpr::VK_GOTPCREL, Ctx);
      break;
    case CSE523_RELOC_GOT:
      Expr = MCSymbolRefExpr::Create(Sym, isPCRel ?
                                     MCSymbolRefExpr::VK_GOTPCREL :
                                     MCSymbolRefExpr::VK_GOT,
                                     Ctx);
      break;
    case CSE523_RELOC_SUBTRACTOR:
      {
        Rel.moveNext();
        any_relocation_info RENext =
            Obj->getRelocation(Rel.getRawDataRefImpl());

        // CSE523_SUBTRACTOR must be followed by a relocation of type
        // CSE523_RELOC_UNSIGNED.
        // NOTE: Scattered relocations don't exist on x86_64.
        unsigned RType = Obj->getAnyRelocationType(RENext);
        if (RType != CSE523_RELOC_UNSIGNED)
          report_fatal_error("Expected CSE523_RELOC_UNSIGNED after "
                             "CSE523_RELOC_SUBTRACTOR.");

        const MCExpr *LHS = MCSymbolRefExpr::Create(Sym, Ctx);

        symbol_iterator RSymI = Rel.getSymbol();
        uint64_t RSymAddr;
        RSymI->getAddress(RSymAddr);
        StringRef RSymName;
        RSymI->getName(RSymName);

        MCSymbol *RSym = Ctx.GetOrCreateSymbol(RSymName);
        if (RSym->isVariable() == false)
          RSym->setVariableValue(MCConstantExpr::Create(RSymAddr, Ctx));

        const MCExpr *RHS = MCSymbolRefExpr::Create(RSym, Ctx);

        Expr = MCBinaryExpr::CreateSub(LHS, RHS, Ctx);
        break;
      }
    default:
      Expr = MCSymbolRefExpr::Create(Sym, Ctx);
      break;
    }
    return Expr;
  }
};
} // End unnamed namespace

/// createCse523_64MachORelocationInfo - Construct an Cse523-64 Mach-O RelocationInfo.
MCRelocationInfo *llvm::createCse523_64MachORelocationInfo(MCContext &Ctx) {
  return new Cse523_64MachORelocationInfo(Ctx);
}
