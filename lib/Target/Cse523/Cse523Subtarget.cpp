//===-- Cse523Subtarget.cpp - Cse523 Subtarget Information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Cse523 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "subtarget"
#include "Cse523Subtarget.h"
#include "Cse523InstrInfo.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "Cse523GenSubtargetInfo.inc"

using namespace llvm;

#if defined(_MSC_VER)
#include <intrin.h>
#endif

/// ClassifyBlockAddressReference - Classify a blockaddress reference for the
/// current subtarget according to how we should reference it in a non-pcrel
/// context.
unsigned char Cse523Subtarget::ClassifyBlockAddressReference() const {
    if (isPICStyleGOT())    // 32-bit ELF targets.
        return Cse523II::MO_GOTOFF;

    if (isPICStyleStubPIC())   // Darwin/32 in PIC mode.
        return Cse523II::MO_PIC_BASE_OFFSET;

    // Direct static reference to label.
    return Cse523II::MO_NO_FLAG;
}

/// ClassifyGlobalReference - Classify a global variable reference for the
/// current subtarget according to how we should reference it in a non-pcrel
/// context.
unsigned char Cse523Subtarget::
ClassifyGlobalReference(const GlobalValue *GV, const TargetMachine &TM) const {
    // DLLImport only exists on windows, it is implemented as a load from a
    // DLLIMPORT stub.
    if (GV->hasDLLImportStorageClass())
        return Cse523II::MO_DLLIMPORT;

    // Determine whether this is a reference to a definition or a declaration.
    // Materializable GVs (in JIT lazy compilation mode) do not require an extra
    // load from stub.
    bool isDecl = GV->hasAvailableExternallyLinkage();
    if (GV->isDeclaration() && !GV->isMaterializable())
        isDecl = true;

    // Cse523-64 in PIC mode.
    if (isPICStyleRIPRel()) {
        // Large model never uses stubs.
        if (TM.getCodeModel() == CodeModel::Large)
            return Cse523II::MO_NO_FLAG;

        if (isTargetDarwin()) {
            // If symbol visibility is hidden, the extra load is not needed if
            // target is cse523-64 or the symbol is definitely defined in the current
            // translation unit.
            if (GV->hasDefaultVisibility() &&
                    (isDecl || GV->isWeakForLinker()))
                return Cse523II::MO_GOTPCREL;
        } else if (!isTargetWin64()) {
            assert(isTargetELF() && "Unknown rip-relative target");

            // Extra load is needed for all externally visible.
            if (!GV->hasLocalLinkage() && GV->hasDefaultVisibility())
                return Cse523II::MO_GOTPCREL;
        }

        return Cse523II::MO_NO_FLAG;
    }

    if (isPICStyleGOT()) {   // 32-bit ELF targets.
        // Extra load is needed for all externally visible.
        if (GV->hasLocalLinkage() || GV->hasHiddenVisibility())
            return Cse523II::MO_GOTOFF;
        return Cse523II::MO_GOT;
    }

    if (isPICStyleStubPIC()) {  // Darwin/32 in PIC mode.
        // Determine whether we have a stub reference and/or whether the reference
        // is relative to the PIC base or not.

        // If this is a strong reference to a definition, it is definitely not
        // through a stub.
        if (!isDecl && !GV->isWeakForLinker())
            return Cse523II::MO_PIC_BASE_OFFSET;

        // Unless we have a symbol with hidden visibility, we have to go through a
        // normal $non_lazy_ptr stub because this symbol might be resolved late.
        if (!GV->hasHiddenVisibility())  // Non-hidden $non_lazy_ptr reference.
            return Cse523II::MO_DARWIN_NONLAZY_PIC_BASE;

        // If symbol visibility is hidden, we have a stub for common symbol
        // references and external declarations.
        if (isDecl || GV->hasCommonLinkage()) {
            // Hidden $non_lazy_ptr reference.
            return Cse523II::MO_DARWIN_HIDDEN_NONLAZY_PIC_BASE;
        }

        // Otherwise, no stub.
        return Cse523II::MO_PIC_BASE_OFFSET;
    }

    if (isPICStyleStubNoDynamic()) {  // Darwin/32 in -mdynamic-no-pic mode.
        // Determine whether we have a stub reference.

        // If this is a strong reference to a definition, it is definitely not
        // through a stub.
        if (!isDecl && !GV->isWeakForLinker())
            return Cse523II::MO_NO_FLAG;

        // Unless we have a symbol with hidden visibility, we have to go through a
        // normal $non_lazy_ptr stub because this symbol might be resolved late.
        if (!GV->hasHiddenVisibility())  // Non-hidden $non_lazy_ptr reference.
            return Cse523II::MO_DARWIN_NONLAZY;

        // Otherwise, no stub.
        return Cse523II::MO_NO_FLAG;
    }

    // Direct static reference to global.
    return Cse523II::MO_NO_FLAG;
}


/// getBZeroEntry - This function returns the name of a function which has an
/// interface like the non-standard bzero function, if such a function exists on
/// the current subtarget and it is considered prefereable over memset with zero
/// passed as the second argument. Otherwise it returns null.
const char *Cse523Subtarget::getBZeroEntry() const {
    // Darwin 10 has a __bzero entry point for this purpose.
    if (getTargetTriple().isMacOSX() &&
            !getTargetTriple().isMacOSXVersionLT(10, 6))
        return "__bzero";

    return 0;
}

