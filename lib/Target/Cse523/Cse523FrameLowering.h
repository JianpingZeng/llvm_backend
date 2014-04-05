//===-- Cse523TargetFrameLowering.h - Define frame lowering for Cse523 -*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class implements Cse523-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef CSE523_FRAMELOWERING_H
#define CSE523_FRAMELOWERING_H

#include "Cse523Subtarget.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {

class MCSymbol;
class Cse523TargetMachine;

class Cse523FrameLowering : public TargetFrameLowering {
  const Cse523TargetMachine &TM;
  const Cse523Subtarget &STI;
public:
  explicit Cse523FrameLowering(const Cse523TargetMachine &tm, const Cse523Subtarget &sti)
    : TargetFrameLowering(StackGrowsDown, sti.getStackAlignment(), -8),
      TM(tm), STI(sti) {
  }

  void emitCalleeSavedFrameMoves(MachineFunction &MF, MCSymbol *Label,
                                 unsigned FramePtr) const;

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF) const;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;

  void adjustForSegmentedStacks(MachineFunction &MF) const;

  void adjustForHiPEPrologue(MachineFunction &MF) const;

  void processFunctionBeforeCalleeSavedScan(MachineFunction &MF,
                                            RegScavenger *RS = NULL) const;

  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 const std::vector<CalleeSavedInfo> &CSI,
                                 const TargetRegisterInfo *TRI) const;

  bool restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator MI,
                                   const std::vector<CalleeSavedInfo> &CSI,
                                   const TargetRegisterInfo *TRI) const;

  bool hasFP(const MachineFunction &MF) const;
  bool hasReservedCallFrame(const MachineFunction &MF) const;

  int getFrameIndexOffset(const MachineFunction &MF, int FI) const;
  int getFrameIndexReference(const MachineFunction &MF, int FI,
                             unsigned &FrameReg) const;

  void eliminateCallFramePseudoInstr(MachineFunction &MF,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator MI) const;
};

} // End llvm namespace

#endif
