//===-- Cse523TargetInfo.cpp - Cse523 Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Cse523.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheCse523Target;

extern "C" void LLVMInitializeCse523TargetInfo() {
  RegisterTarget<Triple::cse523, /*HasJIT=*/ false>
    X(TheCse523Target, "cse523", "Cse523");
}

