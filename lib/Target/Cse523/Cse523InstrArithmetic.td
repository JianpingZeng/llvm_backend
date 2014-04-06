//===-- Cse523InstrArithmetic.td - Integer Arithmetic Instrs --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the integer arithmetic instructions in the Cse523
// architecture.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// LEA - Load Effective Address
let SchedRW = [WriteLEA] in {
let neverHasSideEffects = 1 in
let isReMaterializable = 1 in
def LEA64r   : RI<0x8D, MRMSrcMem, (outs GR64:$dst), (ins lea64mem:$src),
                  "lea{q}\t{$src|$dst}, {$dst|$src}",
                  [(set GR64:$dst, lea64addr:$src)], IIC_LEA>;
} // SchedRW

//===----------------------------------------------------------------------===//
//  Fixed-Register Multiplication and Division Instructions.
//

// SchedModel info for instruction that loads one value and gets the second
// (and possibly third) value from a register.
// This is used for instructions that put the memory operands before other
// uses.
class SchedLoadReg<SchedWrite SW> : Sched<[SW,
  // Memory operand.
  ReadDefault, ReadDefault, ReadDefault, ReadDefault, ReadDefault,
  // Register reads (implicit or explicit).
  ReadAfterLd, ReadAfterLd]>;

// Extra precision multiplication
// RAX,RDX = RAX*GR64
let Defs = [RAX,RDX,EFLAGS], Uses = [RAX], neverHasSideEffects = 1 in
def MUL64r : RI<0xF7, MRM4r, (outs), (ins GR64:$src),
                "mul{q}\t$src",
                [/*(set RAX, RDX, EFLAGS, (Cse523umul_flag RAX, GR64:$src))*/],
                IIC_MUL64>, Sched<[WriteIMul]>;
// AX,DX = AX*[mem16]
let mayLoad = 1, neverHasSideEffects = 1 in {
// RAX,RDX = RAX*[mem64]
let Defs = [RAX,RDX,EFLAGS], Uses = [RAX] in
def MUL64m : RI<0xF7, MRM4m, (outs), (ins i64mem:$src),
                "mul{q}\t$src", [], IIC_MUL64>, SchedLoadReg<WriteIMulLd>;
}

let neverHasSideEffects = 1 in {
// RAX,RDX = RAX*GR64
let Defs = [RAX,RDX,EFLAGS], Uses = [RAX] in
def IMUL64r : RI<0xF7, MRM5r, (outs), (ins GR64:$src), "imul{q}\t$src", [],
              IIC_IMUL64_RR>, Sched<[WriteIMul]>;

let mayLoad = 1 in {
// RAX,RDX = RAX*[mem64]
let Defs = [RAX,RDX,EFLAGS], Uses = [RAX] in
def IMUL64m : RI<0xF7, MRM5m, (outs), (ins i64mem:$src),
                 "imul{q}\t$src", [], IIC_IMUL64>, SchedLoadReg<WriteIMulLd>;
}
} // neverHasSideEffects


let Defs = [EFLAGS] in {
let Constraints = "$src1 = $dst" in {

let isCommutable = 1, SchedRW = [WriteIMul] in {
// X = IMUL Y, Z --> X = IMUL Z, Y
// Register-Register Signed Integer Multiply
def IMUL64rr : RI<0xAF, MRMSrcReg, (outs GR64:$dst),
                                   (ins GR64:$src1, GR64:$src2),
                  "imul{q}\t{$src2, $dst|$dst, $src2}",
                  [(set GR64:$dst, EFLAGS,
                        (Cse523smul_flag GR64:$src1, GR64:$src2))], IIC_IMUL64_RR>,
                 TB;
} // isCommutable, SchedRW

// Register-Memory Signed Integer Multiply
let SchedRW = [WriteIMulLd, ReadAfterLd] in {
def IMUL64rm : RI<0xAF, MRMSrcMem, (outs GR64:$dst),
                                   (ins GR64:$src1, i64mem:$src2),
                  "imul{q}\t{$src2, $dst|$dst, $src2}",
                  [(set GR64:$dst, EFLAGS,
                        (Cse523smul_flag GR64:$src1, (load addr:$src2)))],
                        IIC_IMUL64_RM>,
               TB;
} // SchedRW
} // Constraints = "$src1 = $dst"

} // Defs = [EFLAGS]

// Surprisingly enough, these are not two address instructions!
let Defs = [EFLAGS] in {
let SchedRW = [WriteIMul] in {
// Register-Integer Signed Integer Multiply
def IMUL64rri32 : RIi32S<0x69, MRMSrcReg,                    // GR64 = GR64*I32
                         (outs GR64:$dst), (ins GR64:$src1, i64i32imm:$src2),
                         "imul{q}\t{$src2, $src1, $dst|$dst, $src1, $src2}",
                         [(set GR64:$dst, EFLAGS,
                             (Cse523smul_flag GR64:$src1, i64immSExt32:$src2))],
                             IIC_IMUL64_RRI>;
} // SchedRW

// Memory-Integer Signed Integer Multiply
let SchedRW = [WriteIMulLd] in {
def IMUL64rmi32 : RIi32S<0x69, MRMSrcMem,                   // GR64 = [mem64]*I32
                         (outs GR64:$dst), (ins i64mem:$src1, i64i32imm:$src2),
                         "imul{q}\t{$src2, $src1, $dst|$dst, $src1, $src2}",
                         [(set GR64:$dst, EFLAGS,
                              (Cse523smul_flag (load addr:$src1),
                                            i64immSExt32:$src2))],
                                            IIC_IMUL64_RMI>;
} // SchedRW
} // Defs = [EFLAGS]




