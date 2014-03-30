//===-- Cse523SelectionDAGInfo.h - Cse523 SelectionDAG Info -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the Cse523 subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef CSE523SELECTIONDAGINFO_H
#define CSE523SELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {

class Cse523TargetLowering;
class Cse523TargetMachine;
class Cse523Subtarget;

class Cse523SelectionDAGInfo : public TargetSelectionDAGInfo {
  /// Subtarget - Keep a pointer to the Cse523Subtarget around so that we can
  /// make the right decision when generating code for different targets.
  const Cse523Subtarget *Subtarget;

  const Cse523TargetLowering &TLI;

public:
  explicit Cse523SelectionDAGInfo(const Cse523TargetMachine &TM);
  ~Cse523SelectionDAGInfo();

  virtual
  SDValue EmitTargetCodeForMemset(SelectionDAG &DAG, SDLoc dl,
                                  SDValue Chain,
                                  SDValue Dst, SDValue Src,
                                  SDValue Size, unsigned Align,
                                  bool isVolatile,
                                  MachinePointerInfo DstPtrInfo) const;

  virtual
  SDValue EmitTargetCodeForMemcpy(SelectionDAG &DAG, SDLoc dl,
                                  SDValue Chain,
                                  SDValue Dst, SDValue Src,
                                  SDValue Size, unsigned Align,
                                  bool isVolatile, bool AlwaysInline,
                                  MachinePointerInfo DstPtrInfo,
                                  MachinePointerInfo SrcPtrInfo) const;
};

}

#endif
