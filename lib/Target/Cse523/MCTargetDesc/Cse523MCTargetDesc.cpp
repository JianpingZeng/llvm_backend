//===-- Cse523MCTargetDesc.cpp - Cse523 Target Descriptions ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Cse523 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "Cse523MCTargetDesc.h"
#include "InstPrinter/Cse523ATTInstPrinter.h"
#include "InstPrinter/Cse523IntelInstPrinter.h"
#include "Cse523MCAsmInfo.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_REGINFO_MC_DESC
#include "Cse523GenRegisterInfo.inc"

//#define GET_INSTRINFO_MC_DESC
//#include "Cse523GenInstrInfo.inc"
//
//#define GET_SUBTARGETINFO_MC_DESC
//#include "Cse523GenSubtargetInfo.inc"

#if _MSC_VER
#include <intrin.h>
#endif

using namespace llvm;


std::string Cse523_MC::ParseCse523Triple(StringRef TT) {
  Triple TheTriple(TT);
  std::string FS;
  if (TheTriple.getArch() == Triple::cse523)
    FS = "+64bit-mode,-32bit-mode,-16bit-mode";

  return FS;
}

/// GetCpuIDAndInfo - Execute the specified cpuid and return the 4 values in the
/// specified arguments.  If we can't run cpuid on the host, return true.
bool Cse523_MC::GetCpuIDAndInfo(unsigned value, unsigned *rEAX,
                             unsigned *rEBX, unsigned *rECX, unsigned *rEDX) {
#if defined(__Cse523__) || defined(_M_AMD64) || defined (_M_X64)
  #if defined(__GNUC__)
    // gcc doesn't know cpuid would clobber ebx/rbx. Preseve it manually.
    asm ("movq\t%%rbx, %%rsi\n\t"
         "cpuid\n\t"
         "xchgq\t%%rbx, %%rsi\n\t"
         : "=a" (*rEAX),
           "=S" (*rEBX),
           "=c" (*rECX),
           "=d" (*rEDX)
         :  "a" (value));
    return false;
  #elif defined(_MSC_VER)
    int registers[4];
    __cpuid(registers, value);
    *rEAX = registers[0];
    *rEBX = registers[1];
    *rECX = registers[2];
    *rEDX = registers[3];
    return false;
  #else
    return true;
  #endif
#elif defined(i386) || defined(__i386__) || defined(__Cse523__) || defined(_M_ICse523)
  #if defined(__GNUC__)
    asm ("movl\t%%ebx, %%esi\n\t"
         "cpuid\n\t"
         "xchgl\t%%ebx, %%esi\n\t"
         : "=a" (*rEAX),
           "=S" (*rEBX),
           "=c" (*rECX),
           "=d" (*rEDX)
         :  "a" (value));
    return false;
  #elif defined(_MSC_VER)
    __asm {
      mov   eax,value
      cpuid
      mov   esi,rEAX
      mov   dword ptr [esi],eax
      mov   esi,rEBX
      mov   dword ptr [esi],ebx
      mov   esi,rECX
      mov   dword ptr [esi],ecx
      mov   esi,rEDX
      mov   dword ptr [esi],edx
    }
    return false;
  #else
    return true;
  #endif
#else
  return true;
#endif
}

/// GetCpuIDAndInfoEx - Execute the specified cpuid with subleaf and return the
/// 4 values in the specified arguments.  If we can't run cpuid on the host,
/// return true.
bool Cse523_MC::GetCpuIDAndInfoEx(unsigned value, unsigned subleaf, unsigned *rEAX,
                               unsigned *rEBX, unsigned *rECX, unsigned *rEDX) {
#if defined(__Cse523__) || defined(_M_AMD64) || defined (_M_X64)
  #if defined(__GNUC__)
    // gcc desn't know cpuid would clobber ebx/rbx. Preseve it manually.
    asm ("movq\t%%rbx, %%rsi\n\t"
         "cpuid\n\t"
         "xchgq\t%%rbx, %%rsi\n\t"
         : "=a" (*rEAX),
           "=S" (*rEBX),
           "=c" (*rECX),
           "=d" (*rEDX)
         :  "a" (value),
            "c" (subleaf));
    return false;
  #elif defined(_MSC_VER)
    // __cpuidex was added in MSVC++ 9.0 SP1
    #if (_MSC_VER > 1500) || (_MSC_VER == 1500 && _MSC_FULL_VER >= 150030729)
      int registers[4];
      __cpuidex(registers, value, subleaf);
      *rEAX = registers[0];
      *rEBX = registers[1];
      *rECX = registers[2];
      *rEDX = registers[3];
      return false;
    #else
      return true;
    #endif
  #else
    return true;
  #endif
#elif defined(i386) || defined(__i386__) || defined(__Cse523__) || defined(_M_ICse523)
  #if defined(__GNUC__)
    asm ("movl\t%%ebx, %%esi\n\t"
         "cpuid\n\t"
         "xchgl\t%%ebx, %%esi\n\t"
         : "=a" (*rEAX),
           "=S" (*rEBX),
           "=c" (*rECX),
           "=d" (*rEDX)
         :  "a" (value),
            "c" (subleaf));
    return false;
  #elif defined(_MSC_VER)
    __asm {
      mov   eax,value
      mov   ecx,subleaf
      cpuid
      mov   esi,rEAX
      mov   dword ptr [esi],eax
      mov   esi,rEBX
      mov   dword ptr [esi],ebx
      mov   esi,rECX
      mov   dword ptr [esi],ecx
      mov   esi,rEDX
      mov   dword ptr [esi],edx
    }
    return false;
  #else
    return true;
  #endif
#else
  return true;
#endif
}