// unsigned division/remainder
let hasSideEffects = 1 in { // so that we don't speculatively execute
let SchedRW = [WriteIDiv] in {
// RDX:RAX/r64 = RAX,RDX
let Defs = [RAX,RDX,EFLAGS], Uses = [RAX,RDX] in
def DIV64r : RI<0xF7, MRM6r, (outs), (ins GR64:$src),
                "div{q}\t$src", [], IIC_DIV64>;
} // SchedRW

let mayLoad = 1 in {
// RDX:RAX/[mem64] = RAX,RDX
let Defs = [RAX,RDX,EFLAGS], Uses = [RAX,RDX] in
def DIV64m : RI<0xF7, MRM6m, (outs), (ins i64mem:$src),
                "div{q}\t$src", [], IIC_DIV64>,
             SchedLoadReg<WriteIDivLd>;
}

// Signed division/remainder.
let SchedRW = [WriteIDiv] in {
// RDX:RAX/r64 = RAX,RDX
let Defs = [RAX,RDX,EFLAGS], Uses = [RAX,RDX] in
def IDIV64r: RI<0xF7, MRM7r, (outs), (ins GR64:$src),
                "idiv{q}\t$src", [], IIC_IDIV64>;
} // SchedRW

let mayLoad = 1 in {
let Defs = [RAX,RDX,EFLAGS], Uses = [RAX,RDX] in // RDX:RAX/[mem64] = RAX,RDX
def IDIV64m: RI<0xF7, MRM7m, (outs), (ins i64mem:$src),
                "idiv{q}\t$src", [], IIC_IDIV64>,
             SchedLoadReg<WriteIDivLd>;
}
} // hasSideEffects = 0

//===----------------------------------------------------------------------===//
//  Two address Instructions.
//

// unary instructions
let CodeSize = 2 in {
let Defs = [EFLAGS] in {
let Constraints = "$src1 = $dst", SchedRW = [WriteALU] in {
def NEG64r : RI<0xF7, MRM3r, (outs GR64:$dst), (ins GR64:$src1), "neg{q}\t$dst",
                [(set GR64:$dst, (ineg GR64:$src1)),
                 (implicit EFLAGS)], IIC_UNARY_REG>;
} // Constraints = "$src1 = $dst", SchedRW

// Read-modify-write negate.
let SchedRW = [WriteALULd, WriteRMW] in {
def NEG64m : RI<0xF7, MRM3m, (outs), (ins i64mem:$dst), "neg{q}\t$dst",
                [(store (ineg (loadi64 addr:$dst)), addr:$dst),
                 (implicit EFLAGS)], IIC_UNARY_MEM>;
} // SchedRW
} // Defs = [EFLAGS]


// Note: NOT does not set EFLAGS!

let Constraints = "$src1 = $dst", SchedRW = [WriteALU] in {
// Match xor -1 to not. Favors these over a move imm + xor to save code size.
let AddedComplexity = 15 in {
def NOT64r : RI<0xF7, MRM2r, (outs GR64:$dst), (ins GR64:$src1), "not{q}\t$dst",
                [(set GR64:$dst, (not GR64:$src1))], IIC_UNARY_REG>;
}
} // Constraints = "$src1 = $dst", SchedRW

let SchedRW = [WriteALULd, WriteRMW] in {
def NOT64m : RI<0xF7, MRM2m, (outs), (ins i64mem:$dst), "not{q}\t$dst",
                [(store (not (loadi64 addr:$dst)), addr:$dst)], IIC_UNARY_MEM>;
} // SchedRW
} // CodeSize

// TODO: inc/dec is slow for P4, but fast for Pentium-M.
let Defs = [EFLAGS] in {
let Constraints = "$src1 = $dst", SchedRW = [WriteALU] in {

let isConvertibleToThreeAddress = 1, CodeSize = 1 in {  // Can xform into LEA.
def INC64r : RI<0xFF, MRM0r, (outs GR64:$dst), (ins GR64:$src1), "inc{q}\t$dst",
                [(set GR64:$dst, EFLAGS, (Cse523inc_flag GR64:$src1))],
                IIC_UNARY_REG>;
} // isConvertibleToThreeAddress = 1, CodeSize = 1

} // Constraints = "$src1 = $dst", SchedRW

let CodeSize = 2, SchedRW = [WriteALULd, WriteRMW] in {
  def INC64m : RI<0xFF, MRM0m, (outs), (ins i64mem:$dst), "inc{q}\t$dst",
                  [(store (add (loadi64 addr:$dst), 1), addr:$dst),
                   (implicit EFLAGS)], IIC_UNARY_MEM>;
} // CodeSize = 2, SchedRW

let Constraints = "$src1 = $dst", SchedRW = [WriteALU] in {
let isConvertibleToThreeAddress = 1, CodeSize = 1 in {   // Can xform into LEA.
def DEC64r : RI<0xFF, MRM1r, (outs GR64:$dst), (ins GR64:$src1), "dec{q}\t$dst",
                [(set GR64:$dst, EFLAGS, (Cse523dec_flag GR64:$src1))],
                IIC_UNARY_REG>;
} // CodeSize = 2
} // Constraints = "$src1 = $dst", SchedRW


let CodeSize = 2, SchedRW = [WriteALULd, WriteRMW] in {
  def DEC64m : RI<0xFF, MRM1m, (outs), (ins i64mem:$dst), "dec{q}\t$dst",
                  [(store (add (loadi64 addr:$dst), -1), addr:$dst),
                   (implicit EFLAGS)], IIC_UNARY_MEM>;
} // CodeSize = 2, SchedRW
} // Defs = [EFLAGS]

