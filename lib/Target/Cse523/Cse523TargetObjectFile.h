//===-- Cse523TargetObjectFile.h - Cse523 Object Info -----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_CSE523_TARGETOBJECTFILE_H
#define LLVM_TARGET_CSE523_TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

  /// Cse523_64MachoTargetObjectFile - This TLOF implementation is used for Darwin
  /// cse523-64.
  class Cse523_64MachoTargetObjectFile : public TargetLoweringObjectFileMachO {
  public:
    const MCExpr *
    getTTypeGlobalReference(const GlobalValue *GV, unsigned Encoding,
                            Mangler &Mang, const TargetMachine &TM,
                            MachineModuleInfo *MMI, MCStreamer &Streamer) const
        LLVM_OVERRIDE;

    // getCFIPersonalitySymbol - The symbol that gets passed to
    // .cfi_personality.
    MCSymbol *getCFIPersonalitySymbol(const GlobalValue *GV, Mangler &Mang,
                                      const TargetMachine &TM,
                                      MachineModuleInfo *MMI) const
        LLVM_OVERRIDE;
  };

  /// Cse523LinuxTargetObjectFile - This implementation is used for linux cse523
  /// and cse523-64.
  class Cse523LinuxTargetObjectFile : public TargetLoweringObjectFileELF {
    void Initialize(MCContext &Ctx, const TargetMachine &TM) LLVM_OVERRIDE;

    /// \brief Describe a TLS variable address within debug info.
    const MCExpr *getDebugThreadLocalSymbol(const MCSymbol *Sym) const
        LLVM_OVERRIDE;
  };

  /// \brief This implementation is used for Windows targets on cse523 and cse523-64.
  class Cse523WindowsTargetObjectFile : public TargetLoweringObjectFileCOFF {
    const MCExpr *getExecutableRelativeSymbol(const ConstantExpr *CE,
                                              Mangler &Mang,
                                              const TargetMachine &TM) const
        LLVM_OVERRIDE;
  };

} // end namespace llvm

#endif
