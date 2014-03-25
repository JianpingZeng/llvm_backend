//===-- Cse523InstrFormats.td - Cse523 Instruction Formats -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Cse523 Instruction Format Definitions.
//

// Format specifies the encoding used by the instruction.  This is part of the
// ad-hoc solution used to emit machine instruction encodings by our machine
// code emitter.
class Format<bits<7> val> {
  bits<7> Value = val;
}

def Pseudo     : Format<0>; def RawFrm     : Format<1>;
def AddRegFrm  : Format<2>; def MRMDestReg : Format<3>;
def MRMDestMem : Format<4>; def MRMSrcReg  : Format<5>;
def MRMSrcMem  : Format<6>; def RawFrmMemOffs : Format<7>;
def RawFrmSrc  : Format<8>; def RawFrmDst     : Format<9>;
def RawFrmDstSrc: Format<10>;
def RawFrmImm8 : Format<11>;
def RawFrmImm16 : Format<12>;
def MRMXr  : Format<14>; def MRMXm  : Format<15>;
def MRM0r  : Format<16>; def MRM1r  : Format<17>; def MRM2r  : Format<18>;
def MRM3r  : Format<19>; def MRM4r  : Format<20>; def MRM5r  : Format<21>;
def MRM6r  : Format<22>; def MRM7r  : Format<23>;
def MRM0m  : Format<24>; def MRM1m  : Format<25>; def MRM2m  : Format<26>;
def MRM3m  : Format<27>; def MRM4m  : Format<28>; def MRM5m  : Format<29>;
def MRM6m  : Format<30>; def MRM7m  : Format<31>;
def MRM_C0 : Format<32>; def MRM_C1 : Format<33>; def MRM_C2 : Format<34>;
def MRM_C3 : Format<35>; def MRM_C4 : Format<36>; def MRM_C8 : Format<37>;
def MRM_C9 : Format<38>; def MRM_CA : Format<39>; def MRM_CB : Format<40>;
def MRM_D0 : Format<41>; def MRM_D1 : Format<42>; def MRM_D4 : Format<43>;
def MRM_D5 : Format<44>; def MRM_D6 : Format<45>; def MRM_D8 : Format<46>;
def MRM_D9 : Format<47>; def MRM_DA : Format<48>; def MRM_DB : Format<49>;
def MRM_DC : Format<50>; def MRM_DD : Format<51>; def MRM_DE : Format<52>;
def MRM_DF : Format<53>; def MRM_E0 : Format<54>; def MRM_E1 : Format<55>;
def MRM_E2 : Format<56>; def MRM_E3 : Format<57>; def MRM_E4 : Format<58>;
def MRM_E5 : Format<59>; def MRM_E8 : Format<60>; def MRM_E9 : Format<61>;
def MRM_EA : Format<62>; def MRM_EB : Format<63>; def MRM_EC : Format<64>;
def MRM_ED : Format<65>; def MRM_EE : Format<66>; def MRM_F0 : Format<67>;
def MRM_F1 : Format<68>; def MRM_F2 : Format<69>; def MRM_F3 : Format<70>;
def MRM_F4 : Format<71>; def MRM_F5 : Format<72>; def MRM_F6 : Format<73>;
def MRM_F7 : Format<74>; def MRM_F8 : Format<75>; def MRM_F9 : Format<76>;
def MRM_FA : Format<77>; def MRM_FB : Format<78>; def MRM_FC : Format<79>;
def MRM_FD : Format<80>; def MRM_FE : Format<81>; def MRM_FF : Format<82>;

// ImmType - This specifies the immediate type used by an instruction. This is
// part of the ad-hoc solution used to emit machine instruction encodings by our
// machine code emitter.
class ImmType<bits<4> val> {
  bits<4> Value = val;
}
def NoImm      : ImmType<0>;
def Imm8       : ImmType<1>;
def Imm8PCRel  : ImmType<2>;
def Imm16      : ImmType<3>;
def Imm16PCRel : ImmType<4>;
def Imm32      : ImmType<5>;
def Imm32PCRel : ImmType<6>;
def Imm32S     : ImmType<7>;
def Imm64      : ImmType<8>;

