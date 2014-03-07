//===-- Cse523TargetStreamer.h - Cse523 Target Streamer ----------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CSE523TARGETSTREAMER_H
#define CSE523TARGETSTREAMER_H

#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCStreamer.h"

namespace llvm {
class Cse523TargetStreamer : public MCTargetStreamer {
  virtual void anchor();

public:
  Cse523TargetStreamer(MCStreamer &S);
  /// Emit ".register <reg>, #ignore".
  virtual void emitCse523RegisterIgnore(unsigned reg) = 0;
  /// Emit ".register <reg>, #scratch".
  virtual void emitCse523RegisterScratch(unsigned reg) = 0;
};

// This part is for ascii assembly output
class Cse523TargetAsmStreamer : public Cse523TargetStreamer {
  formatted_raw_ostream &OS;

public:
  Cse523TargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);
  virtual void emitCse523RegisterIgnore(unsigned reg);
  virtual void emitCse523RegisterScratch(unsigned reg);

};

// This part is for ELF object output
class Cse523TargetELFStreamer : public Cse523TargetStreamer {
public:
  Cse523TargetELFStreamer(MCStreamer &S);
  MCELFStreamer &getStreamer();
  virtual void emitCse523RegisterIgnore(unsigned reg) {}
  virtual void emitCse523RegisterScratch(unsigned reg) {}
};
} // end namespace llvm

#endif
