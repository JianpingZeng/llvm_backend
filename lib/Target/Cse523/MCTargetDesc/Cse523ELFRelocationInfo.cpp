//===-- Cse523ELFRelocationInfo.cpp ----------------------------------------===//
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
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Support/ELF.h"

using namespace llvm;
using namespace object;
using namespace ELF;

namespace {
class Cse523_64ELFRelocationInfo : public MCRelocationInfo {
public:
  Cse523_64ELFRelocationInfo(MCContext &Ctx) : MCRelocationInfo(Ctx) {}

  const MCExpr *createExprForRelocation(RelocationRef Rel) {
    uint64_t RelType; Rel.getType(RelType);
    symbol_iterator SymI = Rel.getSymbol();

    StringRef SymName; SymI->getName(SymName);
    uint64_t  SymAddr; SymI->getAddress(SymAddr);
    uint64_t  SymSize; SymI->getSize(SymSize);
    int64_t  Addend;  getELFRelocationAddend(Rel, Addend);

    MCSymbol *Sym = Ctx.GetOrCreateSymbol(SymName);
    // FIXME: check that the value is actually the same.
    if (Sym->isVariable() == false)
      Sym->setVariableValue(MCConstantExpr::Create(SymAddr, Ctx));

    const MCExpr *Expr = 0;
    // If hasAddend is true, then we need to add Addend (r_addend) to Expr.
    bool hasAddend = false;

    // The AMD64 SysV ABI says:
    // A: the addend used to compute the value of the relocatable field.
    // B: the base address at which a shared object has been loaded into memory
    //    during execution. Generally, a shared object is built with a 0 base
    //    virtual address, but the execution address will be different.
    // G: the offset into the global offset table at which the relocation
    //    entry's symbol will reside during execution.
    // GOT: the address of the global offset table.
    // L: the place (section offset or address) of the Procedure Linkage Table
    //    entry for a symbol.
    // P: the place (section offset or address) of the storage unit being
    //    relocated (computed using r_offset).
    // S: the value of the symbol whose index resides in the relocation entry.
    // Z: the size of the symbol whose index resides in the relocation entry.

    switch(RelType) {
    case R_CSE523_NONE:
    case R_CSE523_COPY:
      // none
      break;
    case R_CSE523_64:
    case R_CSE523_16:
    case R_CSE523_8:
      // S + A
    case R_CSE523_32:
    case R_CSE523_32S:
      // S + A (We don't care about the result not fitting in 32 bits.)
    case R_CSE523_PC32:
    case R_CSE523_PC16:
    case R_CSE523_PC8:
    case R_CSE523_PC64:
      // S + A - P (P/pcrel is implicit)
      hasAddend = true;
      Expr = MCSymbolRefExpr::Create(Sym, Ctx);
      break;
    case R_CSE523_GOT32:
    case R_CSE523_GOT64:
    case R_CSE523_GOTPC32:
    case R_CSE523_GOTPC64:
    case R_CSE523_GOTPLT64:
      // G + A
      hasAddend = true;
      Expr = MCSymbolRefExpr::Create(Sym, MCSymbolRefExpr::VK_GOT, Ctx);
      break;
    case R_CSE523_PLT32:
      // L + A - P -> S@PLT + A
      hasAddend = true;
      Expr = MCSymbolRefExpr::Create(Sym, MCSymbolRefExpr::VK_PLT, Ctx);
      break;
    case R_CSE523_GLOB_DAT:
    case R_CSE523_JUMP_SLOT:
      // S
      Expr = MCSymbolRefExpr::Create(Sym, Ctx);
      break;
    case R_CSE523_GOTPCREL:
    case R_CSE523_GOTPCREL64:
      // G + GOT + A - P -> S@GOTPCREL + A
      hasAddend = true;
      Expr = MCSymbolRefExpr::Create(Sym, MCSymbolRefExpr::VK_GOTPCREL, Ctx);
      break;
    case R_CSE523_GOTOFF64:
      // S + A - GOT
      Expr = MCSymbolRefExpr::Create(Sym, MCSymbolRefExpr::VK_GOTOFF, Ctx);
      break;
    case R_CSE523_PLTOFF64:
      // L + A - GOT
      break;
    case R_CSE523_SIZE32:
    case R_CSE523_SIZE64:
      // Z + A
      Expr = MCConstantExpr::Create(SymSize, Ctx);
      break;
    default:
      Expr = MCSymbolRefExpr::Create(Sym, Ctx);
      break;
    }
    if (Expr && hasAddend && Addend != 0)
      Expr = MCBinaryExpr::CreateAdd(Expr,
                                     MCConstantExpr::Create(Addend, Ctx),
                                     Ctx);
    return Expr;
  }
};
} // End unnamed namespace

/// createCse523ELFRelocationInfo - Construct an Cse523 Mach-O RelocationInfo.
MCRelocationInfo *llvm::createCse523_64ELFRelocationInfo(MCContext &Ctx) {
  // We only handle cse523-64 for now.
  return new Cse523_64ELFRelocationInfo(Ctx);
}
