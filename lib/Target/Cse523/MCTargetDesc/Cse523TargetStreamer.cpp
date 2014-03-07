//===-- Cse523TargetStreamer.cpp - Cse523 Target Streamer Methods -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Cse523 specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "Cse523TargetStreamer.h"
#include "InstPrinter/Cse523InstPrinter.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

// pin vtable to this file
Cse523TargetStreamer::Cse523TargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

void Cse523TargetStreamer::anchor() {}

Cse523TargetAsmStreamer::Cse523TargetAsmStreamer(MCStreamer &S,
                                               formatted_raw_ostream &OS)
    : Cse523TargetStreamer(S), OS(OS) {}

void Cse523TargetAsmStreamer::emitCse523RegisterIgnore(unsigned reg) {
  OS << "\t.register "
     << "%" << StringRef(Cse523InstPrinter::getRegisterName(reg)).lower()
     << ", #ignore\n";
}

void Cse523TargetAsmStreamer::emitCse523RegisterScratch(unsigned reg) {
  OS << "\t.register "
     << "%" << StringRef(Cse523InstPrinter::getRegisterName(reg)).lower()
     << ", #scratch\n";
}

Cse523TargetELFStreamer::Cse523TargetELFStreamer(MCStreamer &S)
    : Cse523TargetStreamer(S) {}

MCELFStreamer &Cse523TargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}
