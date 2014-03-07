//===-- Cse523FixupKinds.h - Cse523 Specific Fixup Entries ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_Cse523_Cse523FIXUPKINDS_H
#define LLVM_Cse523_Cse523FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace Cse523 {
enum Fixups {
  reloc_riprel_4byte = FirstTargetFixupKind, // 32-bit rip-relative
  reloc_riprel_4byte_movq_load,              // 32-bit rip-relative in movq
  reloc_signed_4byte,                        // 32-bit signed. Unlike FK_Data_4
                                             // this will be sign extended at
                                             // runtime.
  reloc_global_offset_table,                 // 32-bit, relative to the start
                                             // of the instruction. Used only
                                             // for _GLOBAL_OFFSET_TABLE_.
  // Marker
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};
}
}

#endif