void Cse523_MC::DetectFamilyModel(unsigned EAX, unsigned &Family,
                               unsigned &Model) {
  Family = (EAX >> 8) & 0xf; // Bits 8 - 11
  Model  = (EAX >> 4) & 0xf; // Bits 4 - 7
  if (Family == 6 || Family == 0xf) {
    if (Family == 0xf)
      // Examine extended family ID if family ID is F.
      Family += (EAX >> 20) & 0xff;    // Bits 20 - 27
    // Examine extended model ID if family ID is 6 or F.
    Model += ((EAX >> 16) & 0xf) << 4; // Bits 16 - 19
  }
}

unsigned Cse523_MC::getDwarfRegFlavour(StringRef TT, bool isEH) {
  Triple TheTriple(TT);
  return DWARFFlavour::Cse523;

}

void Cse523_MC::InitLLVM2SEHRegisterMapping(MCRegisterInfo *MRI) {
  // FIXME: TableGen these.
  for (unsigned Reg = Cse523::NoRegister+1; Reg < Cse523::NUM_TARGET_REGS; ++Reg) {
    unsigned SEH = MRI->getEncodingValue(Reg);
    MRI->mapLLVMRegToSEHReg(Reg, SEH);
  }
}

MCSubtargetInfo *Cse523_MC::createCse523MCSubtargetInfo(StringRef TT, StringRef CPU,
                                                  StringRef FS) {
  std::string ArchFS = Cse523_MC::ParseCse523Triple(TT);
  if (!FS.empty()) {
    if (!ArchFS.empty())
      ArchFS = ArchFS + "," + FS.str();
    else
      ArchFS = FS;
  }

  std::string CPUName = CPU;
  if (CPUName.empty()) {
#if defined(i386) || defined(__i386__) || defined(__Cse523__) || defined(_M_ICse523)\
    || defined(__Cse523__) || defined(_M_AMD64) || defined (_M_X64)
    CPUName = sys::getHostCPUName();
#else
    CPUName = "generic";
#endif
  }

  MCSubtargetInfo *X = new MCSubtargetInfo();
  //InitCse523MCSubtargetInfo(X, TT, CPUName, ArchFS);
  return X;
}

static MCInstrInfo *createCse523MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  //InitCse523MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createCse523MCRegisterInfo(StringRef TT) {
  Triple TheTriple(TT);
  unsigned RA = Cse523::RIP;     // Should have dwarf #16.

  MCRegisterInfo *X = new MCRegisterInfo();
  InitCse523MCRegisterInfo(X, RA,
                        Cse523_MC::getDwarfRegFlavour(TT, false),
                        Cse523_MC::getDwarfRegFlavour(TT, true),
                        RA);
  Cse523_MC::InitLLVM2SEHRegisterMapping(X);
  return X;
}

static MCAsmInfo *createCse523MCAsmInfo(const MCRegisterInfo &MRI, StringRef TT) {
  Triple TheTriple(TT);

  MCAsmInfo *MAI;
  if (TheTriple.isOSBinFormatMachO()) {
    MAI = new Cse523_64MCAsmInfoDarwin(TheTriple);
  } else if (TheTriple.getEnvironment() == Triple::ELF) {
    // Force the use of an ELF container.
    MAI = new Cse523ELFMCAsmInfo(TheTriple);
  } else if (TheTriple.getOS() == Triple::Win32) {
    MAI = new Cse523MCAsmInfoMicrosoft(TheTriple);
  } else if (TheTriple.getOS() == Triple::MinGW32 || TheTriple.getOS() == Triple::Cygwin) {
    MAI = new Cse523MCAsmInfoGNUCOFF(TheTriple);
  } else {
    // The default is ELF.
    MAI = new Cse523ELFMCAsmInfo(TheTriple);
  }

  // Initialize initial frame state.
  // Calculate amount of bytes used for return address storing
  int stackGrowth = -8;

  // Initial state of the frame pointer is esp+stackGrowth.
  unsigned StackPtr = Cse523::RSP;
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(
      0, MRI.getDwarfRegNum(StackPtr, true), -stackGrowth);
  MAI->addInitialFrameState(Inst);

  // Add return address to move list
  unsigned InstPtr = Cse523::RIP;
  MCCFIInstruction Inst2 = MCCFIInstruction::createOffset(
      0, MRI.getDwarfRegNum(InstPtr, true), stackGrowth);
  MAI->addInitialFrameState(Inst2);

  return MAI;
}