// FPFormat - This specifies what form this FP instruction has.  This is used by
// the Floating-Point stackifier pass.
class FPFormat<bits<3> val> {
  bits<3> Value = val;
}
def NotFP      : FPFormat<0>;
def ZeroArgFP  : FPFormat<1>;
def OneArgFP   : FPFormat<2>;
def OneArgFPRW : FPFormat<3>;
def TwoArgFP   : FPFormat<4>;
def CompareFP  : FPFormat<5>;
def CondMovFP  : FPFormat<6>;
def SpecialFP  : FPFormat<7>;

// Class specifying the SSE execution domain, used by the SSEDomainFix pass.
// Keep in sync with tables in Cse523InstrInfo.cpp.
class Domain<bits<2> val> {
  bits<2> Value = val;
}
def GenericDomain   : Domain<0>;
def SSEPackedSingle : Domain<1>;
def SSEPackedDouble : Domain<2>;
def SSEPackedInt    : Domain<3>;

// Class specifying the vector form of the decompressed
// displacement of 8-bit.
class CD8VForm<bits<3> val> {
  bits<3> Value = val;
}
def CD8VF  : CD8VForm<0>;  // v := VL
def CD8VH  : CD8VForm<1>;  // v := VL/2
def CD8VQ  : CD8VForm<2>;  // v := VL/4
def CD8VO  : CD8VForm<3>;  // v := VL/8
def CD8VT1 : CD8VForm<4>;  // v := 1
def CD8VT2 : CD8VForm<5>;  // v := 2
def CD8VT4 : CD8VForm<6>;  // v := 4
def CD8VT8 : CD8VForm<7>;  // v := 8

// Class specifying the prefix used an opcode extension.
class Prefix<bits<3> val> {
  bits<3> Value = val;
}
def NoPrfx : Prefix<0>;
def PS     : Prefix<1>;
def PD     : Prefix<2>;
def XS     : Prefix<3>;
def XD     : Prefix<4>;

// Class specifying the opcode map.
class Map<bits<3> val> {
  bits<3> Value = val;
}
def OB   : Map<0>;
def TB   : Map<1>;
def T8   : Map<2>;
def TA   : Map<3>;
def XOP8 : Map<4>;
def XOP9 : Map<5>;
def XOPA : Map<6>;

// Class specifying the encoding
class Encoding<bits<2> val> {
  bits<2> Value = val;
}
def EncNormal : Encoding<0>;
def EncVEX    : Encoding<1>;
def EncXOP    : Encoding<2>;
def EncEVEX   : Encoding<3>;

// Operand size for encodings that change based on mode.
class OperandSize<bits<2> val> {
  bits<2> Value = val;
}
def OpSizeFixed : OperandSize<0>; // Never needs a 0x66 prefix.
def OpSize16    : OperandSize<1>; // Needs 0x66 prefix in 32-bit mode.
def OpSize32    : OperandSize<2>; // Needs 0x66 prefix in 16-bit mode.

