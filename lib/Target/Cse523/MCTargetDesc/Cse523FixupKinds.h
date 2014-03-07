//===-- Cse523FixupKinds.h - Cse523 Specific Fixup Entries --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CSE523_FIXUPKINDS_H
#define LLVM_CSE523_FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
  namespace Cse523 {
    enum Fixups {
      // fixup_cse523_call30 - 30-bit PC relative relocation for call
      fixup_cse523_call30 = FirstTargetFixupKind,

      /// fixup_cse523_br22 - 22-bit PC relative relocation for
      /// branches
      fixup_cse523_br22,

      /// fixup_cse523_br19 - 19-bit PC relative relocation for
      /// branches on icc/xcc
      fixup_cse523_br19,

      /// fixup_cse523_hi22  - 22-bit fixup corresponding to %hi(foo)
      /// for sethi
      fixup_cse523_hi22,

      /// fixup_cse523_lo10  - 10-bit fixup corresponding to %lo(foo)
      fixup_cse523_lo10,

      /// fixup_cse523_h44  - 22-bit fixup corresponding to %h44(foo)
      fixup_cse523_h44,

      /// fixup_cse523_m44  - 10-bit fixup corresponding to %m44(foo)
      fixup_cse523_m44,

      /// fixup_cse523_l44  - 12-bit fixup corresponding to %l44(foo)
      fixup_cse523_l44,

      /// fixup_cse523_hh  -  22-bit fixup corresponding to %hh(foo)
      fixup_cse523_hh,

      /// fixup_cse523_hm  -  10-bit fixup corresponding to %hm(foo)
      fixup_cse523_hm,

      /// fixup_cse523_pc22 - 22-bit fixup corresponding to %pc22(foo)
      fixup_cse523_pc22,

      /// fixup_cse523_pc10 - 10-bit fixup corresponding to %pc10(foo)
      fixup_cse523_pc10,

      /// fixup_cse523_got22 - 22-bit fixup corresponding to %got22(foo)
      fixup_cse523_got22,

      /// fixup_cse523_got10 - 10-bit fixup corresponding to %got10(foo)
      fixup_cse523_got10,

      /// fixup_cse523_wplt30
      fixup_cse523_wplt30,

      /// fixups for Thread Local Storage
      fixup_cse523_tls_gd_hi22,
      fixup_cse523_tls_gd_lo10,
      fixup_cse523_tls_gd_add,
      fixup_cse523_tls_gd_call,
      fixup_cse523_tls_ldm_hi22,
      fixup_cse523_tls_ldm_lo10,
      fixup_cse523_tls_ldm_add,
      fixup_cse523_tls_ldm_call,
      fixup_cse523_tls_ldo_hix22,
      fixup_cse523_tls_ldo_lox10,
      fixup_cse523_tls_ldo_add,
      fixup_cse523_tls_ie_hi22,
      fixup_cse523_tls_ie_lo10,
      fixup_cse523_tls_ie_ld,
      fixup_cse523_tls_ie_ldx,
      fixup_cse523_tls_ie_add,
      fixup_cse523_tls_le_hix22,
      fixup_cse523_tls_le_lox10,

      // Marker
      LastTargetFixupKind,
      NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
    };
  }
}

#endif
