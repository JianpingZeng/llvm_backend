//===-- Cse523ELFObjectWriter.cpp - Cse523 ELF Writer ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Cse523FixupKinds.h"
#include "MCTargetDesc/Cse523MCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
  class Cse523ELFObjectWriter : public MCELFObjectTargetWriter {
  public:
    Cse523ELFObjectWriter(bool IsELF64, uint8_t OSABI, uint16_t EMachine);

    virtual ~Cse523ELFObjectWriter();
  protected:
    virtual unsigned GetRelocType(const MCValue &Target, const MCFixup &Fixup,
                                  bool IsPCRel, bool IsRelocWithSymbol,
                                  int64_t Addend) const;
  };
}

Cse523ELFObjectWriter::Cse523ELFObjectWriter(bool IsELF64, uint8_t OSABI,
                                       uint16_t EMachine)
  : MCELFObjectTargetWriter(IsELF64, OSABI, EMachine,
                            // Only i386 uses Rel instead of RelA.
                            /*HasRelocationAddend*/ EMachine != ELF::EM_386) {}

Cse523ELFObjectWriter::~Cse523ELFObjectWriter()
{}

unsigned Cse523ELFObjectWriter::GetRelocType(const MCValue &Target,
                                          const MCFixup &Fixup,
                                          bool IsPCRel,
                                          bool IsRelocWithSymbol,
                                          int64_t Addend) const {
  // determine the type of the relocation

  MCSymbolRefExpr::VariantKind Modifier = Target.isAbsolute() ?
    MCSymbolRefExpr::VK_None : Target.getSymA()->getKind();
  unsigned Type;
  if (getEMachine() == ELF::EM_CSE523) {
    if (IsPCRel) {
      switch ((unsigned)Fixup.getKind()) {
      default: llvm_unreachable("invalid fixup kind!");

      case FK_Data_8: Type = ELF::R_CSE523_PC64; break;
      case FK_Data_4: Type = ELF::R_CSE523_PC32; break;
      case FK_Data_2: Type = ELF::R_CSE523_PC16; break;
      case FK_Data_1: Type = ELF::R_CSE523_PC8; break;

      case FK_PCRel_8:
        assert(Modifier == MCSymbolRefExpr::VK_None);
        Type = ELF::R_CSE523_PC64;
        break;
      case Cse523::reloc_signed_4byte:
      case Cse523::reloc_riprel_4byte_movq_load:
      case Cse523::reloc_riprel_4byte:
      case FK_PCRel_4:
        switch (Modifier) {
        default:
          llvm_unreachable("Unimplemented");
        case MCSymbolRefExpr::VK_None:
          Type = ELF::R_CSE523_PC32;
          break;
        case MCSymbolRefExpr::VK_PLT:
          Type = ELF::R_CSE523_PLT32;
          break;
        case MCSymbolRefExpr::VK_GOTPCREL:
          Type = ELF::R_CSE523_GOTPCREL;
          break;
        case MCSymbolRefExpr::VK_GOTTPOFF:
          Type = ELF::R_CSE523_GOTTPOFF;
        break;
        case MCSymbolRefExpr::VK_TLSGD:
          Type = ELF::R_CSE523_TLSGD;
          break;
        case MCSymbolRefExpr::VK_TLSLD:
          Type = ELF::R_CSE523_TLSLD;
          break;
        }
        break;
      case FK_PCRel_2:
        assert(Modifier == MCSymbolRefExpr::VK_None);
        Type = ELF::R_CSE523_PC16;
        break;
      case FK_PCRel_1:
        assert(Modifier == MCSymbolRefExpr::VK_None);
        Type = ELF::R_CSE523_PC8;
        break;
      }
    } else {
      switch ((unsigned)Fixup.getKind()) {
      default: llvm_unreachable("invalid fixup kind!");
      case FK_Data_8:
        switch (Modifier) {
        default:
          llvm_unreachable("Unimplemented");
        case MCSymbolRefExpr::VK_None:
          Type = ELF::R_CSE523_64;
          break;
        case MCSymbolRefExpr::VK_GOT:
          Type = ELF::R_CSE523_GOT64;
          break;
        case MCSymbolRefExpr::VK_GOTOFF:
          Type = ELF::R_CSE523_GOTOFF64;
          break;
        case MCSymbolRefExpr::VK_TPOFF:
          Type = ELF::R_CSE523_TPOFF64;
          break;
        case MCSymbolRefExpr::VK_DTPOFF:
          Type = ELF::R_CSE523_DTPOFF64;
          break;
        }
        break;
      case Cse523::reloc_signed_4byte:
        switch (Modifier) {
        default:
          llvm_unreachable("Unimplemented");
        case MCSymbolRefExpr::VK_None:
          Type = ELF::R_CSE523_32S;
          break;
        case MCSymbolRefExpr::VK_GOT:
          Type = ELF::R_CSE523_GOT32;
          break;
        case MCSymbolRefExpr::VK_GOTPCREL:
          Type = ELF::R_CSE523_GOTPCREL;
          break;
        case MCSymbolRefExpr::VK_TPOFF:
          Type = ELF::R_CSE523_TPOFF32;
          break;
        case MCSymbolRefExpr::VK_DTPOFF:
          Type = ELF::R_CSE523_DTPOFF32;
          break;
        }
        break;
      case FK_Data_4:
        Type = ELF::R_CSE523_32;
        break;
      case FK_Data_2: Type = ELF::R_CSE523_16; break;
      case FK_PCRel_1:
      case FK_Data_1: Type = ELF::R_CSE523_8; break;
      }
    }
  } else
    llvm_unreachable("unsupported elf machine type.");

  return Type;
}

MCObjectWriter *llvm::createCse523ELFObjectWriter(raw_ostream &OS,
                                               bool IsELF64,
                                               uint8_t OSABI,
                                               uint16_t EMachine) {
  MCELFObjectTargetWriter *MOTW =
    new Cse523ELFObjectWriter(IsELF64, OSABI, EMachine);
  return createELFObjectWriter(MOTW, OS,  /*IsLittleEndian=*/true);
}