bool Cse523Subtarget::hasSinCos() const {
    return getTargetTriple().isMacOSX() &&
        !getTargetTriple().isMacOSXVersionLT(10, 9) &&
        is64Bit();
}

/// IsLegalToCallImmediateAddr - Return true if the subtarget allows calls
/// to immediate address.
bool Cse523Subtarget::IsLegalToCallImmediateAddr(const TargetMachine &TM) const {
    return false;
}

static bool OSHasAVXSupport() {
    return false;
}

void Cse523Subtarget::AutoDetectSubtargetFeatures() {
    ToggleFeature(Cse523::FeatureCSE523);
}

void Cse523Subtarget::resetSubtargetFeatures(const MachineFunction *MF) {
    AttributeSet FnAttrs = MF->getFunction()->getAttributes();
    Attribute CPUAttr = FnAttrs.getAttribute(AttributeSet::FunctionIndex,
            "target-cpu");
    Attribute FSAttr = FnAttrs.getAttribute(AttributeSet::FunctionIndex,
            "target-features");
    std::string CPU =
        !CPUAttr.hasAttribute(Attribute::None) ?CPUAttr.getValueAsString() : "";
    std::string FS =
        !FSAttr.hasAttribute(Attribute::None) ? FSAttr.getValueAsString() : "";
    if (!FS.empty()) {
        initializeEnvironment();
        resetSubtargetFeatures(CPU, FS);
    }
}

void Cse523Subtarget::resetSubtargetFeatures(StringRef CPU, StringRef FS) {
    std::string CPUName = CPU;
    if (!FS.empty() || !CPU.empty()) {

        // If feature string is not empty, parse features string.
        //ParseSubtargetFeatures(CPUName, FullFS);
    } else {
        // Otherwise, use CPUID to auto-detect feature set.
        AutoDetectSubtargetFeatures();
    }

    // CPUName may have been set by the CPU detection code. Make sure the
    // new MCSchedModel is used.
    //InitCPUSchedModel(CPUName);

    //InstrItins = getInstrItineraryForCPU(CPUName);

    // It's important to keep the MCSubtargetInfo feature bits in sync with
    // target data structure which is shared with MC code emitter, etc.
    if (!In64BitMode)
        llvm_unreachable("Not 64-bit mode!");

    DEBUG(dbgs() << "Subtarget features: SSELevel " << Cse523SSELevel
            << ", 3DNowLevel " << Cse5233DNowLevel
            << ", 64bit " << IsCSE523 << "\n");
    assert((In64BitMode && IsCSE523) &&
            "64-bit code requested on a subtarget that doesn't support it!");

    // Stack alignment is 16 bytes on Darwin, Linux and Solaris (both
    // 32 and 64 bit) and for all 64-bit targets.
    if (StackAlignOverride)
        stackAlignment = StackAlignOverride;
    else if (In64BitMode)
        stackAlignment = 16;
}

void Cse523Subtarget::initializeEnvironment() {
    Cse523SSELevel = NoMMXSSE;
    Cse5233DNowLevel = NoThreeDNow;
    HasCMov = false;
    IsCSE523 = true;
    HasPOPCNT = false;
    HasSSE4A = false;
    HasAES = false;
    HasPCLMUL = false;
    HasFMA = false;
    HasFMA4 = false;
    HasXOP = false;
    HasTBM = false;
    HasMOVBE = false;
    HasRDRAND = false;
    HasF16C = false;
    HasFSGSBase = false;
    HasLZCNT = false;
    HasBMI = false;
    HasBMI2 = false;
    HasRTM = false;
    HasHLE = false;
    HasERI = false;
    HasCDI = false;
    HasPFI = false;
    HasADX = false;
    HasSHA = false;
    HasPRFCHW = false;
    HasRDSEED = false;
    IsBTMemSlow = false;
    IsSHLDSlow = false;
    IsUAMemFast = false;
    HasVectorUAMem = false;
    HasCmpxchg16b = false;
    UseLeaForSP = false;
    HasSlowDivide = false;
    PostRAScheduler = false;
    PadShortFunctions = false;
    CallRegIndirect = false;
    LEAUsesAG = false;
    stackAlignment = 4;
    // FIXME: this is a known good value for Yonah. How about others?
    MaxInlineSizeThreshold = 128;
}

Cse523Subtarget::Cse523Subtarget(const std::string &TT, const std::string &CPU,
        const std::string &FS,
        unsigned StackAlignOverride)
    : Cse523GenSubtargetInfo(TT, CPU, FS)
    , Cse523ProcFamily(Others)
    , PICStyle(PICStyles::None)
    , TargetTriple(TT)
    , StackAlignOverride(StackAlignOverride)
    , In64BitMode(TargetTriple.getArch() == Triple::cse523)
    , In32BitMode(false)
    , In16BitMode(false) 
{
    initializeEnvironment();
    resetSubtargetFeatures(CPU, FS);
}

bool Cse523Subtarget::enablePostRAScheduler(
        CodeGenOpt::Level OptLevel,
        TargetSubtargetInfo::AntiDepBreakMode& Mode,
        RegClassVector& CriticalPathRCs) const {
    Mode = TargetSubtargetInfo::ANTIDEP_CRITICAL;
    CriticalPathRCs.clear();
    return PostRAScheduler && OptLevel >= CodeGenOpt::Default;
}