/// Cse523TypeInfo - This is a bunch of information that describes relevant Cse523
/// information about value types.  For example, it can tell you what the
/// register class and preferred load to use.
class Cse523TypeInfo<ValueType vt, string instrsuffix, RegisterClass regclass,
                  PatFrag loadnode, Cse523MemOperand memoperand, ImmType immkind,
                  Operand immoperand, SDPatternOperator immoperator,
                  Operand imm8operand, SDPatternOperator imm8operator,
                  bit hasOddOpcode, OperandSize opSize,
                  bit hasREX_WPrefix> {
  /// VT - This is the value type itself.
  ValueType VT = vt;

  /// InstrSuffix - This is the suffix used on instructions with this type.  For
  /// example, i8 -> "b", i16 -> "w", i32 -> "l", i64 -> "q".
  string InstrSuffix = instrsuffix;

  /// RegClass - This is the register class associated with this type.  For
  /// example, i8 -> GR8, i16 -> GR16, i32 -> GR32, i64 -> GR64.
  RegisterClass RegClass = regclass;

  /// LoadNode - This is the load node associated with this type.  For
  /// example, i8 -> loadi8, i16 -> loadi16, i32 -> loadi32, i64 -> loadi64.
  PatFrag LoadNode = loadnode;

  /// MemOperand - This is the memory operand associated with this type.  For
  /// example, i8 -> i8mem, i16 -> i16mem, i32 -> i32mem, i64 -> i64mem.
  Cse523MemOperand MemOperand = memoperand;

  /// ImmEncoding - This is the encoding of an immediate of this type.  For
  /// example, i8 -> Imm8, i16 -> Imm16, i32 -> Imm32.  Note that i64 -> Imm32
  /// since the immediate fields of i64 instructions is a 32-bit sign extended
  /// value.
  ImmType ImmEncoding = immkind;

  /// ImmOperand - This is the operand kind of an immediate of this type.  For
  /// example, i8 -> i8imm, i16 -> i16imm, i32 -> i32imm.  Note that i64 ->
  /// i64i32imm since the immediate fields of i64 instructions is a 32-bit sign
  /// extended value.
  Operand ImmOperand = immoperand;

  /// ImmOperator - This is the operator that should be used to match an
  /// immediate of this kind in a pattern (e.g. imm, or i64immSExt32).
  SDPatternOperator ImmOperator = immoperator;

  /// Imm8Operand - This is the operand kind to use for an imm8 of this type.
  /// For example, i8 -> <invalid>, i16 -> i16i8imm, i32 -> i32i8imm.  This is
  /// only used for instructions that have a sign-extended imm8 field form.
  Operand Imm8Operand = imm8operand;

  /// Imm8Operator - This is the operator that should be used to match an 8-bit
  /// sign extended immediate of this kind in a pattern (e.g. imm16immSExt8).
  SDPatternOperator Imm8Operator = imm8operator;

  /// HasOddOpcode - This bit is true if the instruction should have an odd (as
  /// opposed to even) opcode.  Operations on i8 are usually even, operations on
  /// other datatypes are odd.
  bit HasOddOpcode = hasOddOpcode;

  /// OpSize - Selects whether the instruction needs a 0x66 prefix based on
  /// 16-bit vs 32-bit mode. i8/i64 set this to OpSizeFixed. i16 sets this
  /// to Opsize16. i32 sets this to OpSize32.
  OperandSize OpSize = opSize;

  /// HasREX_WPrefix - This bit is set to true if the instruction should have
  /// the 0x40 REX prefix.  This is set for i64 types.
  bit HasREX_WPrefix = hasREX_WPrefix;
}

def invalid_node : SDNode<"<<invalid_node>>", SDTIntLeaf,[],"<<invalid_node>>">;

def Xi64 : Cse523TypeInfo<i64, "q", GR64, loadi64, i64mem,
                       Imm32S, i64i32imm, i64immSExt32, i64i8imm, i64immSExt8,
                       1, OpSizeFixed, 1>;

/// ITy - This instruction base class takes the type info for the instruction.
/// Using this, it:
/// 1. Concatenates together the instruction mnemonic with the appropriate
///    suffix letter, a tab, and the arguments.
/// 2. Infers whether the instruction should have a 0x66 prefix byte.
/// 3. Infers whether the instruction should have a 0x40 REX_W prefix.
/// 4. Infers whether the low bit of the opcode should be 0 (for i8 operations)
///    or 1 (for i16,i32,i64 operations).
class ITy<bits<8> opcode, Format f, Cse523TypeInfo typeinfo, dag outs, dag ins,
          string mnemonic, string args, list<dag> pattern,
          InstrItinClass itin = IIC_BIN_NONMEM>
  : I<{opcode{7}, opcode{6}, opcode{5}, opcode{4},
       opcode{3}, opcode{2}, opcode{1}, typeinfo.HasOddOpcode },
      f, outs, ins,
      !strconcat(mnemonic, "{", typeinfo.InstrSuffix, "}\t", args), pattern,
      itin> {

  // Infer instruction prefixes from type info.
  let OpSize = typeinfo.OpSize;
  let hasREX_WPrefix  = typeinfo.HasREX_WPrefix;
}

// BinOpRR - Instructions like "add reg, reg, reg".
class BinOpRR<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
              dag outlist, list<dag> pattern, InstrItinClass itin,
              Format f = MRMDestReg>
  : ITy<opcode, f, typeinfo, outlist,
        (ins typeinfo.RegClass:$src1, typeinfo.RegClass:$src2),
        mnemonic, "{$src2, $src1|$src1, $src2}", pattern, itin>,
    Sched<[WriteALU]>;

// BinOpRR_R - Instructions like "add reg, reg, reg", where the pattern has
// just a regclass (no eflags) as a result.
class BinOpRR_R<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                SDNode opnode>
  : BinOpRR<opcode, mnemonic, typeinfo, (outs typeinfo.RegClass:$dst),
            [(set typeinfo.RegClass:$dst,
                  (opnode typeinfo.RegClass:$src1, typeinfo.RegClass:$src2))],
                  IIC_BIN_NONMEM>;

