//===-- Cse523.td - Describe the Cse523 Target Machine -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// SPARC Subtarget features.
//

def FeatureCSE523
  : SubtargetFeature<"cse523", "IsCSE523", "true",
                     "Enable CSE523 instructions">;

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

//include "Cse523RegisterInfo.td"
//include "Cse523CallingConv.td"
//include "Cse523InstrInfo.td"

//def Cse523InstrInfo : InstrInfo;

//def Cse523AsmParser : AsmParser {
//  bit ShouldEmitMatchRegisterName = 0;
//}

//===----------------------------------------------------------------------===//
// SPARC processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, NoItineraries, Features>;

def : Proc<"cse523",          [FeatureCSE523]>;


//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def Cse523 : Target {
  // Pull in Instruction Info:
  //let InstructionSet = Cse523InstrInfo;
  //let AssemblyParsers  = [Cse523AsmParser];
}
