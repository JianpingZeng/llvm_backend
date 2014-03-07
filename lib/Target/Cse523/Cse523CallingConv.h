//=== Cse523CallingConv.h - Cse523 Custom Calling Convention Routines -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the custom routines for the Cse523 Calling Convention that
// aren't done by tablegen.
//
//===----------------------------------------------------------------------===//

#ifndef Cse523CALLINGCONV_H
#define Cse523CALLINGCONV_H

#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/IR/CallingConv.h"

namespace llvm {

inline bool CC_Cse523_CDeclMethod_SRet(unsigned &ValNo, MVT &ValVT, MVT &LocVT,
                                    CCValAssign::LocInfo &LocInfo,
                                    ISD::ArgFlagsTy &ArgFlags, CCState &State) {

  llvm_unreachable("Inside CC_Cse523_CDeclMethod_SRet()");

  // Swap the order of the first two parameters if the first parameter is sret.
  if (ArgFlags.isSRet()) {
    assert(ValNo == 0);
    assert(ValVT == MVT::i32);
    State.AllocateStack(8, 4);
    State.addLoc(CCValAssign::getCustomMem(ValNo, ValVT, 4, LocVT, LocInfo));

    // Indicate that we need to swap the order of the first and second
    // parameters by "allocating" register zero.  There are no register
    // parameters with cdecl methods, so we can use this to communicate to the
    // next call.
    State.AllocateReg(1);
    return true;
  } else if (ValNo == 1 && State.isAllocated(1)) {
    assert(ValVT == MVT::i32 && "non-i32-sized this param unsupported");
    // Stack was already allocated while processing sret.
    State.addLoc(CCValAssign::getCustomMem(ValNo, ValVT, 0, LocVT, LocInfo));
    return true;
  }

  // All other args use the C calling convention.
  return false;
}

} // End llvm namespace

#endif