// BinOpRR_F - Instructions like "cmp reg, Reg", where the pattern has
// just a EFLAGS as a result.
class BinOpRR_F<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                SDPatternOperator opnode, Format f = MRMDestReg>
  : BinOpRR<opcode, mnemonic, typeinfo, (outs),
            [(set EFLAGS,
                  (opnode typeinfo.RegClass:$src1, typeinfo.RegClass:$src2))],
            IIC_BIN_NONMEM, f>;

// BinOpRR_RF - Instructions like "add reg, reg, reg", where the pattern has
// both a regclass and EFLAGS as a result.
class BinOpRR_RF<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                 SDNode opnode>
  : BinOpRR<opcode, mnemonic, typeinfo, (outs typeinfo.RegClass:$dst),
            [(set typeinfo.RegClass:$dst, EFLAGS,
                  (opnode typeinfo.RegClass:$src1, typeinfo.RegClass:$src2))],
                  IIC_BIN_NONMEM>;

// BinOpRR_RFF - Instructions like "adc reg, reg, reg", where the pattern has
// both a regclass and EFLAGS as a result, and has EFLAGS as input.
class BinOpRR_RFF<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                  SDNode opnode>
  : BinOpRR<opcode, mnemonic, typeinfo, (outs typeinfo.RegClass:$dst),
            [(set typeinfo.RegClass:$dst, EFLAGS,
                  (opnode typeinfo.RegClass:$src1, typeinfo.RegClass:$src2,
                          EFLAGS))], IIC_BIN_CARRY_NONMEM>;

// BinOpRR_Rev - Instructions like "add reg, reg, reg" (reversed encoding).
class BinOpRR_Rev<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                 InstrItinClass itin = IIC_BIN_NONMEM>
  : ITy<opcode, MRMSrcReg, typeinfo,
        (outs typeinfo.RegClass:$dst),
        (ins typeinfo.RegClass:$src1, typeinfo.RegClass:$src2),
        mnemonic, "{$src2, $dst|$dst, $src2}", [], itin>,
    Sched<[WriteALU]> {
  // The disassembler should know about this, but not the asmparser.
  let isCodeGenOnly = 1;
  let ForceDisassemble = 1;
  let hasSideEffects = 0;
}

// BinOpRR_RDD_Rev - Instructions like "adc reg, reg, reg" (reversed encoding).
class BinOpRR_RFF_Rev<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo>
  : BinOpRR_Rev<opcode, mnemonic, typeinfo, IIC_BIN_CARRY_NONMEM>;

// BinOpRR_F_Rev - Instructions like "cmp reg, reg" (reversed encoding).
class BinOpRR_F_Rev<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo>
  : ITy<opcode, MRMSrcReg, typeinfo, (outs),
        (ins typeinfo.RegClass:$src1, typeinfo.RegClass:$src2),
        mnemonic, "{$src2, $src1|$src1, $src2}", [], IIC_BIN_NONMEM>,
    Sched<[WriteALU]> {
  // The disassembler should know about this, but not the asmparser.
  let isCodeGenOnly = 1;
  let ForceDisassemble = 1;
  let hasSideEffects = 0;
}

// BinOpRM - Instructions like "add reg, reg, [mem]".
class BinOpRM<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
              dag outlist, list<dag> pattern,
              InstrItinClass itin = IIC_BIN_MEM>
  : ITy<opcode, MRMSrcMem, typeinfo, outlist,
        (ins typeinfo.RegClass:$src1, typeinfo.MemOperand:$src2),
        mnemonic, "{$src2, $src1|$src1, $src2}", pattern, itin>,
    Sched<[WriteALULd, ReadAfterLd]>;

// BinOpRM_R - Instructions like "add reg, reg, [mem]".
class BinOpRM_R<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
              SDNode opnode>
  : BinOpRM<opcode, mnemonic, typeinfo, (outs typeinfo.RegClass:$dst),
            [(set typeinfo.RegClass:$dst,
            (opnode typeinfo.RegClass:$src1, (typeinfo.LoadNode addr:$src2)))]>;

// BinOpRM_F - Instructions like "cmp reg, [mem]".
class BinOpRM_F<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
              SDPatternOperator opnode>
  : BinOpRM<opcode, mnemonic, typeinfo, (outs),
            [(set EFLAGS,
            (opnode typeinfo.RegClass:$src1, (typeinfo.LoadNode addr:$src2)))]>;

// BinOpRM_RF - Instructions like "add reg, reg, [mem]".
class BinOpRM_RF<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                 SDNode opnode>
  : BinOpRM<opcode, mnemonic, typeinfo, (outs typeinfo.RegClass:$dst),
            [(set typeinfo.RegClass:$dst, EFLAGS,
            (opnode typeinfo.RegClass:$src1, (typeinfo.LoadNode addr:$src2)))]>;

// BinOpRM_RFF - Instructions like "adc reg, reg, [mem]".
class BinOpRM_RFF<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                 SDNode opnode>
  : BinOpRM<opcode, mnemonic, typeinfo, (outs typeinfo.RegClass:$dst),
            [(set typeinfo.RegClass:$dst, EFLAGS,
            (opnode typeinfo.RegClass:$src1, (typeinfo.LoadNode addr:$src2),
                    EFLAGS))], IIC_BIN_CARRY_MEM>;

// BinOpRI - Instructions like "add reg, reg, imm".
class BinOpRI<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
              Format f, dag outlist, list<dag> pattern,
              InstrItinClass itin = IIC_BIN_NONMEM>
  : ITy<opcode, f, typeinfo, outlist,
        (ins typeinfo.RegClass:$src1, typeinfo.ImmOperand:$src2),
        mnemonic, "{$src2, $src1|$src1, $src2}", pattern, itin>,
    Sched<[WriteALU]> {
  let ImmT = typeinfo.ImmEncoding;
}

