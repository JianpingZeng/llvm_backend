//===-- Cse523MCTargetDesc.h - Cse523 Target Descriptions ---------*- C++ -*-===//
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

#ifndef CSE523MCTARGETDESC_H
#define CSE523MCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class Target;
class StringRef;
class raw_ostream;

extern Target TheCse523Target;

MCCodeEmitter *createCse523MCCodeEmitter(const MCInstrInfo &MCII,
                                        const MCRegisterInfo &MRI,
                                        const MCSubtargetInfo &STI,
                                        MCContext &Ctx);
MCAsmBackend *createCse523AsmBackend(const Target &T,
                                    const MCRegisterInfo &MRI,
                                    StringRef TT,
                                    StringRef CPU);
MCObjectWriter *createCse523ELFObjectWriter(raw_ostream &OS,
                                           bool Is64Bit,
                                           uint8_t OSABI);
} // End llvm namespace

//// Defines symbolic names for Cse523 registers.  This defines a mapping from
//// register name to register number.
////
//#define GET_REGINFO_ENUM
//#include "Cse523GenRegisterInfo.inc"
//
//// Defines symbolic names for the Cse523 instructions.
////
//#define GET_INSTRINFO_ENUM
//#include "Cse523GenInstrInfo.inc"
//
//#define GET_SUBTARGETINFO_ENUM
//#include "Cse523GenSubtargetInfo.inc"

#endif