static MCCodeGenInfo *createCse523MCCodeGenInfo(StringRef TT, Reloc::Model RM,
                                             CodeModel::Model CM,
                                             CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();

  Triple T(TT);

  if (RM == Reloc::Default) {
    // Darwin defaults to PIC in 64 bit mode and dynamic-no-pic in 32 bit mode.
    // Win64 requires rip-rel addressing, thus we force it to PIC. Otherwise we
    // use static relocation model by default.
    if (T.isOSDarwin()) {
      RM = Reloc::PIC_;
    } else if (T.isOSWindows())
      RM = Reloc::PIC_;
    else
      RM = Reloc::Static;
  }

  // ELF and Cse523-64 don't have a distinct DynamicNoPIC model.  DynamicNoPIC
  // is defined as a model for code which may be used in static or dynamic
  // executables but not necessarily a shared library. On Cse523-32 we just
  // compile in -static mode, in Cse523-64 we use PIC.
  if (RM == Reloc::DynamicNoPIC) {
    RM = Reloc::PIC_;
  }

  // If we are on Darwin, disallow static relocation model in Cse523-64 mode, since
  // the Mach-O file format doesn't support it.
  if (RM == Reloc::Static && T.isOSDarwin())
    RM = Reloc::PIC_;

  // For static codegen, if we're not already set, use Small codegen.
  if (CM == CodeModel::Default)
    CM = CodeModel::Small;
  else if (CM == CodeModel::JITDefault)
    // 64-bit JIT places everything in the same buffer except external funcs.
    CM = CodeModel::Large;

  X->InitMCCodeGenInfo(RM, CM, OL);
  return X;
}

static MCStreamer *createMCStreamer(const Target &T, StringRef TT,
                                    MCContext &Ctx, MCAsmBackend &MAB,
                                    raw_ostream &_OS,
                                    MCCodeEmitter *_Emitter,
                                    const MCSubtargetInfo &STI,
                                    bool RelaxAll,
                                    bool NoExecStack) {
  Triple TheTriple(TT);

  if (TheTriple.isOSBinFormatMachO())
    return createMachOStreamer(Ctx, MAB, _OS, _Emitter, RelaxAll);

  if (TheTriple.isOSWindows() && TheTriple.getEnvironment() != Triple::ELF)
    return createWinCOFFStreamer(Ctx, MAB, *_Emitter, _OS, RelaxAll);

  return createELFStreamer(Ctx, MAB, _OS, _Emitter, RelaxAll, NoExecStack);
}

static MCInstPrinter *createCse523MCInstPrinter(const Target &T,
                                             unsigned SyntaxVariant,
                                             const MCAsmInfo &MAI,
                                             const MCInstrInfo &MII,
                                             const MCRegisterInfo &MRI,
                                             const MCSubtargetInfo &STI) {
  if (SyntaxVariant == 0)
    return new Cse523ATTInstPrinter(MAI, MII, MRI);
  if (SyntaxVariant == 1)
    return new Cse523IntelInstPrinter(MAI, MII, MRI);
  return 0;
}

static MCRelocationInfo *createCse523MCRelocationInfo(StringRef TT,
                                                   MCContext &Ctx) {
  Triple TheTriple(TT);
  if (TheTriple.isOSBinFormatMachO() && TheTriple.getArch() == Triple::cse523)
    return createCse523_64MachORelocationInfo(Ctx);
  else if (TheTriple.isOSBinFormatELF())
    return createCse523_64ELFRelocationInfo(Ctx);
  // Default to the stock relocation info.
  return llvm::createMCRelocationInfo(TT, Ctx);
}

static MCInstrAnalysis *createCse523MCInstrAnalysis(const MCInstrInfo *Info) {
  return new MCInstrAnalysis(Info);
}

// Force static initialization.
extern "C" void LLVMInitializeCse523TargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn B(TheCse523Target, createCse523MCAsmInfo);

  // Register the MC codegen info.
  RegisterMCCodeGenInfoFn D(TheCse523Target, createCse523MCCodeGenInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheCse523Target, createCse523MCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheCse523Target, createCse523MCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheCse523Target,
                                          Cse523_MC::createCse523MCSubtargetInfo);

  // Register the MC instruction analyzer.
  TargetRegistry::RegisterMCInstrAnalysis(TheCse523Target,
                                          createCse523MCInstrAnalysis);

  // Register the code emitter.
  TargetRegistry::RegisterMCCodeEmitter(TheCse523Target,
                                        createCse523MCCodeEmitter);

  // Register the asm backend.
  TargetRegistry::RegisterMCAsmBackend(TheCse523Target,
                                       createCse523_64AsmBackend);

  // Register the object streamer.
  TargetRegistry::RegisterMCObjectStreamer(TheCse523Target,
                                           createMCStreamer);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(TheCse523Target,
                                        createCse523MCInstPrinter);

  // Register the MC relocation info.
  TargetRegistry::RegisterMCRelocationInfo(TheCse523Target,
                                           createCse523MCRelocationInfo);
}
