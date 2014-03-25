//===-- Cse523.td - Target definition file for the Intel Cse523 --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is a target description file for the Intel i386 architecture, referred
// to here as the "Cse523" architecture.
//
//===----------------------------------------------------------------------===//

// Get the target-independent interfaces which we are implementing...
//
include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Cse523 Subtarget features
//===----------------------------------------------------------------------===//

def FeatureCSE523
: SubtargetFeature<"cse523", "IsCSE523", "true",
    "Enable CSE523 instructions">;

//===----------------------------------------------------------------------===//
// Cse523 processors supported.
//===----------------------------------------------------------------------===//

include "Cse523Schedule.td"

class Proc<string Name, list<SubtargetFeature> Features>
: ProcessorModel<Name, GenericModel, Features>;

def : Proc<"cse523",          [FeatureCSE523]>;

//===----------------------------------------------------------------------===//
// Register File Description
//===----------------------------------------------------------------------===//

include "Cse523RegisterInfo.td"

//===----------------------------------------------------------------------===//
// Instruction Descriptions
//===----------------------------------------------------------------------===//

include "Cse523InstrInfo.td"

def Cse523InstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// Calling Conventions
//===----------------------------------------------------------------------===//

include "Cse523CallingConv.td"


//===----------------------------------------------------------------------===//
// Assembly Parser
//===----------------------------------------------------------------------===//

def ATTAsmParser : AsmParser {
    string AsmParserClassName = "AsmParser";
}

def ATTAsmParserVariant : AsmParserVariant {
    int Variant = 0;

    // Variant name.
    string Name = "att";

    // Discard comments in assembly strings.
    string CommentDelimiter = "#";

    // Recognize hard coded registers.
    string RegisterPrefix = "%";
}

def IntelAsmParserVariant : AsmParserVariant {
    int Variant = 1;

    // Variant name.
    string Name = "intel";

    // Discard comments in assembly strings.
    string CommentDelimiter = ";";

    // Recognize hard coded registers.
    string RegisterPrefix = "";
}

//===----------------------------------------------------------------------===//
// Assembly Printers
//===----------------------------------------------------------------------===//

// The Cse523 target supports two different syntaxes for emitting machine code.
// This is controlled by the -x86-asm-syntax={att|intel}
def ATTAsmWriter : AsmWriter {
    string AsmWriterClassName  = "ATTInstPrinter";
    int Variant = 0;
}
def IntelAsmWriter : AsmWriter {
    string AsmWriterClassName  = "IntelInstPrinter";
    int Variant = 1;
}

def Cse523 : Target {
    // Information about the instructions...
    let InstructionSet = Cse523InstrInfo;
    let AssemblyParsers = [ATTAsmParser];
    let AssemblyParserVariants = [ATTAsmParserVariant, IntelAsmParserVariant];
    let AssemblyWriters = [ATTAsmWriter, IntelAsmWriter];
}

