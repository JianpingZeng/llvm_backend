//===-- Cse523MCAsmInfo.h - Cse523 asm properties --------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the Cse523MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef Cse523TARGETASMINFO_H
#define Cse523TARGETASMINFO_H

#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCAsmInfoCOFF.h"
#include "llvm/MC/MCAsmInfoDarwin.h"
#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
  class Triple;

  class Cse523MCAsmInfoDarwin : public MCAsmInfoDarwin {
    virtual void anchor();
  public:
    explicit Cse523MCAsmInfoDarwin(const Triple &Triple);
  };

  struct Cse523_64MCAsmInfoDarwin : public Cse523MCAsmInfoDarwin {
    explicit Cse523_64MCAsmInfoDarwin(const Triple &Triple);
    virtual const MCExpr *
    getExprForPersonalitySymbol(const MCSymbol *Sym,
                                unsigned Encoding,
                                MCStreamer &Streamer) const;
  };

  class Cse523ELFMCAsmInfo : public MCAsmInfoELF {
    virtual void anchor();
  public:
    explicit Cse523ELFMCAsmInfo(const Triple &Triple);
    virtual const MCSection *getNonexecutableStackSection(MCContext &Ctx) const;
  };

  class Cse523MCAsmInfoMicrosoft : public MCAsmInfoMicrosoft {
    virtual void anchor();
  public:
    explicit Cse523MCAsmInfoMicrosoft(const Triple &Triple);
  };

  class Cse523MCAsmInfoGNUCOFF : public MCAsmInfoGNUCOFF {
    virtual void anchor();
  public:
    explicit Cse523MCAsmInfoGNUCOFF(const Triple &Triple);
  };
} // namespace llvm

#endif