// BinOpRI_R - Instructions like "add reg, reg, imm".
class BinOpRI_R<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                SDNode opnode, Format f>
  : BinOpRI<opcode, mnemonic, typeinfo, f, (outs typeinfo.RegClass:$dst),
            [(set typeinfo.RegClass:$dst,
                (opnode typeinfo.RegClass:$src1, typeinfo.ImmOperator:$src2))]>;

// BinOpRI_F - Instructions like "cmp reg, imm".
class BinOpRI_F<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                SDPatternOperator opnode, Format f>
  : BinOpRI<opcode, mnemonic, typeinfo, f, (outs),
            [(set EFLAGS,
                (opnode typeinfo.RegClass:$src1, typeinfo.ImmOperator:$src2))]>;

// BinOpRI_RF - Instructions like "add reg, reg, imm".
class BinOpRI_RF<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                 SDNode opnode, Format f>
  : BinOpRI<opcode, mnemonic, typeinfo, f, (outs typeinfo.RegClass:$dst),
            [(set typeinfo.RegClass:$dst, EFLAGS,
                (opnode typeinfo.RegClass:$src1, typeinfo.ImmOperator:$src2))]>;
// BinOpRI_RFF - Instructions like "adc reg, reg, imm".
class BinOpRI_RFF<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                 SDNode opnode, Format f>
  : BinOpRI<opcode, mnemonic, typeinfo, f, (outs typeinfo.RegClass:$dst),
            [(set typeinfo.RegClass:$dst, EFLAGS,
                (opnode typeinfo.RegClass:$src1, typeinfo.ImmOperator:$src2,
                        EFLAGS))], IIC_BIN_CARRY_NONMEM>;

// BinOpRI8 - Instructions like "add reg, reg, imm8".
class BinOpRI8<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
               Format f, dag outlist, list<dag> pattern,
               InstrItinClass itin = IIC_BIN_NONMEM>
  : ITy<opcode, f, typeinfo, outlist,
        (ins typeinfo.RegClass:$src1, typeinfo.Imm8Operand:$src2),
        mnemonic, "{$src2, $src1|$src1, $src2}", pattern, itin>,
    Sched<[WriteALU]> {
  let ImmT = Imm8; // Always 8-bit immediate.
}

// BinOpRI8_R - Instructions like "add reg, reg, imm8".
class BinOpRI8_R<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                  SDNode opnode, Format f>
  : BinOpRI8<opcode, mnemonic, typeinfo, f, (outs typeinfo.RegClass:$dst),
             [(set typeinfo.RegClass:$dst,
               (opnode typeinfo.RegClass:$src1, typeinfo.Imm8Operator:$src2))]>;

// BinOpRI8_F - Instructions like "cmp reg, imm8".
class BinOpRI8_F<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                  SDNode opnode, Format f>
  : BinOpRI8<opcode, mnemonic, typeinfo, f, (outs),
             [(set EFLAGS,
               (opnode typeinfo.RegClass:$src1, typeinfo.Imm8Operator:$src2))]>;

// BinOpRI8_RF - Instructions like "add reg, reg, imm8".
class BinOpRI8_RF<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                  SDNode opnode, Format f>
  : BinOpRI8<opcode, mnemonic, typeinfo, f, (outs typeinfo.RegClass:$dst),
             [(set typeinfo.RegClass:$dst, EFLAGS,
               (opnode typeinfo.RegClass:$src1, typeinfo.Imm8Operator:$src2))]>;

// BinOpRI8_RFF - Instructions like "adc reg, reg, imm8".
class BinOpRI8_RFF<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                   SDNode opnode, Format f>
  : BinOpRI8<opcode, mnemonic, typeinfo, f, (outs typeinfo.RegClass:$dst),
             [(set typeinfo.RegClass:$dst, EFLAGS,
               (opnode typeinfo.RegClass:$src1, typeinfo.Imm8Operator:$src2,
                       EFLAGS))], IIC_BIN_CARRY_NONMEM>;

// BinOpMR - Instructions like "add [mem], reg".
class BinOpMR<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
              list<dag> pattern, InstrItinClass itin = IIC_BIN_MEM>
  : ITy<opcode, MRMDestMem, typeinfo,
        (outs), (ins typeinfo.MemOperand:$dst, typeinfo.RegClass:$src),
        mnemonic, "{$src, $dst|$dst, $src}", pattern, itin>,
    Sched<[WriteALULd, WriteRMW]>;

// BinOpMR_RMW - Instructions like "add [mem], reg".
class BinOpMR_RMW<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                  SDNode opnode>
  : BinOpMR<opcode, mnemonic, typeinfo,
          [(store (opnode (load addr:$dst), typeinfo.RegClass:$src), addr:$dst),
           (implicit EFLAGS)]>;

// BinOpMR_RMW_FF - Instructions like "adc [mem], reg".
class BinOpMR_RMW_FF<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                    SDNode opnode>
  : BinOpMR<opcode, mnemonic, typeinfo,
          [(store (opnode (load addr:$dst), typeinfo.RegClass:$src, EFLAGS),
                  addr:$dst),
           (implicit EFLAGS)], IIC_BIN_CARRY_MEM>;

// BinOpMR_F - Instructions like "cmp [mem], reg".
class BinOpMR_F<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                  SDNode opnode>
  : BinOpMR<opcode, mnemonic, typeinfo,
            [(set EFLAGS, (opnode (load addr:$dst), typeinfo.RegClass:$src))]>;

// BinOpMI - Instructions like "add [mem], imm".
class BinOpMI<string mnemonic, Cse523TypeInfo typeinfo,
              Format f, list<dag> pattern, bits<8> opcode = 0x80,
              InstrItinClass itin = IIC_BIN_MEM>
  : ITy<opcode, f, typeinfo,
        (outs), (ins typeinfo.MemOperand:$dst, typeinfo.ImmOperand:$src),
        mnemonic, "{$src, $dst|$dst, $src}", pattern, itin>,
    Sched<[WriteALULd, WriteRMW]> {
  let ImmT = typeinfo.ImmEncoding;
}

