//=- Cse523InstComments.h - Generate verbose-asm comments for instrs -*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This defines functionality used to emit comments about Cse523 instructions to
// an output stream for -fverbose-asm.
//
//===----------------------------------------------------------------------===//

#ifndef CSE523_INST_COMMENTS_H
#define CSE523_INST_COMMENTS_H

namespace llvm {
  class MCInst;
  class raw_ostream;
  void EmitAnyCse523InstComments(const MCInst *MI, raw_ostream &OS,
                              const char *(*getRegName)(unsigned));
}

#endif