// Prefix byte classes which are used to indicate to the ad-hoc machine code
// emitter that various prefix bytes are required.
class OpSize16 { OperandSize OpSize = OpSize16; }
class OpSize32 { OperandSize OpSize = OpSize32; }
class AdSize { bit hasAdSizePrefix = 1; }
class REX_W  { bit hasREX_WPrefix = 1; }
class LOCK   { bit hasLockPrefix = 1; }
class REP    { bit hasREPPrefix = 1; }
class TB     { Map OpMap = TB; }
class T8     { Map OpMap = T8; }
class TA     { Map OpMap = TA; }
class XOP8   { Map OpMap = XOP8; Prefix OpPrefix = PS; }
class XOP9   { Map OpMap = XOP9; Prefix OpPrefix = PS; }
class XOPA   { Map OpMap = XOPA; Prefix OpPrefix = PS; }
class OBXS   { Prefix OpPrefix = XS; }
class PS   : TB { Prefix OpPrefix = PS; }
class PD   : TB { Prefix OpPrefix = PD; }
class XD   : TB { Prefix OpPrefix = XD; }
class XS   : TB { Prefix OpPrefix = XS; }
class T8PS : T8 { Prefix OpPrefix = PS; }
class T8PD : T8 { Prefix OpPrefix = PD; }
class T8XD : T8 { Prefix OpPrefix = XD; }
class T8XS : T8 { Prefix OpPrefix = XS; }
class TAPS : TA { Prefix OpPrefix = PS; }
class TAPD : TA { Prefix OpPrefix = PD; }
class TAXD : TA { Prefix OpPrefix = XD; }
class VEX    { Encoding OpEnc = EncVEX; }
class VEX_W  { bit hasVEX_WPrefix = 1; }
class VEX_4V : VEX { bit hasVEX_4V = 1; }
class VEX_4VOp3 : VEX { bit hasVEX_4VOp3 = 1; }
class VEX_I8IMM { bit hasVEX_i8ImmReg = 1; }
class VEX_L  { bit hasVEX_L = 1; }
class VEX_LIG { bit ignoresVEX_L = 1; }
class EVEX : VEX { Encoding OpEnc = EncEVEX; }
class EVEX_4V : VEX_4V { Encoding OpEnc = EncEVEX; }
class EVEX_K { bit hasEVEX_K = 1; }
class EVEX_KZ : EVEX_K { bit hasEVEX_Z = 1; }
class EVEX_B { bit hasEVEX_B = 1; }
class EVEX_RC { bit hasEVEX_RC = 1; }
class EVEX_V512 { bit hasEVEX_L2 = 1; bit hasVEX_L = 0; }
class EVEX_CD8<int esize, CD8VForm form> {
  bits<2> EVEX_CD8E = !if(!eq(esize, 8),  0b00,
                      !if(!eq(esize, 16), 0b01,
                      !if(!eq(esize, 32), 0b10,
                      !if(!eq(esize, 64), 0b11, ?))));
  bits<3> EVEX_CD8V = form.Value;
}
class Has3DNow0F0FOpcode  { bit has3DNow0F0FOpcode = 1; }
class MemOp4 { bit hasMemOp4Prefix = 1; }
class XOP { Encoding OpEnc = EncXOP; }
class XOP_4V : XOP { bit hasVEX_4V = 1; }
class XOP_4VOp3 : XOP { bit hasVEX_4VOp3 = 1; }