// BinOpMI_RMW - Instructions like "add [mem], imm".
class BinOpMI_RMW<string mnemonic, Cse523TypeInfo typeinfo,
                  SDNode opnode, Format f>
  : BinOpMI<mnemonic, typeinfo, f,
            [(store (opnode (typeinfo.VT (load addr:$dst)),
                            typeinfo.ImmOperator:$src), addr:$dst),
             (implicit EFLAGS)]>;
// BinOpMI_RMW_FF - Instructions like "adc [mem], imm".
class BinOpMI_RMW_FF<string mnemonic, Cse523TypeInfo typeinfo,
                  SDNode opnode, Format f>
  : BinOpMI<mnemonic, typeinfo, f,
            [(store (opnode (typeinfo.VT (load addr:$dst)),
                            typeinfo.ImmOperator:$src, EFLAGS), addr:$dst),
             (implicit EFLAGS)], 0x80, IIC_BIN_CARRY_MEM>;

// BinOpMI_F - Instructions like "cmp [mem], imm".
class BinOpMI_F<string mnemonic, Cse523TypeInfo typeinfo,
                SDPatternOperator opnode, Format f, bits<8> opcode = 0x80>
  : BinOpMI<mnemonic, typeinfo, f,
            [(set EFLAGS, (opnode (typeinfo.VT (load addr:$dst)),
                                               typeinfo.ImmOperator:$src))],
            opcode>;

// BinOpMI8 - Instructions like "add [mem], imm8".
class BinOpMI8<string mnemonic, Cse523TypeInfo typeinfo,
               Format f, list<dag> pattern,
               InstrItinClass itin = IIC_BIN_MEM>
  : ITy<0x82, f, typeinfo,
        (outs), (ins typeinfo.MemOperand:$dst, typeinfo.Imm8Operand:$src),
        mnemonic, "{$src, $dst|$dst, $src}", pattern, itin>,
    Sched<[WriteALULd, WriteRMW]> {
  let ImmT = Imm8; // Always 8-bit immediate.
}

// BinOpMI8_RMW - Instructions like "add [mem], imm8".
class BinOpMI8_RMW<string mnemonic, Cse523TypeInfo typeinfo,
                   SDNode opnode, Format f>
  : BinOpMI8<mnemonic, typeinfo, f,
             [(store (opnode (load addr:$dst),
                             typeinfo.Imm8Operator:$src), addr:$dst),
              (implicit EFLAGS)]>;

// BinOpMI8_RMW_FF - Instructions like "adc [mem], imm8".
class BinOpMI8_RMW_FF<string mnemonic, Cse523TypeInfo typeinfo,
                   SDNode opnode, Format f>
  : BinOpMI8<mnemonic, typeinfo, f,
             [(store (opnode (load addr:$dst),
                             typeinfo.Imm8Operator:$src, EFLAGS), addr:$dst),
              (implicit EFLAGS)], IIC_BIN_CARRY_MEM>;

// BinOpMI8_F - Instructions like "cmp [mem], imm8".
class BinOpMI8_F<string mnemonic, Cse523TypeInfo typeinfo,
                 SDNode opnode, Format f>
  : BinOpMI8<mnemonic, typeinfo, f,
             [(set EFLAGS, (opnode (load addr:$dst),
                                   typeinfo.Imm8Operator:$src))]>;

// BinOpAI - Instructions like "add %eax, %eax, imm", that imp-def EFLAGS.
class BinOpAI<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
              Register areg, string operands,
              InstrItinClass itin = IIC_BIN_NONMEM>
  : ITy<opcode, RawFrm, typeinfo,
        (outs), (ins typeinfo.ImmOperand:$src),
        mnemonic, operands, [], itin>, Sched<[WriteALU]> {
  let ImmT = typeinfo.ImmEncoding;
  let Uses = [areg];
  let Defs = [areg, EFLAGS];
  let hasSideEffects = 0;
}

// BinOpAI_FF - Instructions like "adc %eax, %eax, imm", that implicitly define
// and use EFLAGS.
class BinOpAI_FF<bits<8> opcode, string mnemonic, Cse523TypeInfo typeinfo,
                Register areg, string operands>
  : BinOpAI<opcode, mnemonic, typeinfo, areg, operands,
            IIC_BIN_CARRY_NONMEM> {
  let Uses = [areg, EFLAGS];
}

/// ArithBinOp_RF - This is an arithmetic binary operator where the pattern is
/// defined with "(set GPR:$dst, EFLAGS, (...".
///
/// It would be nice to get rid of the second and third argument here, but
/// tblgen can't handle dependent type references aggressively enough: PR8330
multiclass ArithBinOp_RF<bits<8> BaseOpc, bits<8> BaseOpc2, bits<8> BaseOpc4,
                         string mnemonic, Format RegMRM, Format MemMRM,
                         SDNode opnodeflag, SDNode opnode,
                         bit CommutableRR, bit ConvertibleToThreeAddress> {
  let Defs = [EFLAGS] in {
    let Constraints = "$src1 = $dst" in {
      let isCommutable = CommutableRR,
          isConvertibleToThreeAddress = ConvertibleToThreeAddress in {
        def NAME#64rr : BinOpRR_RF<BaseOpc, mnemonic, Xi64, opnodeflag>;
      } // isCommutable

      def NAME#64rr_REV : BinOpRR_Rev<BaseOpc2, mnemonic, Xi64>;

      def NAME#64rm  : BinOpRM_RF<BaseOpc2, mnemonic, Xi64, opnodeflag>;

      let isConvertibleToThreeAddress = ConvertibleToThreeAddress in {
        // NOTE: These are order specific, we want the ri8 forms to be listed
        def NAME#64ri32: BinOpRI_RF<0x80, mnemonic, Xi64, opnodeflag, RegMRM>;
      }
    } // Constraints = "$src1 = $dst"

    def NAME#64mr   : BinOpMR_RMW<BaseOpc, mnemonic, Xi64, opnode>;

    // NOTE: These are order specific, we want the mi8 forms to be listed
    // first so that they are slightly preferred to the mi forms.
    def NAME#64mi32 : BinOpMI_RMW<mnemonic, Xi64, opnode, MemMRM>;
  } // Defs = [EFLAGS]

  def NAME#64i32 : BinOpAI<BaseOpc4, mnemonic, Xi64, RAX,
                           "{$src, %rax|rax, $src}">;
}

