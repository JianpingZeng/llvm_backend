//===-- Cse523MCAsmInfo.h - Cse523 asm properties ----------------*- C++ -*--===//
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

#ifndef CSE523TARGETASMINFO_H
#define CSE523TARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class StringRef;

class Cse523ELFMCAsmInfo : public MCAsmInfoELF {
  virtual void anchor();
public:
  explicit Cse523ELFMCAsmInfo(StringRef TT);
  virtual const MCExpr* getExprForPersonalitySymbol(const MCSymbol *Sym,
                                                    unsigned Encoding,
                                                    MCStreamer &Streamer) const;
  virtual const MCExpr* getExprForFDESymbol(const MCSymbol *Sym,
                                            unsigned Encoding,
                                            MCStreamer &Streamer) const;

};

} // namespace llvm

#endif
