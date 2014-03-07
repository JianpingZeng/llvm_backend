//===-- Cse523WinCOFFObjectWriter.cpp - Cse523 Win COFF Writer ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Cse523FixupKinds.h"
#include "MCTargetDesc/Cse523MCTargetDesc.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/MCWinCOFFObjectWriter.h"
#include "llvm/Support/COFF.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace llvm {
  class MCObjectWriter;
}

namespace {
  class Cse523WinCOFFObjectWriter : public MCWinCOFFObjectTargetWriter {
    const bool Is64Bit;

  public:
    Cse523WinCOFFObjectWriter(bool Is64Bit_);
    virtual ~Cse523WinCOFFObjectWriter();

    virtual unsigned getRelocType(const MCValue &Target,
                                  const MCFixup &Fixup,
                                  bool IsCrossSection) const LLVM_OVERRIDE;
  };
}

Cse523WinCOFFObjectWriter::Cse523WinCOFFObjectWriter(bool Is64Bit_)
  : MCWinCOFFObjectTargetWriter(Is64Bit_ ? COFF::IMAGE_FILE_MACHINE_AMD64 :
                                COFF::IMAGE_FILE_MACHINE_I386),
    Is64Bit(Is64Bit_) {}

Cse523WinCOFFObjectWriter::~Cse523WinCOFFObjectWriter() {}

unsigned Cse523WinCOFFObjectWriter::getRelocType(const MCValue &Target,
                                              const MCFixup &Fixup,
                                              bool IsCrossSection) const {
  unsigned FixupKind = IsCrossSection ? FK_PCRel_4 : Fixup.getKind();

  MCSymbolRefExpr::VariantKind Modifier = Target.isAbsolute() ?
    MCSymbolRefExpr::VK_None : Target.getSymA()->getKind();

  switch (FixupKind) {
  case FK_PCRel_4:
  case Cse523::reloc_riprel_4byte:
  case Cse523::reloc_riprel_4byte_movq_load:
    return Is64Bit ? COFF::IMAGE_REL_AMD64_REL32 : COFF::IMAGE_REL_I386_REL32;
  case FK_Data_4:
  case Cse523::reloc_signed_4byte:
    if (Modifier == MCSymbolRefExpr::VK_COFF_IMGREL32)
      return Is64Bit ? COFF::IMAGE_REL_AMD64_ADDR32NB :
                       COFF::IMAGE_REL_I386_DIR32NB;
    return Is64Bit ? COFF::IMAGE_REL_AMD64_ADDR32 : COFF::IMAGE_REL_I386_DIR32;
  case FK_Data_8:
    if (Is64Bit)
      return COFF::IMAGE_REL_AMD64_ADDR64;
    llvm_unreachable("unsupported relocation type");
  case FK_SecRel_2:
    return Is64Bit ? COFF::IMAGE_REL_AMD64_SECTION
                   : COFF::IMAGE_REL_I386_SECTION;
  case FK_SecRel_4:
    return Is64Bit ? COFF::IMAGE_REL_AMD64_SECREL : COFF::IMAGE_REL_I386_SECREL;
  default:
    llvm_unreachable("unsupported relocation type");
  }
}

MCObjectWriter *llvm::createCse523WinCOFFObjectWriter(raw_ostream &OS,
                                                   bool Is64Bit) {
  MCWinCOFFObjectTargetWriter *MOTW = new Cse523WinCOFFObjectWriter(Is64Bit);
  return createWinCOFFObjectWriter(MOTW, OS);
}
