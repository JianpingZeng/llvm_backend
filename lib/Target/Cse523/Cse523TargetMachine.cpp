//===-- Cse523TargetMachine.cpp - Define TargetMachine for Cse523 -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "Cse523TargetMachine.h"
#include "Cse523.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeCse523Target() {
    // Register the target.
    RegisterTargetMachine<Cse523TargetMachine> X(TheCse523Target);
}

static std::string computeDataLayout(const Cse523Subtarget &ST) {
    // Cse523 is little endian.
    std::string Ret = "e";

    // Some ABIs have 32bit pointers.
    if (!ST.is64Bit())
        Ret += "-p:32:32";

    // Alignments for 64 bit integers.
    Ret += "-i64:64";

    // On Cse523 128 floats are aligned to 128 bits, on others only to 64.
    // On Cse523 registers can hold 64 or 32 bits, on others only 32.
    if (ST.is64Bit())
        Ret += "-n32:64";
    else
        Ret += "-f128:64-n32";

    if (ST.is64Bit())
        Ret += "-S128";
    else
        Ret += "-S64";

    return Ret;
}

/// Cse523TargetMachine ctor - Create an ILP32 architecture model
///
Cse523TargetMachine::Cse523TargetMachine(const Target &T, StringRef TT,
        StringRef CPU, StringRef FS,
        const TargetOptions &Options,
        Reloc::Model RM, CodeModel::Model CM,
        CodeGenOpt::Level OL)
    : LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
    Subtarget(TT, CPU, FS, true),
    DL(computeDataLayout(Subtarget)),
    InstrInfo(*this)
    //FrameLowering(*this, Subtarget),
    //InstrItins(Subtarget.getInstrItineraryData()),
    //TLInfo(*this),
    //TSInfo(*this),
    //JITInfo(*this)
{
    initAsmInfo();
}

namespace {
    /// Cse523 Code Generator Pass Configuration Options.
    class Cse523PassConfig : public TargetPassConfig {
        public:
            Cse523PassConfig(Cse523TargetMachine *TM, PassManagerBase &PM)
                : TargetPassConfig(TM, PM) {}

            Cse523TargetMachine &getCse523TargetMachine() const {
                return getTM<Cse523TargetMachine>();
            }

            virtual bool addInstSelector();
            virtual bool addPreEmitPass();
    };
} // namespace

TargetPassConfig *Cse523TargetMachine::createPassConfig(PassManagerBase &PM) {
    return new Cse523PassConfig(this, PM);
}

bool Cse523PassConfig::addInstSelector() {
    //addPass(createCse523ISelDag(getCse523TargetMachine()));
    return false;
}

bool Cse523TargetMachine::addCodeEmitter(PassManagerBase &PM,
        JITCodeEmitter &JCE) {
    // Machine code emitter pass for Cse523.
    //PM.add(createCse523JITCodeEmitterPass(*this, JCE));
    return false;
}

/// addPreEmitPass - This pass may be implemented by targets that want to run
/// passes immediately before machine code is emitted.  This should return
/// true if -print-machineinstrs should print out the code after the passes.
bool Cse523PassConfig::addPreEmitPass(){
    //addPass(createCse523DelaySlotFillerPass(getCse523TargetMachine()));
    return true;
}

