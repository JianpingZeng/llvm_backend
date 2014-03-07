//===-- Cse523MCTargetDesc.h - Cse523 Target Descriptions -------------*- C++ -*-===//
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

#ifndef Cse523MCTARGETDESC_H
#define Cse523MCTARGETDESC_H

#include "llvm/Support/DataTypes.h"
#include <string>

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCRelocationInfo;
class Target;
class StringRef;
class raw_ostream;

extern Target TheCse523Target;

/// DWARFFlavour - Flavour of dwarf regnumbers
///
namespace DWARFFlavour {
  enum {
    Cse523 = 0
  };
} 
  
/// N86 namespace - Native Cse523 register numbers
///
namespace N86 {
  enum {
    EAX = 0, ECX = 1, EDX = 2, EBX = 3, ESP = 4, EBP = 5, ESI = 6, EDI = 7
  };
}

namespace Cse523_MC {
  std::string ParseCse523Triple(StringRef TT);

  /// GetCpuIDAndInfo - Execute the specified cpuid and return the 4 values in
  /// the specified arguments.  If we can't run cpuid on the host, return true.
  bool GetCpuIDAndInfo(unsigned value, unsigned *rEAX,
                       unsigned *rEBX, unsigned *rECX, unsigned *rEDX);
  /// GetCpuIDAndInfoEx - Execute the specified cpuid with subleaf and return
  /// the 4 values in the specified arguments.  If we can't run cpuid on the
  /// host, return true.
  bool GetCpuIDAndInfoEx(unsigned value, unsigned subleaf, unsigned *rEAX,
                       unsigned *rEBX, unsigned *rECX, unsigned *rEDX);

  void DetectFamilyModel(unsigned EAX, unsigned &Family, unsigned &Model);

  unsigned getDwarfRegFlavour(StringRef TT, bool isEH);

  void InitLLVM2SEHRegisterMapping(MCRegisterInfo *MRI);

  /// createCse523MCSubtargetInfo - Create a Cse523 MCSubtargetInfo instance.
  /// This is exposed so Asm parser, etc. do not need to go through
  /// TargetRegistry.
  MCSubtargetInfo *createCse523MCSubtargetInfo(StringRef TT, StringRef CPU,
                                            StringRef FS);
}

MCCodeEmitter *createCse523MCCodeEmitter(const MCInstrInfo &MCII,
                                      const MCRegisterInfo &MRI,
                                      const MCSubtargetInfo &STI,
                                      MCContext &Ctx);

MCAsmBackend *createCse523_32AsmBackend(const Target &T, const MCRegisterInfo &MRI,
                                     StringRef TT, StringRef CPU);
MCAsmBackend *createCse523_64AsmBackend(const Target &T, const MCRegisterInfo &MRI,
                                     StringRef TT, StringRef CPU);

/// createCse523MachObjectWriter - Construct an Cse523 Mach-O object writer.
MCObjectWriter *createCse523MachObjectWriter(raw_ostream &OS,
                                          bool Is64Bit,
                                          uint32_t CPUType,
                                          uint32_t CPUSubtype);

/// createCse523ELFObjectWriter - Construct an Cse523 ELF object writer.
MCObjectWriter *createCse523ELFObjectWriter(raw_ostream &OS,
                                         bool IsELF64,
                                         uint8_t OSABI,
                                         uint16_t EMachine);
/// createCse523WinCOFFObjectWriter - Construct an Cse523 Win COFF object writer.
MCObjectWriter *createCse523WinCOFFObjectWriter(raw_ostream &OS, bool Is64Bit);

/// createCse523_64MachORelocationInfo - Construct Cse523-64 Mach-O relocation info.
MCRelocationInfo *createCse523_64MachORelocationInfo(MCContext &Ctx);

/// createCse523_64ELFORelocationInfo - Construct Cse523-64 ELF relocation info.
MCRelocationInfo *createCse523_64ELFRelocationInfo(MCContext &Ctx);
} // End llvm namespace


// Defines symbolic names for Cse523 registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "Cse523GenRegisterInfo.inc"

//// Defines symbolic names for the Cse523 instructions.
////
//#define GET_INSTRINFO_ENUM
//#include "Cse523GenInstrInfo.inc"
//
//#define GET_SUBTARGETINFO_ENUM
//#include "Cse523GenSubtargetInfo.inc"

#endif
