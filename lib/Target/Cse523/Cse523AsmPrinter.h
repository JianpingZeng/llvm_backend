//===-- Cse523AsmPrinter.h - Cse523 implementation of AsmPrinter ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CSE523ASMPRINTER_H
#define CSE523ASMPRINTER_H

#include "Cse523.h"
#include "Cse523MachineFunctionInfo.h"
#include "Cse523TargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/StackMaps.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/Support/Compiler.h"

namespace llvm {

class MCStreamer;

class LLVM_LIBRARY_VISIBILITY Cse523AsmPrinter : public AsmPrinter {
  const Cse523Subtarget *Subtarget;
  StackMaps SM;

 public:
  explicit Cse523AsmPrinter(TargetMachine &TM, MCStreamer &Streamer)
    : AsmPrinter(TM, Streamer), SM(*this) {
    Subtarget = &TM.getSubtarget<Cse523Subtarget>();
  }

  virtual const char *getPassName() const LLVM_OVERRIDE {
    return "Cse523 Assembly / Object Emitter";
  }

  const Cse523Subtarget &getSubtarget() const { return *Subtarget; }

  virtual void EmitStartOfAsmFile(Module &M) LLVM_OVERRIDE;

  virtual void EmitEndOfAsmFile(Module &M) LLVM_OVERRIDE;

  virtual void EmitInstruction(const MachineInstr *MI) LLVM_OVERRIDE;

  virtual bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                               unsigned AsmVariant, const char *ExtraCode,
                               raw_ostream &OS) LLVM_OVERRIDE;
  virtual bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo,
                                     unsigned AsmVariant, const char *ExtraCode,
                                     raw_ostream &OS) LLVM_OVERRIDE;

  virtual bool runOnMachineFunction(MachineFunction &F) LLVM_OVERRIDE;
};

} // end namespace llvm

#endif
