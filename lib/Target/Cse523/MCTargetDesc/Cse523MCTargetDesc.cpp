//===-- Cse523MCTargetDesc.cpp - Cse523 Target Descriptions -----------------===//
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
#include "InstPrinter/Cse523InstPrinter.h"
#include "Cse523MCAsmInfo.h"
#include "Cse523TargetStreamer.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

//#define GET_INSTRINFO_MC_DESC
//#include "Cse523GenInstrInfo.inc"
//
//#define GET_SUBTARGETINFO_MC_DESC
//#include "Cse523GenSubtargetInfo.inc"
//
//#define GET_REGINFO_MC_DESC
//#include "Cse523GenRegisterInfo.inc"

using namespace llvm;


static MCAsmInfo *createCse523MCAsmInfo(const MCRegisterInfo &MRI,
                                       StringRef TT) {
  MCAsmInfo *MAI = new Cse523ELFMCAsmInfo(TT);
  unsigned Reg = 10;//MRI.getDwarfRegNum(SP::O6, true);
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(0, Reg, 0);
  MAI->addInitialFrameState(Inst);
  return MAI;
}

static MCAsmInfo *createCse523V9MCAsmInfo(const MCRegisterInfo &MRI,
                                       StringRef TT) {
  MCAsmInfo *MAI = new Cse523ELFMCAsmInfo(TT);
  unsigned Reg = 10;//MRI.getDwarfRegNum(SP::O6, true);
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(0, Reg, 2047);
  MAI->addInitialFrameState(Inst);
  return MAI;
}

static MCInstrInfo *createCse523MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  //InitCse523MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createCse523MCRegisterInfo(StringRef TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  //InitCse523MCRegisterInfo(X, SP::O7);
  return X;
}

static MCSubtargetInfo *createCse523MCSubtargetInfo(StringRef TT, StringRef CPU,
                                                   StringRef FS) {
  MCSubtargetInfo *X = new MCSubtargetInfo();
  //InitCse523MCSubtargetInfo(X, TT, CPU, FS);
  return X;
}

// Code models. Some only make sense for 64-bit code.
//
// SunCC  Reloc   CodeModel  Constraints
// abs32  Static  Small      text+data+bss linked below 2^32 bytes
// abs44  Static  Medium     text+data+bss linked below 2^44 bytes
// abs64  Static  Large      text smaller than 2^31 bytes
// pic13  PIC_    Small      GOT < 2^13 bytes
// pic32  PIC_    Medium     GOT < 2^32 bytes
//
// All code models require that the text segment is smaller than 2GB.

static MCCodeGenInfo *createCse523MCCodeGenInfo(StringRef TT, Reloc::Model RM,
                                               CodeModel::Model CM,
                                               CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();

  // The default 32-bit code model is abs32/pic32 and the default 32-bit
  // code model for JIT is abs32.
  switch (CM) {
  default: break;
  case CodeModel::Default:
  case CodeModel::JITDefault: CM = CodeModel::Small; break;
  }

  X->InitMCCodeGenInfo(RM, CM, OL);
  return X;
}

static MCCodeGenInfo *createCse523V9MCCodeGenInfo(StringRef TT, Reloc::Model RM,
                                                 CodeModel::Model CM,
                                                 CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();

  // The default 64-bit code model is abs44/pic32 and the default 64-bit
  // code model for JIT is abs64.
  switch (CM) {
  default:  break;
  case CodeModel::Default:
    CM = RM == Reloc::PIC_ ? CodeModel::Small : CodeModel::Medium;
    break;
  case CodeModel::JITDefault:
    CM = CodeModel::Large;
    break;
  }

  X->InitMCCodeGenInfo(RM, CM, OL);
  return X;
}

static MCStreamer *createMCStreamer(const Target &T, StringRef TT,
                                    MCContext &Context, MCAsmBackend &MAB,
                                    raw_ostream &OS, MCCodeEmitter *Emitter,
                                    const MCSubtargetInfo &STI, bool RelaxAll,
                                    bool NoExecStack) {
  MCStreamer *S = createELFStreamer(Context, MAB, OS, Emitter, RelaxAll, NoExecStack);
  new Cse523TargetELFStreamer(*S);
  return S;
}

static MCStreamer *
createMCAsmStreamer(MCContext &Ctx, formatted_raw_ostream &OS,
                    bool isVerboseAsm, bool useCFI, bool useDwarfDirectory,
                    MCInstPrinter *InstPrint, MCCodeEmitter *CE,
                    MCAsmBackend *TAB, bool ShowInst) {

  MCStreamer *S =
      llvm::createAsmStreamer(Ctx, OS, isVerboseAsm, useCFI, useDwarfDirectory,
                              InstPrint, CE, TAB, ShowInst);
  new Cse523TargetAsmStreamer(*S, OS);
  return S;
}

static MCInstPrinter *createCse523MCInstPrinter(const Target &T,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI,
                                              const MCSubtargetInfo &STI) {
  return new Cse523InstPrinter(MAI, MII, MRI);
}

extern "C" void LLVMInitializeCse523TargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X(TheCse523Target, createCse523MCAsmInfo);

  // Register the MC codegen info.
  TargetRegistry::RegisterMCCodeGenInfo(TheCse523Target,
                                       createCse523MCCodeGenInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheCse523Target, createCse523MCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheCse523Target, createCse523MCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheCse523Target,
                                          createCse523MCSubtargetInfo);

  // Register the MC Code Emitter.
  TargetRegistry::RegisterMCCodeEmitter(TheCse523Target,
                                        createCse523MCCodeEmitter);

  //Register the asm backend.
  TargetRegistry::RegisterMCAsmBackend(TheCse523Target,
                                       createCse523AsmBackend);

  // Register the object streamer.
  TargetRegistry::RegisterMCObjectStreamer(TheCse523Target,
                                           createMCStreamer);

  // Register the asm streamer.
  TargetRegistry::RegisterAsmStreamer(TheCse523Target,
                                      createMCAsmStreamer);

  // Register the MCInstPrinter
  TargetRegistry::RegisterMCInstPrinter(TheCse523Target,
                                        createCse523MCInstPrinter);
}