/// ArithBinOp_RFF - This is an arithmetic binary operator where the pattern is
/// defined with "(set GPR:$dst, EFLAGS, (node LHS, RHS, EFLAGS))" like ADC and
/// SBB.
///
/// It would be nice to get rid of the second and third argument here, but
/// tblgen can't handle dependent type references aggressively enough: PR8330
multiclass ArithBinOp_RFF<bits<8> BaseOpc, bits<8> BaseOpc2, bits<8> BaseOpc4,
                          string mnemonic, Format RegMRM, Format MemMRM,
                          SDNode opnode, bit CommutableRR,
                           bit ConvertibleToThreeAddress> {
  let Uses = [EFLAGS], Defs = [EFLAGS] in {
    let Constraints = "$src1 = $dst" in {
      let isCommutable = CommutableRR,
          isConvertibleToThreeAddress = ConvertibleToThreeAddress in {
        def NAME#64rr : BinOpRR_RFF<BaseOpc, mnemonic, Xi64, opnode>;
      } // isCommutable

      def NAME#64rr_REV : BinOpRR_RFF_Rev<BaseOpc2, mnemonic, Xi64>;

      def NAME#64rm  : BinOpRM_RFF<BaseOpc2, mnemonic, Xi64, opnode>;
    } // Constraints = "$src1 = $dst"

    def NAME#64mr   : BinOpMR_RMW_FF<BaseOpc, mnemonic, Xi64, opnode>;
  } // Uses = [EFLAGS], Defs = [EFLAGS]

  def NAME#64i32 : BinOpAI_FF<BaseOpc4, mnemonic, Xi64, RAX,
                              "{$src, %rax|rax, $src}">;
}

/// ArithBinOp_F - This is an arithmetic binary operator where the pattern is
/// defined with "(set EFLAGS, (...".  It would be really nice to find a way
/// to factor this with the other ArithBinOp_*.
///
multiclass ArithBinOp_F<bits<8> BaseOpc, bits<8> BaseOpc2, bits<8> BaseOpc4,
                        string mnemonic, Format RegMRM, Format MemMRM,
                        SDNode opnode,
                        bit CommutableRR, bit ConvertibleToThreeAddress> {
  let Defs = [EFLAGS] in {
    let isCommutable = CommutableRR,
        isConvertibleToThreeAddress = ConvertibleToThreeAddress in {
      def NAME#64rr : BinOpRR_F<BaseOpc, mnemonic, Xi64, opnode>;
    } // isCommutable

    def NAME#64rr_REV : BinOpRR_F_Rev<BaseOpc2, mnemonic, Xi64>;

    def NAME#64rm  : BinOpRM_F<BaseOpc2, mnemonic, Xi64, opnode>;

    let isConvertibleToThreeAddress = ConvertibleToThreeAddress in {
      // NOTE: These are order specific, we want the ri8 forms to be listed
      // first so that they are slightly preferred to the ri forms.

      def NAME#64ri32: BinOpRI_F<0x80, mnemonic, Xi64, opnode, RegMRM>;
    }

    def NAME#64mr   : BinOpMR_F<BaseOpc, mnemonic, Xi64, opnode>;

    // NOTE: These are order specific, we want the mi8 forms to be listed
    // first so that they are slightly preferred to the mi forms.

    def NAME#64mi32 : BinOpMI_F<mnemonic, Xi64, opnode, MemMRM>;
  } // Defs = [EFLAGS]

  def NAME#64i32 : BinOpAI<BaseOpc4, mnemonic, Xi64, RAX,
                           "{$src, %rax|rax, $src}">;
}


defm AND : ArithBinOp_RF<0x20, 0x22, 0x24, "and", MRM4r, MRM4m,
                         Cse523and_flag, and, 1, 0>;
defm OR  : ArithBinOp_RF<0x08, 0x0A, 0x0C, "or", MRM1r, MRM1m,
                         Cse523or_flag, or, 1, 0>;
defm XOR : ArithBinOp_RF<0x30, 0x32, 0x34, "xor", MRM6r, MRM6m,
                         Cse523xor_flag, xor, 1, 0>;
defm ADD : ArithBinOp_RF<0x00, 0x02, 0x04, "add", MRM0r, MRM0m,
                         Cse523add_flag, add, 1, 1>;
let isCompare = 1 in {
defm SUB : ArithBinOp_RF<0x28, 0x2A, 0x2C, "sub", MRM5r, MRM5m,
                         Cse523sub_flag, sub, 0, 0>;
}

// Arithmetic.
defm ADC : ArithBinOp_RFF<0x10, 0x12, 0x14, "adc", MRM2r, MRM2m, Cse523adc_flag,
                          1, 0>;
defm SBB : ArithBinOp_RFF<0x18, 0x1A, 0x1C, "sbb", MRM3r, MRM3m, Cse523sbb_flag,
                          0, 0>;

let isCompare = 1 in {
defm CMP : ArithBinOp_F<0x38, 0x3A, 0x3C, "cmp", MRM7r, MRM7m, Cse523cmp, 0, 0>;
}