class Cse523Inst<bits<8> opcod, Format f, ImmType i, dag outs, dag ins,
              string AsmStr,
              InstrItinClass itin,
              Domain d = GenericDomain>
  : Instruction {
  let Namespace = "Cse523";

  bits<8> Opcode = opcod;
  Format Form = f;
  bits<7> FormBits = Form.Value;
  ImmType ImmT = i;

  dag OutOperandList = outs;
  dag InOperandList = ins;
  string AsmString = AsmStr;

  // If this is a pseudo instruction, mark it isCodeGenOnly.
  let isCodeGenOnly = !eq(!cast<string>(f), "Pseudo");

  let Itinerary = itin;

  //
  // Attributes specific to Cse523 instructions...
  //
  bit ForceDisassemble = 0; // Force instruction to disassemble even though it's
                            // isCodeGenonly. Needed to hide an ambiguous
                            // AsmString from the parser, but still disassemble.

  OperandSize OpSize = OpSizeFixed; // Does this instruction's encoding change
                                    // based on operand size of the mode
  bits<2> OpSizeBits = OpSize.Value;
  bit hasAdSizePrefix = 0;  // Does this inst have a 0x67 prefix?

  Prefix OpPrefix = NoPrfx; // Which prefix byte does this inst have?
  bits<3> OpPrefixBits = OpPrefix.Value;
  Map OpMap = OB;           // Which opcode map does this inst have?
  bits<3> OpMapBits = OpMap.Value;
  bit hasREX_WPrefix  = 0;  // Does this inst require the REX.W prefix?
  FPFormat FPForm = NotFP;  // What flavor of FP instruction is this?
  bit hasLockPrefix = 0;    // Does this inst have a 0xF0 prefix?
  Domain ExeDomain = d;
  bit hasREPPrefix = 0;     // Does this inst have a REP prefix?
  Encoding OpEnc = EncNormal; // Encoding used by this instruction
  bits<2> OpEncBits = OpEnc.Value;
  bit hasVEX_WPrefix = 0;   // Does this inst set the VEX_W field?
  bit hasVEX_4V = 0;        // Does this inst require the VEX.VVVV field?
  bit hasVEX_4VOp3 = 0;     // Does this inst require the VEX.VVVV field to
                            // encode the third operand?
  bit hasVEX_i8ImmReg = 0;  // Does this inst require the last source register
                            // to be encoded in a immediate field?
  bit hasVEX_L = 0;         // Does this inst use large (256-bit) registers?
  bit ignoresVEX_L = 0;     // Does this instruction ignore the L-bit
  bit hasEVEX_K = 0;        // Does this inst require masking?
  bit hasEVEX_Z = 0;        // Does this inst set the EVEX_Z field?
  bit hasEVEX_L2 = 0;       // Does this inst set the EVEX_L2 field?
  bit hasEVEX_B = 0;        // Does this inst set the EVEX_B field?
  bits<2> EVEX_CD8E = 0;    // Compressed disp8 form - element-size.
  bits<3> EVEX_CD8V = 0;    // Compressed disp8 form - vector-width.
  bit has3DNow0F0FOpcode =0;// Wacky 3dNow! encoding?
  bit hasMemOp4Prefix = 0;  // Same bit as VEX_W, but used for swapping operands
  bit hasEVEX_RC = 0;       // Explicitly specified rounding control in FP instruction.

  // TSFlags layout should be kept in sync with Cse523InstrInfo.h.
  let TSFlags{6-0}   = FormBits;
  let TSFlags{8-7}   = OpSizeBits;
  let TSFlags{9}     = hasAdSizePrefix;
  let TSFlags{12-10} = OpPrefixBits;
  let TSFlags{15-13} = OpMapBits;
  let TSFlags{16}    = hasREX_WPrefix;
  let TSFlags{20-17} = ImmT.Value;
  let TSFlags{23-21} = FPForm.Value;
  let TSFlags{24}    = hasLockPrefix;
  let TSFlags{25}    = hasREPPrefix;
  let TSFlags{27-26} = ExeDomain.Value;
  let TSFlags{29-28} = OpEncBits;
  let TSFlags{37-30} = Opcode;
  let TSFlags{38}    = hasVEX_WPrefix;
  let TSFlags{39}    = hasVEX_4V;
  let TSFlags{40}    = hasVEX_4VOp3;
  let TSFlags{41}    = hasVEX_i8ImmReg;
  let TSFlags{42}    = hasVEX_L;
  let TSFlags{43}    = ignoresVEX_L;
  let TSFlags{44}    = hasEVEX_K;
  let TSFlags{45}    = hasEVEX_Z;
  let TSFlags{46}    = hasEVEX_L2;
  let TSFlags{47}    = hasEVEX_B;
  let TSFlags{49-48} = EVEX_CD8E;
  let TSFlags{52-50} = EVEX_CD8V;
  let TSFlags{53}    = has3DNow0F0FOpcode;
  let TSFlags{54}    = hasMemOp4Prefix;
  let TSFlags{55}    = hasEVEX_RC;
}

class PseudoI<dag oops, dag iops, list<dag> pattern>
  : Cse523Inst<0, Pseudo, NoImm, oops, iops, "", NoItinerary> {
  let Pattern = pattern;
}

class I<bits<8> o, Format f, dag outs, dag ins, string asm,
        list<dag> pattern, InstrItinClass itin = NoItinerary,
        Domain d = GenericDomain>
  : Cse523Inst<o, f, NoImm, outs, ins, asm, itin, d> {
  let Pattern = pattern;
  let CodeSize = 3;
}
class Ii8 <bits<8> o, Format f, dag outs, dag ins, string asm, 
           list<dag> pattern, InstrItinClass itin = NoItinerary,
           Domain d = GenericDomain>
  : Cse523Inst<o, f, Imm8, outs, ins, asm, itin, d> {
  let Pattern = pattern;
  let CodeSize = 3;
}
class Ii8PCRel<bits<8> o, Format f, dag outs, dag ins, string asm, 
               list<dag> pattern, InstrItinClass itin = NoItinerary>
  : Cse523Inst<o, f, Imm8PCRel, outs, ins, asm, itin> {
  let Pattern = pattern;
  let CodeSize = 3;
}
class Ii16<bits<8> o, Format f, dag outs, dag ins, string asm, 
           list<dag> pattern, InstrItinClass itin = NoItinerary>
  : Cse523Inst<o, f, Imm16, outs, ins, asm, itin> {
  let Pattern = pattern;
  let CodeSize = 3;
}
class Ii32<bits<8> o, Format f, dag outs, dag ins, string asm, 
           list<dag> pattern, InstrItinClass itin = NoItinerary>
  : Cse523Inst<o, f, Imm32, outs, ins, asm, itin> {
  let Pattern = pattern;
  let CodeSize = 3;
}
class Ii32S<bits<8> o, Format f, dag outs, dag ins, string asm,
            list<dag> pattern, InstrItinClass itin = NoItinerary>
  : Cse523Inst<o, f, Imm32S, outs, ins, asm, itin> {
  let Pattern = pattern;
  let CodeSize = 3;
}

