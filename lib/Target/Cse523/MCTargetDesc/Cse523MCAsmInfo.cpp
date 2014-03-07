//===-- Cse523MCAsmInfo.cpp - Cse523 asm properties -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the Cse523MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "Cse523MCAsmInfo.h"
#include "Cse523MCExpr.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCStreamer.h"

using namespace llvm;

void Cse523ELFMCAsmInfo::anchor() { }

Cse523ELFMCAsmInfo::Cse523ELFMCAsmInfo(StringRef TT) {
  IsLittleEndian = true;
  Triple TheTriple(TT);
  bool isV9 = (TheTriple.getArch() == Triple::cse523);

  if (isV9) {
    PointerSize = CalleeSaveStackSlotSize = 8;
  }

  Data16bitsDirective = "\t.half\t";
  Data32bitsDirective = "\t.word\t";
  // .xword is only supported by V9.
  Data64bitsDirective = (isV9) ? "\t.xword\t" : 0;
  ZeroDirective = "\t.skip\t";
  CommentString = "!";
  HasLEB128 = true;
  SupportsDebugInformation = true;

  ExceptionsType = ExceptionHandling::DwarfCFI;

  SunStyleELFSectionSwitchSyntax = true;
  UsesELFSectionDirectiveForBSS = true;

  if (TheTriple.getOS() == llvm::Triple::Solaris)
    UseIntegratedAssembler = true;
}

const MCExpr*
Cse523ELFMCAsmInfo::getExprForPersonalitySymbol(const MCSymbol *Sym,
                                               unsigned Encoding,
                                               MCStreamer &Streamer) const {
  if (Encoding & dwarf::DW_EH_PE_pcrel) {
    MCContext &Ctx = Streamer.getContext();
    return Cse523MCExpr::Create(Cse523MCExpr::VK_Cse523_R_DISP32,
                               MCSymbolRefExpr::Create(Sym, Ctx), Ctx);
  }

  return MCAsmInfo::getExprForPersonalitySymbol(Sym, Encoding, Streamer);
}

const MCExpr*
Cse523ELFMCAsmInfo::getExprForFDESymbol(const MCSymbol *Sym,
                                       unsigned Encoding,
                                       MCStreamer &Streamer) const {
  if (Encoding & dwarf::DW_EH_PE_pcrel) {
    MCContext &Ctx = Streamer.getContext();
    return Cse523MCExpr::Create(Cse523MCExpr::VK_Cse523_R_DISP32,
                               MCSymbolRefExpr::Create(Sym, Ctx), Ctx);
  }
  return MCAsmInfo::getExprForFDESymbol(Sym, Encoding, Streamer);
}
