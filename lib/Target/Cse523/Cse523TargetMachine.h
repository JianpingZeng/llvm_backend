//===-- Cse523TargetMachine.h - Define TargetMachine for the Cse523 ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Cse523 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef CSE523TARGETMACHINE_H
#define CSE523TARGETMACHINE_H

#include "Cse523.h"
//#include "Cse523FrameLowering.h"
//#include "Cse523ISelLowering.h"
#include "Cse523InstrInfo.h"
//#include "Cse523JITInfo.h"
//#include "Cse523SelectionDAGInfo.h"
#include "Cse523Subtarget.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

    class StringRef;

    class Cse523TargetMachine : public LLVMTargetMachine {
        virtual void anchor();
        Cse523Subtarget       Subtarget;
        //Cse523FrameLowering   FrameLowering;
        InstrItineraryData InstrItins;
        const DataLayout   DL; // Calculates type size & alignment
        Cse523InstrInfo       InstrInfo;
        //Cse523TargetLowering  TLInfo;
        //Cse523SelectionDAGInfo TSInfo;
        //Cse523JITInfo         JITInfo;

        public:
        Cse523TargetMachine(const Target &T, StringRef TT,
                StringRef CPU, StringRef FS, const TargetOptions &Options,
                Reloc::Model RM, CodeModel::Model CM,
                CodeGenOpt::Level OL);

        virtual const DataLayout *getDataLayout() const { return &DL; }
        virtual const Cse523InstrInfo     *getInstrInfo() const {
            return &InstrInfo;
        }
        //  virtual const TargetFrameLowering  *getFrameLowering() const {
        //    return &FrameLowering;
        //  }
        //  virtual       Cse523JITInfo       *getJITInfo()         {
        //    return &JITInfo;
        //  }
        virtual const Cse523Subtarget     *getSubtargetImpl() const{ return &Subtarget; }
        //  virtual const Cse523TargetLowering *getTargetLowering() const {
        //    return &TLInfo;
        //  }
        //  virtual const Cse523SelectionDAGInfo *getSelectionDAGInfo() const {
        //    return &TSInfo;
        //  }
        virtual const Cse523RegisterInfo  *getRegisterInfo() const {
            return &getInstrInfo()->getRegisterInfo();
        }
        virtual const InstrItineraryData *getInstrItineraryData() const {
          return &InstrItins;
        }
        
        /// \brief Register Cse523 analysis passes with a pass manager.
        virtual void addAnalysisPasses(PassManagerBase &PM);

        // Set up the pass pipeline.
        virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);

        virtual bool addCodeEmitter(PassManagerBase &PM,
                JITCodeEmitter &JCE);
    };

} // End llvm namespace

#endif
