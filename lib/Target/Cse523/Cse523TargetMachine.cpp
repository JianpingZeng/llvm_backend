//===-- Cse523TargetMachine.cpp - Define TargetMachine for the Cse523 -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the Cse523 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#include "Cse523TargetMachine.h"
#include "Cse523.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/PassManager.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
using namespace llvm;

extern "C" void LLVMInitializeCse523Target() {
    // Register the target.
    RegisterTargetMachine<Cse523TargetMachine> X(TheCse523Target);
}

void Cse523TargetMachine::anchor() { }

static std::string computeDataLayout() {
    // Cse523 is little endian
    std::string Ret = "e";

    Ret += "-m:e";   // DataLayout::getManglingComponent(ST.getTargetTriple());

    // Cse523 and x32 have 32 bit pointers.
    //Ret += "-p:32:32";

    // Some ABIs align 64 bit integers and doubles to 64 bits, others to 32.
    Ret += "-i64:64";

    // Some ABIs align long double to 128 bits, others to 32.
    // No f80

    // The registers can hold 64 in cse523-64, 64 bits.
    Ret += "-n64";

    // The stack is aligned to 32 bits on some ABIs and 128 bits on others.
    Ret += "-S128";

    return Ret;
}

/// Cse523TargetMachine ctor - Create an Cse523 target.
///
Cse523TargetMachine::Cse523TargetMachine(const Target &T, StringRef TT,
        StringRef CPU, StringRef FS,
        const TargetOptions &Options,
        Reloc::Model RM, CodeModel::Model CM,
        CodeGenOpt::Level OL)
    : LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
    Subtarget(TT, CPU, FS, Options.StackAlignmentOverride),
    //FrameLowering(*this, Subtarget),
    InstrItins(), //InstrItins(Subtarget.getInstrItineraryData()),
    DL(computeDataLayout()),
    InstrInfo(*this),
    TLInfo(*this),
    TSInfo(*this)
    //JITInfo(*this) 
{
    // Determine the PICStyle based on the target selected.
    if (getRelocationModel() == Reloc::Static) {
        // Unless we're in PIC or DynamicNoPIC mode, set the PIC style to None.
        Subtarget.setPICStyle(PICStyles::None);
    } else if (Subtarget.is64Bit()) {
        // PIC in 64 bit mode is always rip-rel.
        Subtarget.setPICStyle(PICStyles::RIPRel);
    } else if (Subtarget.isTargetCOFF()) {
        Subtarget.setPICStyle(PICStyles::None);
    } else if (Subtarget.isTargetDarwin()) {
        if (getRelocationModel() == Reloc::PIC_)
            Subtarget.setPICStyle(PICStyles::StubPIC);
        else {
            assert(getRelocationModel() == Reloc::DynamicNoPIC);
            Subtarget.setPICStyle(PICStyles::StubDynamicNoPIC);
        }
    } else if (Subtarget.isTargetELF()) {
        Subtarget.setPICStyle(PICStyles::GOT);
    }

    // default to hard float ABI
    if (Options.FloatABIType == FloatABI::Default)
        this->Options.FloatABIType = FloatABI::Hard;

    initAsmInfo();
}

//===----------------------------------------------------------------------===//
// Command line options for cse523
//===----------------------------------------------------------------------===//
static cl::opt<bool>
UseVZeroUpper("cse523-use-vzeroupper", cl::Hidden,
        cl::desc("Minimize AVX to SSE transition penalty"),
        cl::init(true));

// Temporary option to control early if-conversion for cse523 while adding machine
// models.
static cl::opt<bool>
Cse523EarlyIfConv("cse523-early-ifcvt", cl::Hidden,
        cl::desc("Enable early if-conversion on Cse523"));

//===----------------------------------------------------------------------===//
// Cse523 Analysis Pass Setup
//===----------------------------------------------------------------------===//

void Cse523TargetMachine::addAnalysisPasses(PassManagerBase &PM) {
    // Add first the target-independent BasicTTI pass, then our Cse523 pass. This
    // allows the Cse523 pass to delegate to the target independent layer when
    // appropriate.
    PM.add(createBasicTargetTransformInfoPass(this));
    PM.add(createCse523TargetTransformInfoPass(this));
}


//===----------------------------------------------------------------------===//
// Pass Pipeline Configuration
//===----------------------------------------------------------------------===//

namespace {
    /// Cse523 Code Generator Pass Configuration Options.
    class Cse523PassConfig : public TargetPassConfig {
        public:
            Cse523PassConfig(Cse523TargetMachine *TM, PassManagerBase &PM)
                : TargetPassConfig(TM, PM) {}

            Cse523TargetMachine &getCse523TargetMachine() const {
                return getTM<Cse523TargetMachine>();
            }

            const Cse523Subtarget &getCse523Subtarget() const {
                return *getCse523TargetMachine().getSubtargetImpl();
            }

            virtual bool addInstSelector();
            virtual bool addILPOpts();
            virtual bool addPreRegAlloc();
            virtual bool addPostRegAlloc();
            virtual bool addPreEmitPass();
    };
} // namespace

TargetPassConfig *Cse523TargetMachine::createPassConfig(PassManagerBase &PM) {
    return new Cse523PassConfig(this, PM);
}

bool Cse523PassConfig::addInstSelector() {
    // Install an instruction selector.
    addPass(createCse523ISelDag(getCse523TargetMachine(), getOptLevel()));
 
    // For ELF, cleanup any local-dynamic TLS accesses.
    if (getCse523Subtarget().isTargetELF() && getOptLevel() != CodeGenOpt::None)
        addPass(createCleanupLocalDynamicTLSPass());

    // For 32-bit, prepend instructions to set the "global base reg" for PIC.
    if (!getCse523Subtarget().is64Bit())
        addPass(createGlobalBaseRegPass());

    return false;
}

bool Cse523PassConfig::addILPOpts() {
    if (Cse523EarlyIfConv && getCse523Subtarget().hasCMov()) {
        addPass(&EarlyIfConverterID);
        return true;
    }
    return false;
}

bool Cse523PassConfig::addPreRegAlloc() {
    return false;  // -print-machineinstr shouldn't print after this.
}

bool Cse523PassConfig::addPostRegAlloc() {
    //addPass(createCse523FloatingPointStackifierPass());
    return true;  // -print-machineinstr should print after this.
}

bool Cse523PassConfig::addPreEmitPass() {
    bool ShouldPrint = false;
    if (getOptLevel() != CodeGenOpt::None && getCse523Subtarget().hasSSE2()) {
        //addPass(createExecutionDependencyFixPass(&Cse523::VR128RegClass));
        ShouldPrint = true;
    }

    if (getCse523Subtarget().hasAVX() && UseVZeroUpper) {
        //addPass(createCse523IssueVZeroUpperPass());
        ShouldPrint = true;
    }

    if (getOptLevel() != CodeGenOpt::None && getCse523Subtarget().padShortFunctions()) {
        //addPass(createCse523PadShortFunctions());
        ShouldPrint = true;
    }

    if (getOptLevel() != CodeGenOpt::None &&
            getCse523Subtarget().LEAusesAG()){
        //addPass(createCse523FixupLEAs());
        ShouldPrint = true;
    }

    return ShouldPrint;
}

bool Cse523TargetMachine::addCodeEmitter(PassManagerBase &PM,
        JITCodeEmitter &JCE) {
    //PM.add(createCse523JITCodeEmitterPass(*this, JCE));

    return false;
}