//===----------------------------------------------------------------------===//
// Semantically, test instructions are similar like AND, except they don't
// generate a result.  From an encoding perspective, they are very different:
// they don't have all the usual imm8 and REV forms, and are encoded into a
// different space.
def Cse523testpat : PatFrag<(ops node:$lhs, node:$rhs),
                         (Cse523cmp (and_su node:$lhs, node:$rhs), 0)>;

let isCompare = 1 in {
  let Defs = [EFLAGS] in {
    let isCommutable = 1 in {
      def TEST64rr : BinOpRR_F<0x84, "test", Xi64, Cse523testpat, MRMSrcReg>;
    } // isCommutable

    def TEST64rm   : BinOpRM_F<0x84, "test", Xi64, Cse523testpat>;

    def TEST64ri32 : BinOpRI_F<0xF6, "test", Xi64, Cse523testpat, MRM0r>;

    def TEST64mi32 : BinOpMI_F<"test", Xi64, Cse523testpat, MRM0m, 0xF6>;

  } // Defs = [EFLAGS]

  def TEST64i32  : BinOpAI<0xA8, "test", Xi64, RAX,
                           "{$src, %rax|rax, $src}">;
} // isCompare

//===----------------------------------------------------------------------===//
// ANDN Instruction
//
multiclass bmi_andn<string mnemonic, RegisterClass RC, Cse523MemOperand cse523memop,
                    PatFrag ld_frag> {
  def rr : I<0xF2, MRMSrcReg, (outs RC:$dst), (ins RC:$src1, RC:$src2),
            !strconcat(mnemonic, "\t{$src2, $src1, $dst|$dst, $src1, $src2}"),
            [(set RC:$dst, EFLAGS, (Cse523and_flag (not RC:$src1), RC:$src2))],
            IIC_BIN_NONMEM>, Sched<[WriteALU]>;
  def rm : I<0xF2, MRMSrcMem, (outs RC:$dst), (ins RC:$src1, cse523memop:$src2),
            !strconcat(mnemonic, "\t{$src2, $src1, $dst|$dst, $src1, $src2}"),
            [(set RC:$dst, EFLAGS,
             (Cse523and_flag (not RC:$src1), (ld_frag addr:$src2)))], IIC_BIN_MEM>,
           Sched<[WriteALULd, ReadAfterLd]>;
}

let Predicates = [HasBMI], Defs = [EFLAGS] in {
  defm ANDN64 : bmi_andn<"andn{q}", GR64, i64mem, loadi64>, T8PS, VEX_4V, VEX_W;
}

let Predicates = [HasBMI] in {
  def : Pat<(and (not GR64:$src1), (loadi64 addr:$src2)),
            (ANDN64rm GR64:$src1, addr:$src2)>;
}

//===----------------------------------------------------------------------===//
// MULX Instruction
//
multiclass bmi_mulx<string mnemonic, RegisterClass RC, Cse523MemOperand cse523memop> {
let neverHasSideEffects = 1 in {
  let isCommutable = 1 in
  def rr : I<0xF6, MRMSrcReg, (outs RC:$dst1, RC:$dst2), (ins RC:$src),
             !strconcat(mnemonic, "\t{$src, $dst2, $dst1|$dst1, $dst2, $src}"),
             [], IIC_MUL8>, T8XD, VEX_4V, Sched<[WriteIMul, WriteIMulH]>;

  let mayLoad = 1 in
  def rm : I<0xF6, MRMSrcMem, (outs RC:$dst1, RC:$dst2), (ins cse523memop:$src),
             !strconcat(mnemonic, "\t{$src, $dst2, $dst1|$dst1, $dst2, $src}"),
             [], IIC_MUL8>, T8XD, VEX_4V, Sched<[WriteIMulLd, WriteIMulH]>;
}
}

let Predicates = [HasBMI2] in {
  let Uses = [RDX] in
    defm MULX64 : bmi_mulx<"mulx{q}", GR64, i64mem>, VEX_W;
}

//===----------------------------------------------------------------------===//
// ADCX Instruction
//
let hasSideEffects = 0, Predicates = [HasADX], Defs = [EFLAGS] in {
  let SchedRW = [WriteALU] in {
  def ADCX64rr : RI<0xF6, MRMSrcReg, (outs GR64:$dst), (ins GR64:$src),
             "adcx{q}\t{$src, $dst|$dst, $src}",
             [], IIC_BIN_NONMEM>, T8PD, Requires<[In64BitMode]>;
  } // SchedRW

  let mayLoad = 1, SchedRW = [WriteALULd] in {
  def ADCX64rm : RI<0xF6, MRMSrcMem, (outs GR64:$dst), (ins i64mem:$src),
             "adcx{q}\t{$src, $dst|$dst, $src}",
             [], IIC_BIN_MEM>, T8PD, Requires<[In64BitMode]>;
  }
}

//===----------------------------------------------------------------------===//
// ADOX Instruction
//
let hasSideEffects = 0, Predicates = [HasADX], Defs = [EFLAGS] in {
  let SchedRW = [WriteALU] in {
  def ADOX64rr : RI<0xF6, MRMSrcReg, (outs GR64:$dst), (ins GR64:$src),
             "adox{q}\t{$src, $dst|$dst, $src}",
             [], IIC_BIN_NONMEM>, T8XS, Requires<[In64BitMode]>;
  } // SchedRW

  let mayLoad = 1, SchedRW = [WriteALULd] in {
  def ADOX64rm : RI<0xF6, MRMSrcMem, (outs GR64:$dst), (ins i64mem:$src),
             "adox{q}\t{$src, $dst|$dst, $src}",
             [], IIC_BIN_MEM>, T8XS, Requires<[In64BitMode]>;
  }
}
