//===-- Cse523Relocations.h - Cse523 Code Relocations -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the Cse523 target-specific relocation types
// (for relocation-model=static).
//
//===----------------------------------------------------------------------===//

#ifndef CSE523_RELOCATIONS_H
#define CSE523_RELOCATIONS_H

#include "llvm/CodeGen/MachineRelocation.h"

namespace llvm {
  namespace SP {
    enum RelocationType {
      // reloc_cse523_hi - upper 22 bits
      reloc_cse523_hi = 1,

      // reloc_cse523_lo - lower 10 bits
      reloc_cse523_lo = 2,

      // reloc_cse523_pc30 - pc rel. 30 bits for call
      reloc_cse523_pc30 = 3,

     // reloc_cse523_pc22 - pc rel. 22 bits for branch
      reloc_cse523_pc22 = 4,

      // reloc_cse523_pc22 - pc rel. 19 bits for branch with icc/xcc
      reloc_cse523_pc19 = 5,

      // reloc_cse523_h44 - 43-22 bits
      reloc_cse523_h44 = 6,

      // reloc_cse523_m44 - 21-12 bits
      reloc_cse523_m44 = 7,

      // reloc_cse523_l44 - lower 12 bits
      reloc_cse523_l44 = 8,

      // reloc_cse523_hh - 63-42 bits
      reloc_cse523_hh  = 9,

      // reloc_cse523_hm - 41-32 bits
      reloc_cse523_hm  = 10
    };
  }
}

#endif
