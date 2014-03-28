//===-- Cse523.h - Top-level interface for Cse523 representation ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the x86
// target library, as used by the LLVM JIT.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_CSE523_H
#define TARGET_CSE523_H

#include "MCTargetDesc/Cse523BaseInfo.h"
#include "MCTargetDesc/Cse523MCTargetDesc.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class FunctionPass;
class JITCodeEmitter;
class Cse523TargetMachine;
//
/// createCse523ISelDag - This pass converts a legalized DAG into a
/// Cse523-specific DAG, ready for instruction scheduling.
///
FunctionPass *createCse523ISelDag(Cse523TargetMachine &TM,
                               CodeGenOpt::Level OptLevel);

/// createGlobalBaseRegPass - This pass initializes a global base
/// register for PIC on x86-32.
FunctionPass* createGlobalBaseRegPass();

/// createCleanupLocalDynamicTLSPass() - This pass combines multiple accesses
/// to local-dynamic TLS variables so that the TLS base address for the module
/// is only fetched once per execution path through the function.
FunctionPass *createCleanupLocalDynamicTLSPass();

/// createCse523FloatingPointStackifierPass - This function returns a pass which
/// converts floating point register references and pseudo instructions into
/// floating point stack references and physical instructions.
///
FunctionPass *createCse523FloatingPointStackifierPass();

/// createCse523IssueVZeroUpperPass - This pass inserts AVX vzeroupper instructions
/// before each call to avoid transition penalty between functions encoded with
/// AVX and SSE.
FunctionPass *createCse523IssueVZeroUpperPass();

/// createCse523CodeEmitterPass - Return a pass that emits the collected Cse523 code
/// to the specified MCE object.
FunctionPass *createCse523JITCodeEmitterPass(Cse523TargetMachine &TM,
                                          JITCodeEmitter &JCE);

/// createCse523EmitCodeToMemory - Returns a pass that converts a register
/// allocated function into raw machine code in a dynamically
/// allocated chunk of memory.
///
FunctionPass *createEmitCse523CodeToMemory();

/// \brief Creates an Cse523-specific Target Transformation Info pass.
ImmutablePass *createCse523TargetTransformInfoPass(const Cse523TargetMachine *TM);

/// createCse523PadShortFunctions - Return a pass that pads short functions
/// with NOOPs. This will prevent a stall when returning on the Atom.
FunctionPass *createCse523PadShortFunctions();
/// createCse523FixupLEAs - Return a a pass that selectively replaces
/// certain instructions (like add, sub, inc, dec, some shifts,
/// and some multiplies) by equivalent LEA instructions, in order
/// to eliminate execution delays in some Atom processors.
FunctionPass *createCse523FixupLEAs();

} // End llvm namespace

#endif