class Ii16PCRel<bits<8> o, Format f, dag outs, dag ins, string asm, 
           list<dag> pattern, InstrItinClass itin = NoItinerary>
           : Cse523Inst<o, f, Imm16PCRel, outs, ins, asm, itin> {
  let Pattern = pattern;
  let CodeSize = 3;
}

class Ii32PCRel<bits<8> o, Format f, dag outs, dag ins, string asm, 
           list<dag> pattern, InstrItinClass itin = NoItinerary>
  : Cse523Inst<o, f, Imm32PCRel, outs, ins, asm, itin> {
  let Pattern = pattern;
  let CodeSize = 3;
}

// FPStack Instruction Templates:
// FPI - Floating Point Instruction template.
class FPI<bits<8> o, Format F, dag outs, dag ins, string asm,
          InstrItinClass itin = NoItinerary>
  : I<o, F, outs, ins, asm, [], itin> {}

// FpI_ - Floating Point Pseudo Instruction template. Not Predicated.
class FpI_<dag outs, dag ins, FPFormat fp, list<dag> pattern,
           InstrItinClass itin = NoItinerary>
  : Cse523Inst<0, Pseudo, NoImm, outs, ins, "", itin> {
  let FPForm = fp;
  let Pattern = pattern;
}

// Templates for instructions that use a 16- or 32-bit segmented address as
//  their only operand: lcall (FAR CALL) and ljmp (FAR JMP)
//
//   Iseg16 - 16-bit segment selector, 16-bit offset
//   Iseg32 - 16-bit segment selector, 32-bit offset

class Iseg16 <bits<8> o, Format f, dag outs, dag ins, string asm, 
              list<dag> pattern, InstrItinClass itin = NoItinerary>
      : Cse523Inst<o, f, Imm16, outs, ins, asm, itin> {
  let Pattern = pattern;
  let CodeSize = 3;
}

class Iseg32 <bits<8> o, Format f, dag outs, dag ins, string asm, 
              list<dag> pattern, InstrItinClass itin = NoItinerary>
      : Cse523Inst<o, f, Imm32, outs, ins, asm, itin> {
  let Pattern = pattern;
  let CodeSize = 3;
}

// Cse523-64 Instruction templates...
//

class RI<bits<8> o, Format F, dag outs, dag ins, string asm,
         list<dag> pattern, InstrItinClass itin = NoItinerary>
      : I<o, F, outs, ins, asm, pattern, itin>, REX_W;
class RIi8 <bits<8> o, Format F, dag outs, dag ins, string asm,
            list<dag> pattern, InstrItinClass itin = NoItinerary>
      : Ii8<o, F, outs, ins, asm, pattern, itin>, REX_W;
class RIi16 <bits<8> o, Format F, dag outs, dag ins, string asm,
            list<dag> pattern, InstrItinClass itin = NoItinerary>
      : Ii16<o, F, outs, ins, asm, pattern, itin>, REX_W;
class RIi32 <bits<8> o, Format F, dag outs, dag ins, string asm,
             list<dag> pattern, InstrItinClass itin = NoItinerary>
      : Ii32<o, F, outs, ins, asm, pattern, itin>, REX_W;
class RIi32S <bits<8> o, Format F, dag outs, dag ins, string asm,
              list<dag> pattern, InstrItinClass itin = NoItinerary>
      : Ii32S<o, F, outs, ins, asm, pattern, itin>, REX_W;

class RIi64<bits<8> o, Format f, dag outs, dag ins, string asm,
            list<dag> pattern, InstrItinClass itin = NoItinerary>
  : Cse523Inst<o, f, Imm64, outs, ins, asm, itin>, REX_W {
  let Pattern = pattern;
  let CodeSize = 3;
}

class RIi64_NOREX<bits<8> o, Format f, dag outs, dag ins, string asm,
            list<dag> pattern, InstrItinClass itin = NoItinerary>
  : Cse523Inst<o, f, Imm64, outs, ins, asm, itin> {
  let Pattern = pattern;
  let CodeSize = 3;
}

