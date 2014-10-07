//===-- Cse523InstrInfo.td - Main Cse523 Instruction Definition --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the Cse523 instruction set, defining the instructions, and
// properties of the instructions which are needed for code generation, machine
// code emission, and analysis.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Cse523 specific DAG Nodes.
//

def SDTIntShiftDOp: SDTypeProfile<1, 3,
                                  [SDTCisSameAs<0, 1>, SDTCisSameAs<0, 2>,
                                   SDTCisInt<0>, SDTCisInt<3>]>;

def SDTCse523CmpTest : SDTypeProfile<1, 2, [SDTCisVT<0, i64>, SDTCisSameAs<1, 2>]>;

def SDTCse523Cmps : SDTypeProfile<1, 3, [SDTCisFP<0>, SDTCisSameAs<1, 2>, SDTCisVT<3, i8>]>;

def SDTCse523Cmov    : SDTypeProfile<1, 4,
                                  [SDTCisSameAs<0, 1>, SDTCisSameAs<1, 2>,
                                   SDTCisVT<3, i8>, SDTCisVT<4, i64>]>;

// Unary and binary operator instructions that set EFLAGS as a side-effect.
def SDTUnaryArithWithFlags : SDTypeProfile<2, 1,
                                           [SDTCisInt<0>, SDTCisVT<1, i64>]>;

def SDTBinaryArithWithFlags : SDTypeProfile<2, 2,
                                            [SDTCisSameAs<0, 2>,
                                             SDTCisSameAs<0, 3>,
                                             SDTCisInt<0>, SDTCisVT<1, i64>]>;

// SDTBinaryArithWithFlagsInOut - RES1, EFLAGS = op LHS, RHS, EFLAGS
def SDTBinaryArithWithFlagsInOut : SDTypeProfile<2, 3,
                                            [SDTCisSameAs<0, 2>,
                                             SDTCisSameAs<0, 3>,
                                             SDTCisInt<0>,
                                             SDTCisVT<1, i64>,
                                             SDTCisVT<4, i64>]>;
// RES1, RES2, FLAGS = op LHS, RHS
def SDT2ResultBinaryArithWithFlags : SDTypeProfile<3, 2,
                                            [SDTCisSameAs<0, 1>,
                                             SDTCisSameAs<0, 2>,
                                             SDTCisSameAs<0, 3>,
                                             SDTCisInt<0>, SDTCisVT<1, i64>]>;
def SDTCse523BrCond  : SDTypeProfile<0, 3,
                                  [SDTCisVT<0, OtherVT>,
                                   SDTCisVT<1, i8>, SDTCisVT<2, i64>]>;

def SDTCse523SetCC   : SDTypeProfile<1, 2,
                                  [SDTCisVT<0, i64>,
                                   SDTCisVT<1, i8>, SDTCisVT<2, i64>]>;
def SDTCse523SetCC_C : SDTypeProfile<1, 2,
                                  [SDTCisInt<0>,
                                   SDTCisVT<1, i8>, SDTCisVT<2, i64>]>;

def SDTCse523sahf : SDTypeProfile<1, 1, [SDTCisVT<0, i32>, SDTCisVT<1, i8>]>;

def SDTCse523rdrand : SDTypeProfile<2, 0, [SDTCisInt<0>, SDTCisVT<1, i32>]>;

def SDTCse523cas : SDTypeProfile<0, 3, [SDTCisPtrTy<0>, SDTCisInt<1>,
                                     SDTCisVT<2, i8>]>;
def SDTCse523caspair : SDTypeProfile<0, 1, [SDTCisPtrTy<0>]>;

def SDTCse523atomicBinary : SDTypeProfile<2, 3, [SDTCisInt<0>, SDTCisInt<1>,
                                SDTCisPtrTy<2>, SDTCisInt<3>,SDTCisInt<4>]>;
def SDTCse523Ret     : SDTypeProfile<0, -1, [SDTCisVT<0, i16>]>;

def SDT_Cse523CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>]>;
def SDT_Cse523CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>,
                                        SDTCisVT<1, i32>]>;

def SDT_Cse523Call   : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;

def SDT_Cse523VASTART_SAVE_XMM_REGS : SDTypeProfile<0, -1, [SDTCisVT<0, i8>,
                                                         SDTCisVT<1, iPTR>,
                                                         SDTCisVT<2, iPTR>]>;

def SDT_Cse523VAARG_64 : SDTypeProfile<1, -1, [SDTCisPtrTy<0>,
                                            SDTCisPtrTy<1>,
                                            SDTCisVT<2, i32>,
                                            SDTCisVT<3, i8>,
                                            SDTCisVT<4, i32>]>;

def SDTCse523RepStr  : SDTypeProfile<0, 1, [SDTCisVT<0, OtherVT>]>;

def SDTCse523Void    : SDTypeProfile<0, 0, []>;

def SDTCse523Wrapper : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>, SDTCisPtrTy<0>]>;

def SDT_Cse523TLSADDR : SDTypeProfile<0, 1, [SDTCisInt<0>]>;

def SDT_Cse523TLSBASEADDR : SDTypeProfile<0, 1, [SDTCisInt<0>]>;

def SDT_Cse523TLSCALL : SDTypeProfile<0, 1, [SDTCisInt<0>]>;

def SDT_Cse523SEG_ALLOCA : SDTypeProfile<1, 1, [SDTCisVT<0, iPTR>, SDTCisVT<1, iPTR>]>;

def SDT_Cse523WIN_FTOL : SDTypeProfile<0, 1, [SDTCisFP<0>]>;

def SDT_Cse523EHRET : SDTypeProfile<0, 1, [SDTCisInt<0>]>;

def SDT_Cse523TCRET : SDTypeProfile<0, 2, [SDTCisPtrTy<0>, SDTCisVT<1, i32>]>;

def SDT_Cse523MEMBARRIER : SDTypeProfile<0, 0, []>;

def Cse523MemBarrier : SDNode<"Cse523ISD::MEMBARRIER", SDT_Cse523MEMBARRIER,
                            [SDNPHasChain,SDNPSideEffect]>;
def Cse523MFence : SDNode<"Cse523ISD::MFENCE", SDT_Cse523MEMBARRIER,
                        [SDNPHasChain]>;
def Cse523SFence : SDNode<"Cse523ISD::SFENCE", SDT_Cse523MEMBARRIER,
                        [SDNPHasChain]>;
def Cse523LFence : SDNode<"Cse523ISD::LFENCE", SDT_Cse523MEMBARRIER,
                        [SDNPHasChain]>;


def Cse523bsf     : SDNode<"Cse523ISD::BSF",      SDTUnaryArithWithFlags>;
def Cse523bsr     : SDNode<"Cse523ISD::BSR",      SDTUnaryArithWithFlags>;
def Cse523shld    : SDNode<"Cse523ISD::SHLD",     SDTIntShiftDOp>;
def Cse523shrd    : SDNode<"Cse523ISD::SHRD",     SDTIntShiftDOp>;

def Cse523cmp     : SDNode<"Cse523ISD::CMP" ,     SDTCse523CmpTest>;

def Cse523cmov    : SDNode<"Cse523ISD::CMOV",     SDTCse523Cmov>;
def Cse523brcond  : SDNode<"Cse523ISD::BRCOND",   SDTCse523BrCond,
                        [SDNPHasChain]>;
def Cse523setcc   : SDNode<"Cse523ISD::SETCC",    SDTCse523SetCC>;
def Cse523setcc_c : SDNode<"Cse523ISD::SETCC_CARRY", SDTCse523SetCC_C>;

def Cse523sahf    : SDNode<"Cse523ISD::SAHF",     SDTCse523sahf>;

def Cse523rdrand  : SDNode<"Cse523ISD::RDRAND",   SDTCse523rdrand,
                        [SDNPHasChain, SDNPSideEffect]>;

def Cse523rdseed  : SDNode<"Cse523ISD::RDSEED",   SDTCse523rdrand,
                        [SDNPHasChain, SDNPSideEffect]>;

def Cse523cas : SDNode<"Cse523ISD::LCMPXCHG_DAG", SDTCse523cas,
                        [SDNPHasChain, SDNPInGlue, SDNPOutGlue, SDNPMayStore,
                         SDNPMayLoad, SDNPMemOperand]>;
def Cse523cas8 : SDNode<"Cse523ISD::LCMPXCHG8_DAG", SDTCse523caspair,
                        [SDNPHasChain, SDNPInGlue, SDNPOutGlue, SDNPMayStore,
                         SDNPMayLoad, SDNPMemOperand]>;
def Cse523cas16 : SDNode<"Cse523ISD::LCMPXCHG16_DAG", SDTCse523caspair,
                        [SDNPHasChain, SDNPInGlue, SDNPOutGlue, SDNPMayStore,
                         SDNPMayLoad, SDNPMemOperand]>;

def Cse523AtomAdd64 : SDNode<"Cse523ISD::ATOMADD64_DAG", SDTCse523atomicBinary,
                        [SDNPHasChain, SDNPMayStore,
                         SDNPMayLoad, SDNPMemOperand]>;
def Cse523AtomSub64 : SDNode<"Cse523ISD::ATOMSUB64_DAG", SDTCse523atomicBinary,
                        [SDNPHasChain, SDNPMayStore,
                         SDNPMayLoad, SDNPMemOperand]>;
def Cse523AtomOr64 : SDNode<"Cse523ISD::ATOMOR64_DAG", SDTCse523atomicBinary,
                        [SDNPHasChain, SDNPMayStore,
                         SDNPMayLoad, SDNPMemOperand]>;
def Cse523AtomXor64 : SDNode<"Cse523ISD::ATOMXOR64_DAG", SDTCse523atomicBinary,
                        [SDNPHasChain, SDNPMayStore,
                         SDNPMayLoad, SDNPMemOperand]>;
def Cse523AtomAnd64 : SDNode<"Cse523ISD::ATOMAND64_DAG", SDTCse523atomicBinary,
                        [SDNPHasChain, SDNPMayStore,
                         SDNPMayLoad, SDNPMemOperand]>;
def Cse523AtomNand64 : SDNode<"Cse523ISD::ATOMNAND64_DAG", SDTCse523atomicBinary,
                        [SDNPHasChain, SDNPMayStore,
                         SDNPMayLoad, SDNPMemOperand]>;
def Cse523AtomSwap64 : SDNode<"Cse523ISD::ATOMSWAP64_DAG", SDTCse523atomicBinary,
                        [SDNPHasChain, SDNPMayStore,
                         SDNPMayLoad, SDNPMemOperand]>;
def Cse523retflag : SDNode<"Cse523ISD::RET_FLAG", SDTCse523Ret,
                        [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def Cse523vastart_save_xmm_regs :
                 SDNode<"Cse523ISD::VASTART_SAVE_XMM_REGS",
                        SDT_Cse523VASTART_SAVE_XMM_REGS,
                        [SDNPHasChain, SDNPVariadic]>;
def Cse523vaarg64 :
                 SDNode<"Cse523ISD::VAARG_64", SDT_Cse523VAARG_64,
                        [SDNPHasChain, SDNPMayLoad, SDNPMayStore,
                         SDNPMemOperand]>;
def Cse523callseq_start :
                 SDNode<"ISD::CALLSEQ_START", SDT_Cse523CallSeqStart,
                        [SDNPHasChain, SDNPOutGlue]>;
def Cse523callseq_end :
                 SDNode<"ISD::CALLSEQ_END",   SDT_Cse523CallSeqEnd,
                        [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def Cse523call    : SDNode<"Cse523ISD::CALL",     SDT_Cse523Call,
                        [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue,
                         SDNPVariadic]>;

def Cse523rep_stos: SDNode<"Cse523ISD::REP_STOS", SDTCse523RepStr,
                        [SDNPHasChain, SDNPInGlue, SDNPOutGlue, SDNPMayStore]>;
def Cse523rep_movs: SDNode<"Cse523ISD::REP_MOVS", SDTCse523RepStr,
                        [SDNPHasChain, SDNPInGlue, SDNPOutGlue, SDNPMayStore,
                         SDNPMayLoad]>;

def Cse523rdtsc   : SDNode<"Cse523ISD::RDTSC_DAG", SDTCse523Void,
                        [SDNPHasChain, SDNPOutGlue, SDNPSideEffect]>;

def Cse523Wrapper    : SDNode<"Cse523ISD::Wrapper",     SDTCse523Wrapper>;
def Cse523WrapperRIP : SDNode<"Cse523ISD::WrapperRIP",  SDTCse523Wrapper>;

def Cse523tlsaddr : SDNode<"Cse523ISD::TLSADDR", SDT_Cse523TLSADDR,
                        [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def Cse523tlsbaseaddr : SDNode<"Cse523ISD::TLSBASEADDR", SDT_Cse523TLSBASEADDR,
                        [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def Cse523ehret : SDNode<"Cse523ISD::EH_RETURN", SDT_Cse523EHRET,
                        [SDNPHasChain]>;

def Cse523eh_sjlj_setjmp  : SDNode<"Cse523ISD::EH_SJLJ_SETJMP",
                                SDTypeProfile<1, 1, [SDTCisInt<0>,
                                                     SDTCisPtrTy<1>]>,
                                [SDNPHasChain, SDNPSideEffect]>;
def Cse523eh_sjlj_longjmp : SDNode<"Cse523ISD::EH_SJLJ_LONGJMP",
                                SDTypeProfile<0, 1, [SDTCisPtrTy<0>]>,
                                [SDNPHasChain, SDNPSideEffect]>;

def Cse523tcret : SDNode<"Cse523ISD::TC_RETURN", SDT_Cse523TCRET,
                        [SDNPHasChain,  SDNPOptInGlue, SDNPVariadic]>;

def Cse523add_flag  : SDNode<"Cse523ISD::ADD",  SDTBinaryArithWithFlags,
                          [SDNPCommutative]>;
def Cse523sub_flag  : SDNode<"Cse523ISD::SUB",  SDTBinaryArithWithFlags>;
def Cse523smul_flag : SDNode<"Cse523ISD::SMUL", SDTBinaryArithWithFlags,
                          [SDNPCommutative]>;
def Cse523umul_flag : SDNode<"Cse523ISD::UMUL", SDT2ResultBinaryArithWithFlags,
                          [SDNPCommutative]>;
def Cse523adc_flag  : SDNode<"Cse523ISD::ADC",  SDTBinaryArithWithFlagsInOut>;
def Cse523sbb_flag  : SDNode<"Cse523ISD::SBB",  SDTBinaryArithWithFlagsInOut>;

def Cse523inc_flag  : SDNode<"Cse523ISD::INC",  SDTUnaryArithWithFlags>;
def Cse523dec_flag  : SDNode<"Cse523ISD::DEC",  SDTUnaryArithWithFlags>;
def Cse523or_flag   : SDNode<"Cse523ISD::OR",   SDTBinaryArithWithFlags,
                          [SDNPCommutative]>;
def Cse523xor_flag  : SDNode<"Cse523ISD::XOR",  SDTBinaryArithWithFlags,
                          [SDNPCommutative]>;
def Cse523and_flag  : SDNode<"Cse523ISD::AND",  SDTBinaryArithWithFlags,
                          [SDNPCommutative]>;

def Cse523bzhi   : SDNode<"Cse523ISD::BZHI",   SDTIntShiftOp>;
def Cse523bextr  : SDNode<"Cse523ISD::BEXTR",  SDTIntBinOp>;

def Cse523mul_imm : SDNode<"Cse523ISD::MUL_IMM", SDTIntBinOp>;

def Cse523WinAlloca : SDNode<"Cse523ISD::WIN_ALLOCA", SDTCse523Void,
                          [SDNPHasChain, SDNPInGlue, SDNPOutGlue]>;

def Cse523SegAlloca : SDNode<"Cse523ISD::SEG_ALLOCA", SDT_Cse523SEG_ALLOCA,
                          [SDNPHasChain]>;

def Cse523TLSCall : SDNode<"Cse523ISD::TLSCALL", SDT_Cse523TLSCALL,
                        [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def Cse523WinFTOL : SDNode<"Cse523ISD::WIN_FTOL", SDT_Cse523WIN_FTOL,
                        [SDNPHasChain, SDNPOutGlue]>;

//===----------------------------------------------------------------------===//
// Cse523 Operand Definitions.
//

// A version of ptr_rc which excludes SP, ESP, and RSP. This is used for
// the index operand of an address, to conform to cse523 encoding restrictions.
def ptr_rc_nosp : PointerLikeRegClass<1>;

// *mem - Operand definitions for the funky Cse523 addressing mode operands.
//
def Cse523MemAsmOperand : AsmOperandClass {
 let Name = "Mem";
}
def Cse523Mem8AsmOperand : AsmOperandClass {
  let Name = "Mem8"; let RenderMethod = "addMemOperands";
}
def Cse523Mem16AsmOperand : AsmOperandClass {
  let Name = "Mem16"; let RenderMethod = "addMemOperands";
}
def Cse523Mem32AsmOperand : AsmOperandClass {
  let Name = "Mem32"; let RenderMethod = "addMemOperands";
}
def Cse523Mem64AsmOperand : AsmOperandClass {
  let Name = "Mem64"; let RenderMethod = "addMemOperands";
}
def Cse523Mem80AsmOperand : AsmOperandClass {
  let Name = "Mem80"; let RenderMethod = "addMemOperands";
}
def Cse523Mem128AsmOperand : AsmOperandClass {
  let Name = "Mem128"; let RenderMethod = "addMemOperands";
}
def Cse523Mem256AsmOperand : AsmOperandClass {
  let Name = "Mem256"; let RenderMethod = "addMemOperands";
}
def Cse523Mem512AsmOperand : AsmOperandClass {
  let Name = "Mem512"; let RenderMethod = "addMemOperands";
}

// Gather mem operands
def Cse523MemVX32Operand : AsmOperandClass {
  let Name = "MemVX32"; let RenderMethod = "addMemOperands";
}
def Cse523MemVY32Operand : AsmOperandClass {
  let Name = "MemVY32"; let RenderMethod = "addMemOperands";
}
def Cse523MemVZ32Operand : AsmOperandClass {
  let Name = "MemVZ32"; let RenderMethod = "addMemOperands";
}
def Cse523MemVX64Operand : AsmOperandClass {
  let Name = "MemVX64"; let RenderMethod = "addMemOperands";
}
def Cse523MemVY64Operand : AsmOperandClass {
  let Name = "MemVY64"; let RenderMethod = "addMemOperands";
}
def Cse523MemVZ64Operand : AsmOperandClass {
  let Name = "MemVZ64"; let RenderMethod = "addMemOperands";
}

def Cse523AbsMemAsmOperand : AsmOperandClass {
  let Name = "AbsMem";
  let SuperClasses = [Cse523MemAsmOperand];
}
class Cse523MemOperand<string printMethod> : Operand<iPTR> {
  let PrintMethod = printMethod;
  let MIOperandInfo = (ops ptr_rc, i8imm, ptr_rc_nosp, i32imm, i8imm);
  let ParserMatchClass = Cse523MemAsmOperand;
}

let OperandType = "OPERAND_MEMORY" in {
def opaque32mem : Cse523MemOperand<"printopaquemem">;
def opaque48mem : Cse523MemOperand<"printopaquemem">;
def opaque80mem : Cse523MemOperand<"printopaquemem">;
def opaque512mem : Cse523MemOperand<"printopaquemem">;

def i8mem   : Cse523MemOperand<"printi8mem"> {
  let ParserMatchClass = Cse523Mem8AsmOperand; }
def i16mem  : Cse523MemOperand<"printi16mem"> {
  let ParserMatchClass = Cse523Mem16AsmOperand; }
def i32mem  : Cse523MemOperand<"printi32mem"> {
  let ParserMatchClass = Cse523Mem32AsmOperand; }
def i64mem  : Cse523MemOperand<"printi64mem"> {
  let ParserMatchClass = Cse523Mem64AsmOperand; }
}

// A version of i8mem for use on cse523-64 that uses GR64_NOREX instead of
// plain GR64, so that it doesn't potentially require a REX prefix.
def i8mem_NOREX : Operand<i64> {
  let PrintMethod = "printi8mem";
  let MIOperandInfo = (ops GR64_NOREX, i8imm, GR64_NOREX_NOSP, i32imm, i8imm);
  let ParserMatchClass = Cse523Mem8AsmOperand;
  let OperandType = "OPERAND_MEMORY";
}

// GPRs available for tailcall.
// It represents GR32_TC, GR64_TC or GR64_TCW64.
def ptr_rc_tailcall : PointerLikeRegClass<2>;

// Special i32mem for addresses of load folding tail calls. These are not
// allowed to use callee-saved registers since they must be scheduled
// after callee-saved register are popped.
def i32mem_TC : Operand<i32> {
  let PrintMethod = "printi32mem";
  let MIOperandInfo = (ops ptr_rc_tailcall, i8imm, ptr_rc_tailcall,
                       i32imm, i8imm);
  let ParserMatchClass = Cse523Mem32AsmOperand;
  let OperandType = "OPERAND_MEMORY";
}

// Special i64mem for addresses of load folding tail calls. These are not
// allowed to use callee-saved registers since they must be scheduled
// after callee-saved register are popped.
def i64mem_TC : Operand<i64> {
  let PrintMethod = "printi64mem";
  let MIOperandInfo = (ops ptr_rc_tailcall, i8imm,
                       ptr_rc_tailcall, i32imm, i8imm);
  let ParserMatchClass = Cse523Mem64AsmOperand;
  let OperandType = "OPERAND_MEMORY";
}

let OperandType = "OPERAND_PCREL",
    ParserMatchClass = Cse523AbsMemAsmOperand,
    PrintMethod = "printPCRelImm" in {
def i32imm_pcrel : Operand<i32>;
def i16imm_pcrel : Operand<i16>;

// Branch targets have OtherVT type and print as pc-relative values.
def brtarget : Operand<OtherVT>;
def brtarget8 : Operand<OtherVT>;

}

def Cse523SrcIdx8Operand : AsmOperandClass {
  let Name = "SrcIdx8";
  let RenderMethod = "addSrcIdxOperands";
  let SuperClasses = [Cse523Mem8AsmOperand];
}
def Cse523SrcIdx16Operand : AsmOperandClass {
  let Name = "SrcIdx16";
  let RenderMethod = "addSrcIdxOperands";
  let SuperClasses = [Cse523Mem16AsmOperand];
}
def Cse523SrcIdx32Operand : AsmOperandClass {
  let Name = "SrcIdx32";
  let RenderMethod = "addSrcIdxOperands";
  let SuperClasses = [Cse523Mem32AsmOperand];
}
def Cse523SrcIdx64Operand : AsmOperandClass {
  let Name = "SrcIdx64";
  let RenderMethod = "addSrcIdxOperands";
  let SuperClasses = [Cse523Mem64AsmOperand];
}
def Cse523DstIdx8Operand : AsmOperandClass {
  let Name = "DstIdx8";
  let RenderMethod = "addDstIdxOperands";
  let SuperClasses = [Cse523Mem8AsmOperand];
}
def Cse523DstIdx16Operand : AsmOperandClass {
  let Name = "DstIdx16";
  let RenderMethod = "addDstIdxOperands";
  let SuperClasses = [Cse523Mem16AsmOperand];
}
def Cse523DstIdx32Operand : AsmOperandClass {
  let Name = "DstIdx32";
  let RenderMethod = "addDstIdxOperands";
  let SuperClasses = [Cse523Mem32AsmOperand];
}
def Cse523DstIdx64Operand : AsmOperandClass {
  let Name = "DstIdx64";
  let RenderMethod = "addDstIdxOperands";
  let SuperClasses = [Cse523Mem64AsmOperand];
}
def Cse523MemOffs8AsmOperand : AsmOperandClass {
  let Name = "MemOffs8";
  let RenderMethod = "addMemOffsOperands";
  let SuperClasses = [Cse523Mem8AsmOperand];
}
def Cse523MemOffs16AsmOperand : AsmOperandClass {
  let Name = "MemOffs16";
  let RenderMethod = "addMemOffsOperands";
  let SuperClasses = [Cse523Mem16AsmOperand];
}
def Cse523MemOffs32AsmOperand : AsmOperandClass {
  let Name = "MemOffs32";
  let RenderMethod = "addMemOffsOperands";
  let SuperClasses = [Cse523Mem32AsmOperand];
}
def Cse523MemOffs64AsmOperand : AsmOperandClass {
  let Name = "MemOffs64";
  let RenderMethod = "addMemOffsOperands";
  let SuperClasses = [Cse523Mem64AsmOperand];
}
let OperandType = "OPERAND_MEMORY" in {
def srcidx8 : Operand<iPTR> {
  let ParserMatchClass = Cse523SrcIdx8Operand;
  let MIOperandInfo = (ops ptr_rc, i8imm);
  let PrintMethod = "printSrcIdx8"; }
def srcidx16 : Operand<iPTR> {
  let ParserMatchClass = Cse523SrcIdx16Operand;
  let MIOperandInfo = (ops ptr_rc, i8imm);
  let PrintMethod = "printSrcIdx16"; }
def srcidx32 : Operand<iPTR> {
  let ParserMatchClass = Cse523SrcIdx32Operand;
  let MIOperandInfo = (ops ptr_rc, i8imm);
  let PrintMethod = "printSrcIdx32"; }
def srcidx64 : Operand<iPTR> {
  let ParserMatchClass = Cse523SrcIdx64Operand;
  let MIOperandInfo = (ops ptr_rc, i8imm);
  let PrintMethod = "printSrcIdx64"; }
def dstidx8 : Operand<iPTR> {
  let ParserMatchClass = Cse523DstIdx8Operand;
  let MIOperandInfo = (ops ptr_rc);
  let PrintMethod = "printDstIdx8"; }
def dstidx16 : Operand<iPTR> {
  let ParserMatchClass = Cse523DstIdx16Operand;
  let MIOperandInfo = (ops ptr_rc);
  let PrintMethod = "printDstIdx16"; }
def dstidx32 : Operand<iPTR> {
  let ParserMatchClass = Cse523DstIdx32Operand;
  let MIOperandInfo = (ops ptr_rc);
  let PrintMethod = "printDstIdx32"; }
def dstidx64 : Operand<iPTR> {
  let ParserMatchClass = Cse523DstIdx64Operand;
  let MIOperandInfo = (ops ptr_rc);
  let PrintMethod = "printDstIdx64"; }
def offset8 : Operand<iPTR> {
  let ParserMatchClass = Cse523MemOffs8AsmOperand;
  let MIOperandInfo = (ops i64imm, i8imm);
  let PrintMethod = "printMemOffs8"; }
def offset16 : Operand<iPTR> {
  let ParserMatchClass = Cse523MemOffs16AsmOperand;
  let MIOperandInfo = (ops i64imm, i8imm);
  let PrintMethod = "printMemOffs16"; }
def offset32 : Operand<iPTR> {
  let ParserMatchClass = Cse523MemOffs32AsmOperand;
  let MIOperandInfo = (ops i64imm, i8imm);
  let PrintMethod = "printMemOffs32"; }
def offset64 : Operand<iPTR> {
  let ParserMatchClass = Cse523MemOffs64AsmOperand;
  let MIOperandInfo = (ops i64imm, i8imm);
  let PrintMethod = "printMemOffs64"; }
}

def SSECC : Operand<i8> {
  let PrintMethod = "printSSECC";
  let OperandType = "OPERAND_IMMEDIATE";
}

class ImmSExtAsmOperandClass : AsmOperandClass {
  let SuperClasses = [ImmAsmOperand];
  let RenderMethod = "addImmOperands";
}

class ImmZExtAsmOperandClass : AsmOperandClass {
  let SuperClasses = [ImmAsmOperand];
  let RenderMethod = "addImmOperands";
}

def Cse523GR32orGR64AsmOperand : AsmOperandClass {
  let Name = "GR32orGR64";
}

def GR32orGR64 : RegisterOperand<GR64> {
  let ParserMatchClass = Cse523GR32orGR64AsmOperand;
}

// Define 8/16/32 bit registers based on 64 bit registers
def GR8 : RegisterOperand<GR64> {
  let PrintMethod = "print8ByteReg";
  let ParserMatchClass = Cse523GR32orGR64AsmOperand;
}
def GR16 : RegisterOperand<GR64> {
  let PrintMethod = "print16ByteReg";
  let ParserMatchClass = Cse523GR32orGR64AsmOperand;
}
def GR32 : RegisterOperand<GR64> {
  let PrintMethod = "print32ByteReg";
  let ParserMatchClass = Cse523GR32orGR64AsmOperand;
}

// Sign-extended immediate classes. We don't need to define the full lattice
// here because there is no instruction with an ambiguity between ImmSExti64i32
// and ImmSExti32i8.
//
// The strange ranges come from the fact that the assembler always works with
// 64-bit immediates, but for a 16-bit target value we want to accept both "-1"
// (which will be a -1ULL), and "0xFF" (-1 in 16-bits).

// [0, 0x7FFFFFFF]                                            |
//   [0xFFFFFFFF80000000, 0xFFFFFFFFFFFFFFFF]
def ImmSExti64i32AsmOperand : ImmSExtAsmOperandClass {
  let Name = "ImmSExti64i32";
}

// [0, 0x0000007F] | [0x000000000000FF80, 0x000000000000FFFF] |
//   [0xFFFFFFFFFFFFFF80, 0xFFFFFFFFFFFFFFFF]
def ImmSExti16i8AsmOperand : ImmSExtAsmOperandClass {
  let Name = "ImmSExti16i8";
  let SuperClasses = [ImmSExti64i32AsmOperand];
}

// [0, 0x0000007F] | [0x00000000FFFFFF80, 0x00000000FFFFFFFF] |
//   [0xFFFFFFFFFFFFFF80, 0xFFFFFFFFFFFFFFFF]
def ImmSExti32i8AsmOperand : ImmSExtAsmOperandClass {
  let Name = "ImmSExti32i8";
}

// [0, 0x000000FF]
def ImmZExtu32u8AsmOperand : ImmZExtAsmOperandClass {
  let Name = "ImmZExtu32u8";
}


// [0, 0x0000007F]                                            |
//   [0xFFFFFFFFFFFFFF80, 0xFFFFFFFFFFFFFFFF]
def ImmSExti64i8AsmOperand : ImmSExtAsmOperandClass {
  let Name = "ImmSExti64i8";
  let SuperClasses = [ImmSExti16i8AsmOperand, ImmSExti32i8AsmOperand,
                      ImmSExti64i32AsmOperand];
}

// A couple of more descriptive operand definitions.
// 16-bits but only 8 bits are significant.
def i16i8imm  : Operand<i16> {
  let ParserMatchClass = ImmSExti16i8AsmOperand;
  let OperandType = "OPERAND_IMMEDIATE";
}
// 32-bits but only 8 bits are significant.
def i32i8imm  : Operand<i32> {
  let ParserMatchClass = ImmSExti32i8AsmOperand;
  let OperandType = "OPERAND_IMMEDIATE";
}
// 32-bits but only 8 bits are significant, and those 8 bits are unsigned.
def u32u8imm  : Operand<i32> {
  let ParserMatchClass = ImmZExtu32u8AsmOperand;
  let OperandType = "OPERAND_IMMEDIATE";
}

// 64-bits but only 32 bits are significant.
def i64i32imm  : Operand<i64> {
  let ParserMatchClass = ImmSExti64i32AsmOperand;
  let OperandType = "OPERAND_IMMEDIATE";
}

// 64-bits but only 32 bits are significant, and those bits are treated as being
// pc relative.
def i64i32imm_pcrel : Operand<i64> {
  let PrintMethod = "printPCRelImm";
  let ParserMatchClass = Cse523AbsMemAsmOperand;
  let OperandType = "OPERAND_PCREL";
}

// 64-bits but only 8 bits are significant.
def i64i8imm   : Operand<i64> {
  let ParserMatchClass = ImmSExti64i8AsmOperand;
  let OperandType = "OPERAND_IMMEDIATE";
}

def lea64_32mem : Operand<i32> {
  let PrintMethod = "printi32mem";
  let MIOperandInfo = (ops GR64, i8imm, GR64_NOSP, i32imm, i8imm);
  let ParserMatchClass = Cse523MemAsmOperand;
}

// Memory operands that use 64-bit pointers in both ILP32 and LP64.
def lea64mem : Operand<i64> {
  let PrintMethod = "printi64mem";
  let MIOperandInfo = (ops GR64, i8imm, GR64_NOSP, i32imm, i8imm);
  let ParserMatchClass = Cse523MemAsmOperand;
}


//===----------------------------------------------------------------------===//
// Cse523 Complex Pattern Definitions.
//

// Define Cse523 specific addressing mode.
def addr      : ComplexPattern<iPTR, 5, "SelectAddr", [], [SDNPWantParent]>;
def lea32addr : ComplexPattern<i32, 5, "SelectLEAAddr",
                               [add, sub, mul, Cse523mul_imm, shl, or, frameindex],
                               []>;
// In 64-bit mode 32-bit LEAs can use RIP-relative addressing.
def lea64_32addr : ComplexPattern<i32, 5, "SelectLEA64_32Addr",
                                  [add, sub, mul, Cse523mul_imm, shl, or,
                                   frameindex, Cse523WrapperRIP],
                                  []>;

def tls32addr : ComplexPattern<i32, 5, "SelectTLSADDRAddr",
                               [tglobaltlsaddr], []>;

def tls32baseaddr : ComplexPattern<i32, 5, "SelectTLSADDRAddr",
                               [tglobaltlsaddr], []>;

def lea64addr : ComplexPattern<i64, 5, "SelectLEAAddr",
                        [add, sub, mul, Cse523mul_imm, shl, or, frameindex,
                         Cse523WrapperRIP], []>;

def tls64addr : ComplexPattern<i64, 5, "SelectTLSADDRAddr",
                               [tglobaltlsaddr], []>;

def tls64baseaddr : ComplexPattern<i64, 5, "SelectTLSADDRAddr",
                               [tglobaltlsaddr], []>;

//===----------------------------------------------------------------------===//
// Cse523 Instruction Predicate Definitions.
def HasCMov      : Predicate<"Subtarget->hasCMov()">;
def NoCMov       : Predicate<"!Subtarget->hasCMov()">;

def HasMMX       : Predicate<"Subtarget->hasMMX()">;
def Has3DNow     : Predicate<"Subtarget->has3DNow()">;
def Has3DNowA    : Predicate<"Subtarget->has3DNowA()">;
def HasSSE1      : Predicate<"Subtarget->hasSSE1()">;
def UseSSE1      : Predicate<"Subtarget->hasSSE1() && !Subtarget->hasAVX()">;
def HasSSE2      : Predicate<"Subtarget->hasSSE2()">;
def UseSSE2      : Predicate<"Subtarget->hasSSE2() && !Subtarget->hasAVX()">;
def HasSSE3      : Predicate<"Subtarget->hasSSE3()">;
def UseSSE3      : Predicate<"Subtarget->hasSSE3() && !Subtarget->hasAVX()">;
def HasSSSE3     : Predicate<"Subtarget->hasSSSE3()">;
def UseSSSE3     : Predicate<"Subtarget->hasSSSE3() && !Subtarget->hasAVX()">;
def HasSSE41     : Predicate<"Subtarget->hasSSE41()">;
def UseSSE41     : Predicate<"Subtarget->hasSSE41() && !Subtarget->hasAVX()">;
def HasSSE42     : Predicate<"Subtarget->hasSSE42()">;
def UseSSE42     : Predicate<"Subtarget->hasSSE42() && !Subtarget->hasAVX()">;
def HasSSE4A     : Predicate<"Subtarget->hasSSE4A()">;
def HasAVX       : Predicate<"Subtarget->hasAVX()">;
def HasAVX2      : Predicate<"Subtarget->hasAVX2()">;
def HasAVX1Only  : Predicate<"Subtarget->hasAVX() && !Subtarget->hasAVX2()">;
def HasAVX512    : Predicate<"Subtarget->hasAVX512()">,
                     AssemblerPredicate<"FeatureAVX512", "AVX-512 ISA">;
def UseAVX       : Predicate<"Subtarget->hasAVX() && !Subtarget->hasAVX512()">;
def UseAVX2      : Predicate<"Subtarget->hasAVX2() && !Subtarget->hasAVX512()">;
def NoAVX512       : Predicate<"!Subtarget->hasAVX512()">;
def HasCDI       : Predicate<"Subtarget->hasCDI()">;
def HasPFI       : Predicate<"Subtarget->hasPFI()">;
def HasERI       : Predicate<"Subtarget->hasERI()">;

def HasPOPCNT    : Predicate<"Subtarget->hasPOPCNT()">;
def HasAES       : Predicate<"Subtarget->hasAES()">;
def HasPCLMUL    : Predicate<"Subtarget->hasPCLMUL()">;
def HasFMA       : Predicate<"Subtarget->hasFMA()">;
def UseFMAOnAVX  : Predicate<"Subtarget->hasFMA() && !Subtarget->hasAVX512()">;
def HasFMA4      : Predicate<"Subtarget->hasFMA4()">;
def HasXOP       : Predicate<"Subtarget->hasXOP()">;
def HasTBM       : Predicate<"Subtarget->hasTBM()">;
def HasMOVBE     : Predicate<"Subtarget->hasMOVBE()">;
def HasRDRAND    : Predicate<"Subtarget->hasRDRAND()">;
def HasF16C      : Predicate<"Subtarget->hasF16C()">;
def HasFSGSBase  : Predicate<"Subtarget->hasFSGSBase()">;
def HasLZCNT     : Predicate<"Subtarget->hasLZCNT()">;
def HasBMI       : Predicate<"Subtarget->hasBMI()">;
def HasBMI2      : Predicate<"Subtarget->hasBMI2()">;
def HasRTM       : Predicate<"Subtarget->hasRTM()">;
def HasHLE       : Predicate<"Subtarget->hasHLE()">;
def HasTSX       : Predicate<"Subtarget->hasRTM() || Subtarget->hasHLE()">;
def HasADX       : Predicate<"Subtarget->hasADX()">;
def HasSHA       : Predicate<"Subtarget->hasSHA()">;
def HasPRFCHW    : Predicate<"Subtarget->hasPRFCHW()">;
def HasRDSEED    : Predicate<"Subtarget->hasRDSEED()">;
def HasPrefetchW : Predicate<"Subtarget->hasPRFCHW()">;
def FPStackf32   : Predicate<"!Subtarget->hasSSE1()">;
def FPStackf64   : Predicate<"!Subtarget->hasSSE2()">;
def HasCmpxchg16b: Predicate<"Subtarget->hasCmpxchg16b()">;
def Not64BitMode : Predicate<"!Subtarget->is64Bit()">,
                             AssemblerPredicate<"!Mode64Bit", "Not 64-bit mode">;
def In64BitMode  : Predicate<"Subtarget->is64Bit()">,
                             AssemblerPredicate<"Mode64Bit", "64-bit mode">;
def In16BitMode  : Predicate<"Subtarget->is16Bit()">,
                             AssemblerPredicate<"Mode16Bit", "16-bit mode">;
def Not16BitMode : Predicate<"!Subtarget->is16Bit()">,
                             AssemblerPredicate<"!Mode16Bit", "Not 16-bit mode">;
def In32BitMode  : Predicate<"Subtarget->is32Bit()">,
                             AssemblerPredicate<"Mode32Bit", "32-bit mode">;
def IsWin64      : Predicate<"Subtarget->isTargetWin64()">;
def IsNaCl       : Predicate<"Subtarget->isTargetNaCl()">;
def NotNaCl      : Predicate<"!Subtarget->isTargetNaCl()">;
def SmallCode    : Predicate<"TM.getCodeModel() == CodeModel::Small">;
def KernelCode   : Predicate<"TM.getCodeModel() == CodeModel::Kernel">;
def FarData      : Predicate<"TM.getCodeModel() != CodeModel::Small &&"
                             "TM.getCodeModel() != CodeModel::Kernel">;
def NearData     : Predicate<"TM.getCodeModel() == CodeModel::Small ||"
                             "TM.getCodeModel() == CodeModel::Kernel">;
def IsStatic     : Predicate<"TM.getRelocationModel() == Reloc::Static">;
def IsNotPIC     : Predicate<"TM.getRelocationModel() != Reloc::PIC_">;
def OptForSize   : Predicate<"OptForSize">;
def OptForSpeed  : Predicate<"!OptForSize">;
def FastBTMem    : Predicate<"!Subtarget->isBTMemSlow()">;
def CallImmAddr  : Predicate<"Subtarget->IsLegalToCallImmediateAddr(TM)">;
def FavorMemIndirectCall  : Predicate<"!Subtarget->callRegIndirect()">;

//===----------------------------------------------------------------------===//
// Cse523 Instruction Format Definitions.
//

include "Cse523InstrFormats.td"

//===----------------------------------------------------------------------===//
// Pattern fragments.
//

// Cse523 specific condition code. These correspond to CondCode in
// Cse523InstrInfo.h. They must be kept in synch.
def Cse523_COND_A   : PatLeaf<(i8 0)>;  // alt. COND_NBE
def Cse523_COND_AE  : PatLeaf<(i8 1)>;  // alt. COND_NC
def Cse523_COND_B   : PatLeaf<(i8 2)>;  // alt. COND_C
def Cse523_COND_BE  : PatLeaf<(i8 3)>;  // alt. COND_NA
def Cse523_COND_E   : PatLeaf<(i8 4)>;  // alt. COND_Z
def Cse523_COND_G   : PatLeaf<(i8 5)>;  // alt. COND_NLE
def Cse523_COND_GE  : PatLeaf<(i8 6)>;  // alt. COND_NL
def Cse523_COND_L   : PatLeaf<(i8 7)>;  // alt. COND_NGE
def Cse523_COND_LE  : PatLeaf<(i8 8)>;  // alt. COND_NG
def Cse523_COND_NE  : PatLeaf<(i8 9)>;  // alt. COND_NZ
def Cse523_COND_NO  : PatLeaf<(i8 10)>;
def Cse523_COND_NP  : PatLeaf<(i8 11)>; // alt. COND_PO
def Cse523_COND_NS  : PatLeaf<(i8 12)>;
def Cse523_COND_O   : PatLeaf<(i8 13)>;
def Cse523_COND_P   : PatLeaf<(i8 14)>; // alt. COND_PE
def Cse523_COND_S   : PatLeaf<(i8 15)>;

let FastIselShouldIgnore = 1 in { // FastIsel should ignore all simm8 instrs.
  def i16immSExt8  : ImmLeaf<i16, [{ return Imm == (int8_t)Imm; }]>;
  def i32immSExt8  : ImmLeaf<i32, [{ return Imm == (int8_t)Imm; }]>;
  def i64immSExt8  : ImmLeaf<i64, [{ return Imm == (int8_t)Imm; }]>;
}

def i64immSExt32 : ImmLeaf<i64, [{ return Imm == (int32_t)Imm; }]>;


// i64immZExt32 predicate - True if the 64-bit immediate fits in a 32-bit
// unsigned field.
def i64immZExt32 : ImmLeaf<i64, [{ return (uint64_t)Imm == (uint32_t)Imm; }]>;

def i64immZExt32SExt8 : ImmLeaf<i64, [{
  return (uint64_t)Imm == (uint32_t)Imm && (int32_t)Imm == (int8_t)Imm;
}]>;

// Helper fragments for loads.
// It's always safe to treat a anyext i16 load as a i32 load if the i16 is
// known to be 32-bit aligned or better. Ditto for i8 to i16.
def loadi16 : PatFrag<(ops node:$ptr), (i16 (unindexedload node:$ptr)), [{
  LoadSDNode *LD = cast<LoadSDNode>(N);
  ISD::LoadExtType ExtType = LD->getExtensionType();
  if (ExtType == ISD::NON_EXTLOAD)
    return true;
  if (ExtType == ISD::EXTLOAD)
    return LD->getAlignment() >= 2 && !LD->isVolatile();
  return false;
}]>;

def loadi16_anyext : PatFrag<(ops node:$ptr), (i32 (unindexedload node:$ptr)),[{
  LoadSDNode *LD = cast<LoadSDNode>(N);
  ISD::LoadExtType ExtType = LD->getExtensionType();
  if (ExtType == ISD::EXTLOAD)
    return LD->getAlignment() >= 2 && !LD->isVolatile();
  return false;
}]>;

def loadi32 : PatFrag<(ops node:$ptr), (i32 (unindexedload node:$ptr)), [{
  LoadSDNode *LD = cast<LoadSDNode>(N);
  ISD::LoadExtType ExtType = LD->getExtensionType();
  if (ExtType == ISD::NON_EXTLOAD)
    return true;
  if (ExtType == ISD::EXTLOAD)
    return LD->getAlignment() >= 4 && !LD->isVolatile();
  return false;
}]>;

def loadi8  : PatFrag<(ops node:$ptr), (i8  (load node:$ptr))>;
def loadi64 : PatFrag<(ops node:$ptr), (i64 (load node:$ptr))>;
def loadf32 : PatFrag<(ops node:$ptr), (f32 (load node:$ptr))>;
def loadf64 : PatFrag<(ops node:$ptr), (f64 (load node:$ptr))>;
def loadf80 : PatFrag<(ops node:$ptr), (f80 (load node:$ptr))>;

def sextloadi16i8  : PatFrag<(ops node:$ptr), (i16 (sextloadi8 node:$ptr))>;
def sextloadi32i8  : PatFrag<(ops node:$ptr), (i32 (sextloadi8 node:$ptr))>;
def sextloadi32i16 : PatFrag<(ops node:$ptr), (i32 (sextloadi16 node:$ptr))>;
def sextloadi64i8  : PatFrag<(ops node:$ptr), (i64 (sextloadi8 node:$ptr))>;
def sextloadi64i16 : PatFrag<(ops node:$ptr), (i64 (sextloadi16 node:$ptr))>;
def sextloadi64i32 : PatFrag<(ops node:$ptr), (i64 (sextloadi32 node:$ptr))>;

def zextloadi8i1   : PatFrag<(ops node:$ptr), (i8  (zextloadi1 node:$ptr))>;
def zextloadi16i1  : PatFrag<(ops node:$ptr), (i16 (zextloadi1 node:$ptr))>;
def zextloadi32i1  : PatFrag<(ops node:$ptr), (i32 (zextloadi1 node:$ptr))>;
def zextloadi16i8  : PatFrag<(ops node:$ptr), (i16 (zextloadi8 node:$ptr))>;
def zextloadi32i8  : PatFrag<(ops node:$ptr), (i32 (zextloadi8 node:$ptr))>;
def zextloadi32i16 : PatFrag<(ops node:$ptr), (i32 (zextloadi16 node:$ptr))>;
def zextloadi64i1  : PatFrag<(ops node:$ptr), (i64 (zextloadi1 node:$ptr))>;
def zextloadi64i8  : PatFrag<(ops node:$ptr), (i64 (zextloadi8 node:$ptr))>;
def zextloadi64i16 : PatFrag<(ops node:$ptr), (i64 (zextloadi16 node:$ptr))>;
def zextloadi64i32 : PatFrag<(ops node:$ptr), (i64 (zextloadi32 node:$ptr))>;

def extloadi8i1    : PatFrag<(ops node:$ptr), (i8  (extloadi1 node:$ptr))>;
def extloadi16i1   : PatFrag<(ops node:$ptr), (i16 (extloadi1 node:$ptr))>;
def extloadi32i1   : PatFrag<(ops node:$ptr), (i32 (extloadi1 node:$ptr))>;
def extloadi16i8   : PatFrag<(ops node:$ptr), (i16 (extloadi8 node:$ptr))>;
def extloadi32i8   : PatFrag<(ops node:$ptr), (i32 (extloadi8 node:$ptr))>;
def extloadi32i16  : PatFrag<(ops node:$ptr), (i32 (extloadi16 node:$ptr))>;
def extloadi64i1   : PatFrag<(ops node:$ptr), (i64 (extloadi1 node:$ptr))>;
def extloadi64i8   : PatFrag<(ops node:$ptr), (i64 (extloadi8 node:$ptr))>;
def extloadi64i16  : PatFrag<(ops node:$ptr), (i64 (extloadi16 node:$ptr))>;
def extloadi64i32  : PatFrag<(ops node:$ptr), (i64 (extloadi32 node:$ptr))>;


// An 'and' node with a single use.
def and_su : PatFrag<(ops node:$lhs, node:$rhs), (and node:$lhs, node:$rhs), [{
  return N->hasOneUse();
}]>;
// An 'srl' node with a single use.
def srl_su : PatFrag<(ops node:$lhs, node:$rhs), (srl node:$lhs, node:$rhs), [{
  return N->hasOneUse();
}]>;
// An 'trunc' node with a single use.
def trunc_su : PatFrag<(ops node:$src), (trunc node:$src), [{
  return N->hasOneUse();
}]>;

//===----------------------------------------------------------------------===//
// Instruction list.
//

// Nop
let neverHasSideEffects = 1, SchedRW = [WriteZero] in {
  def NOOP : I<0x90, RawFrm, (outs), (ins), "nop", [], IIC_NOP>;
  def NOOPW : I<0x1f, MRMXm, (outs), (ins i16mem:$zero),
                "nop{w}\t$zero", [], IIC_NOP>, TB, OpSize16;
  def NOOPL : I<0x1f, MRMXm, (outs), (ins i32mem:$zero),
                "nop{l}\t$zero", [], IIC_NOP>, TB, OpSize32;
}


// Constructing a stack frame.
def ENTER : Ii16<0xC8, RawFrmImm8, (outs), (ins i16imm:$len, i8imm:$lvl),
                 "enter\t$len, $lvl", [], IIC_ENTER>, Sched<[WriteMicrocoded]>;

let SchedRW = [WriteALU] in {
let Defs = [RBP,RSP], Uses = [RBP,RSP], mayLoad = 1, neverHasSideEffects = 1 in
def LEAVE64  : I<0xC9, RawFrm,
                 (outs), (ins), "leave", [], IIC_LEAVE>,
                 Requires<[In64BitMode]>;
} // SchedRW

//===----------------------------------------------------------------------===//
//  Miscellaneous Instructions.
//

let Defs = [RSP], Uses = [RSP], neverHasSideEffects=1 in {
let mayLoad = 1, SchedRW = [WriteLoad] in {
def POP64r   : I<0x58, AddRegFrm, (outs GR64:$reg), (ins), "pop{q}\t$reg", [],
                 IIC_POP_REG>, OpSize32, Requires<[In64BitMode]>;
def POP64rmr: I<0x8F, MRM0r, (outs GR64:$reg), (ins), "pop{q}\t$reg", [],
                IIC_POP_REG>, OpSize32, Requires<[In64BitMode]>;
def POP64rmm: I<0x8F, MRM0m, (outs), (ins i64mem:$dst), "pop{q}\t$dst", [],
                IIC_POP_MEM>, OpSize32, Requires<[In64BitMode]>;
} // mayLoad, SchedRW
let mayStore = 1, SchedRW = [WriteStore] in {
def PUSH64r  : I<0x50, AddRegFrm, (outs), (ins GR64:$reg), "push{q}\t$reg", [],
                 IIC_PUSH_REG>, OpSize32, Requires<[In64BitMode]>;
def PUSH64rmr: I<0xFF, MRM6r, (outs), (ins GR64:$reg), "push{q}\t$reg", [],
                 IIC_PUSH_REG>, OpSize32, Requires<[In64BitMode]>;
def PUSH64rmm: I<0xFF, MRM6m, (outs), (ins i64mem:$src), "push{q}\t$src", [],
                 IIC_PUSH_MEM>, OpSize32, Requires<[In64BitMode]>;
} // mayStore, SchedRW
}

let Defs = [RSP], Uses = [RSP], neverHasSideEffects = 1, mayStore = 1,
    SchedRW = [WriteStore] in {
def PUSH64i8   : Ii8<0x6a, RawFrm, (outs), (ins i64i8imm:$imm),
                    "push{q}\t$imm", [], IIC_PUSH_IMM>, Requires<[In64BitMode]>;
def PUSH64i16  : Ii16<0x68, RawFrm, (outs), (ins i16imm:$imm),
                    "push{w}\t$imm", [], IIC_PUSH_IMM>, OpSize16,
                    Requires<[In64BitMode]>;
def PUSH64i32  : Ii32S<0x68, RawFrm, (outs), (ins i64i32imm:$imm),
                    "push{q}\t$imm", [], IIC_PUSH_IMM>, OpSize32,
                    Requires<[In64BitMode]>;
}

let Defs = [RSP, EFLAGS], Uses = [RSP], mayLoad = 1, neverHasSideEffects=1 in
def POPF64   : I<0x9D, RawFrm, (outs), (ins), "popfq", [], IIC_POP_FD>,
               OpSize32, Requires<[In64BitMode]>, Sched<[WriteLoad]>;
let Defs = [RSP], Uses = [RSP, EFLAGS], mayStore = 1, neverHasSideEffects=1 in
def PUSHF64    : I<0x9C, RawFrm, (outs), (ins), "pushfq", [], IIC_PUSH_F>,
                 OpSize32, Requires<[In64BitMode]>, Sched<[WriteStore]>;

let Constraints = "$src = $dst", SchedRW = [WriteALU] in {
def BSWAP64r : RI<0xC8, AddRegFrm, (outs GR64:$dst), (ins GR64:$src),
                  "bswap{q}\t$dst",
                  [(set GR64:$dst, (bswap GR64:$src))], IIC_BSWAP>, TB;
} // Constraints = "$src = $dst", SchedRW

let SchedRW = [WriteMicrocoded] in {
// These uses the DF flag in the EFLAGS register to inc or dec EDI and ESI
let Defs = [RCX,RDI], Uses = [RAX,RCX,RDI,EFLAGS] in
def STOSQ : RI<0xAB, RawFrmDst, (outs dstidx64:$dst), (ins),
               "stosq\t{%rax, $dst|$dst, rax}", [], IIC_STOS>;
} // SchedRW

//===----------------------------------------------------------------------===//
//  Move Instructions.
//
let SchedRW = [WriteMove] in {
let neverHasSideEffects = 1 in {
def MOV64rr : RI<0x89, MRMDestReg, (outs GR64:$dst), (ins GR64:$src),
                 "mov{q}\t{$src, $dst|$dst, $src}", [], IIC_MOV>;
}

let isReMaterializable = 1, isAsCheapAsAMove = 1 in {
def MOV64ri32 : RIi32S<0xC7, MRM0r, (outs GR64:$dst), (ins i64i32imm:$src),
                       "mov{q}\t{$src, $dst|$dst, $src}",
                       [(set GR64:$dst, i64immSExt32:$src)], IIC_MOV>;
}

let isReMaterializable = 1 in {
def MOV64ri : RIi64<0xB8, AddRegFrm, (outs GR64:$dst), (ins i64imm:$src),
                    "movabs{q}\t{$src, $dst|$dst, $src}",
                    [(set GR64:$dst, imm:$src)], IIC_MOV>;
}

} // SchedRW

let SchedRW = [WriteStore] in {
def MOV64mi32 : RIi32S<0xC7, MRM0m, (outs), (ins i64mem:$dst, i64i32imm:$src),
                       "mov{q}\t{$src, $dst|$dst, $src}",
                       [(store i64immSExt32:$src, addr:$dst)], IIC_MOV_MEM>;
} // SchedRW

let hasSideEffects = 0 in {

// These forms all have full 64-bit absolute addresses in their instructions
// and use the movabs mnemonic to indicate this specific form.
let mayLoad = 1 in {
let Defs = [RAX] in
def MOV64o64a : RIi64<0xA1, RawFrmMemOffs, (outs), (ins offset64:$src),
                     "movabs{q}\t{$src, %rax|rax, $src}", []>,
                     Requires<[In64BitMode]>;
}

let mayStore = 1 in {
let Uses = [RAX] in
def MOV64ao64 : RIi64<0xA3, RawFrmMemOffs, (outs offset64:$dst), (ins),
                     "movabs{q}\t{%rax, $dst|$dst, rax}", []>,
                     Requires<[In64BitMode]>;
}
} // hasSideEffects = 0

let isCodeGenOnly = 1, ForceDisassemble = 1, hasSideEffects = 0,
    SchedRW = [WriteMove] in {
def MOV64rr_REV : RI<0x8B, MRMSrcReg, (outs GR64:$dst), (ins GR64:$src),
                     "mov{q}\t{$src, $dst|$dst, $src}", [], IIC_MOV>;
}

let canFoldAsLoad = 1, isReMaterializable = 1, SchedRW = [WriteLoad] in {
def MOV64rm : RI<0x8B, MRMSrcMem, (outs GR64:$dst), (ins i64mem:$src),
                 "mov{q}\t{$src, $dst|$dst, $src}",
                 [(set GR64:$dst, (load addr:$src))], IIC_MOV_MEM>;
}

let SchedRW = [WriteStore] in {
def MOV8mr  : I<0x88, MRMDestMem, (outs), (ins i8mem:$dst, GR8:$src),
                "mov{b}\t{$src, $dst|$dst, $src}",
                [(truncstorei8 GR8:$src, addr:$dst)], IIC_MOV_MEM>;
def MOV16mr : I<0x89, MRMDestMem, (outs), (ins i16mem:$dst, GR16:$src),
                "mov{w}\t{$src, $dst|$dst, $src}",
                [(truncstorei16 GR16:$src, addr:$dst)], IIC_MOV_MEM>, OpSize16;
def MOV32mr : I<0x89, MRMDestMem, (outs), (ins i32mem:$dst, GR32:$src),
                "mov{l}\t{$src, $dst|$dst, $src}",
                [(truncstorei32 GR32:$src, addr:$dst)], IIC_MOV_MEM>, OpSize32;
def MOV64mr : RI<0x89, MRMDestMem, (outs), (ins i64mem:$dst, GR64:$src),
                 "mov{q}\t{$src, $dst|$dst, $src}",
                 [(store GR64:$src, addr:$dst)], IIC_MOV_MEM>;
} // SchedRW


//===----------------------------------------------------------------------===//
// Atomic support
//

// Atomic swap. These are just normal xchg instructions. But since a memory
// operand is referenced, the atomicity is ensured.
multiclass ATOMIC_SWAP<bits<8> opc8, bits<8> opc, string mnemonic, string frag,
                       InstrItinClass itin> {
  let Constraints = "$val = $dst", SchedRW = [WriteALULd, WriteRMW] in {
    def NAME#64rm : RI<opc, MRMSrcMem, (outs GR64:$dst),
                       (ins GR64:$val, i64mem:$ptr),
                       !strconcat(mnemonic, "{q}\t{$val, $ptr|$ptr, $val}"),
                       [(set
                         GR64:$dst,
                         (!cast<PatFrag>(frag # "_64") addr:$ptr, GR64:$val))],
                       itin>;
  }
}

defm XCHG    : ATOMIC_SWAP<0x86, 0x87, "xchg", "atomic_swap", IIC_XCHG_MEM>;

// Swap between registers.
let SchedRW = [WriteALU] in {
let Constraints = "$val = $dst" in {
def XCHG64rr : RI<0x87, MRMSrcReg, (outs GR64:$dst), (ins GR64:$val,GR64:$src),
                  "xchg{q}\t{$val, $src|$src, $val}", [], IIC_XCHG_REG>;
}

// Swap between EAX and other registers.
let Uses = [RAX], Defs = [RAX] in
def XCHG64ar : RI<0x90, AddRegFrm, (outs), (ins GR64:$src),
                  "xchg{q}\t{$src, %rax|rax, $src}", [], IIC_XCHG_REG>;
} // SchedRW

let SchedRW = [WriteALU] in {
def XADD64rr  : RI<0xC1, MRMDestReg, (outs GR64:$dst), (ins GR64:$src),
                   "xadd{q}\t{$src, $dst|$dst, $src}", [], IIC_XADD_REG>, TB;
} // SchedRW

let mayLoad = 1, mayStore = 1, SchedRW = [WriteALULd, WriteRMW] in {
def XADD64rm  : RI<0xC1, MRMDestMem, (outs), (ins i64mem:$dst, GR64:$src),
                   "xadd{q}\t{$src, $dst|$dst, $src}", [], IIC_XADD_MEM>, TB;

}

let SchedRW = [WriteALU] in {
def CMPXCHG64rr  : RI<0xB1, MRMDestReg, (outs GR64:$dst), (ins GR64:$src),
                      "cmpxchg{q}\t{$src, $dst|$dst, $src}", [],
                      IIC_CMPXCHG_REG>, TB;
} // SchedRW

let SchedRW = [WriteALULd, WriteRMW] in {
let mayLoad = 1, mayStore = 1 in {
def CMPXCHG64rm  : RI<0xB1, MRMDestMem, (outs), (ins i64mem:$dst, GR64:$src),
                      "cmpxchg{q}\t{$src, $dst|$dst, $src}", [],
                      IIC_CMPXCHG_MEM>, TB;
}

} // SchedRW


// Lock instruction prefix
def LOCK_PREFIX : I<0xF0, RawFrm, (outs),  (ins), "lock", []>;

// Rex64 instruction prefix
def REX64_PREFIX : I<0x48, RawFrm, (outs),  (ins), "rex64", []>,
                     Requires<[In64BitMode]>;

// Data16 instruction prefix
def DATA16_PREFIX : I<0x66, RawFrm, (outs),  (ins), "data16", []>;

// Flag instructions
let SchedRW = [WriteALU] in {
def CLC : I<0xF8, RawFrm, (outs), (ins), "clc", [], IIC_CLC>;
def STC : I<0xF9, RawFrm, (outs), (ins), "stc", [], IIC_STC>;
def CLI : I<0xFA, RawFrm, (outs), (ins), "cli", [], IIC_CLI>;
def STI : I<0xFB, RawFrm, (outs), (ins), "sti", [], IIC_STI>;
def CLD : I<0xFC, RawFrm, (outs), (ins), "cld", [], IIC_CLD>;
def STD : I<0xFD, RawFrm, (outs), (ins), "std", [], IIC_STD>;
def CMC : I<0xF5, RawFrm, (outs), (ins), "cmc", [], IIC_CMC>;

def CLTS : I<0x06, RawFrm, (outs), (ins), "clts", [], IIC_CLTS>, TB;
}

// Table lookup instructions
def XLAT : I<0xD7, RawFrm, (outs), (ins), "xlatb", [], IIC_XLAT>,
           Sched<[WriteLoad]>;

//===----------------------------------------------------------------------===//
// MOVBE Instructions
//
let Predicates = [HasMOVBE] in {
  let SchedRW = [WriteALULd] in {
  def MOVBE64rm : RI<0xF0, MRMSrcMem, (outs GR64:$dst), (ins i64mem:$src),
                     "movbe{q}\t{$src, $dst|$dst, $src}",
                     [(set GR64:$dst, (bswap (loadi64 addr:$src)))], IIC_MOVBE>,
                     T8PS;
  }
  let SchedRW = [WriteStore] in {
  def MOVBE64mr : RI<0xF1, MRMDestMem, (outs), (ins i64mem:$dst, GR64:$src),
                     "movbe{q}\t{$src, $dst|$dst, $src}",
                     [(store (bswap GR64:$src), addr:$dst)], IIC_MOVBE>,
                     T8PS;
  }
}



//===----------------------------------------------------------------------===//
// Subsystems.
//===----------------------------------------------------------------------===//

include "Cse523InstrArithmetic.td"
//include "Cse523InstrCMovSetCC.td"
include "Cse523InstrExtension.td"
include "Cse523InstrControl.td"
include "Cse523InstrShiftRotate.td"
//
//// X87 Floating Point Stack.
//include "Cse523InstrFPStack.td"
//
//// SIMD support (SSE, MMX and AVX)
//include "Cse523InstrFragmentsSIMD.td"
//
//// FMA - Fused Multiply-Add support (requires FMA)
//include "Cse523InstrFMA.td"
//
//// XOP
//include "Cse523InstrXOP.td"
//
//// SSE, MMX and 3DNow! vector support.
//include "Cse523InstrSSE.td"
//include "Cse523InstrAVX512.td"
//include "Cse523InstrMMX.td"
//include "Cse523Instr3DNow.td"
//
//include "Cse523InstrVMX.td"
//include "Cse523InstrSVM.td"
//
//include "Cse523InstrTSX.td"
//
//// System instructions.
include "Cse523InstrSystem.td"
//
// Compiler Pseudo Instructions and Pat Patterns
include "Cse523InstrCompiler.td"

//===----------------------------------------------------------------------===//
// Assembler Mnemonic Aliases
//===----------------------------------------------------------------------===//

def : MnemonicAlias<"call", "callq", "att">, Requires<[In64BitMode]>;

def : MnemonicAlias<"cbw",  "cbtw", "att">;
def : MnemonicAlias<"cwde", "cwtl", "att">;
def : MnemonicAlias<"cwd",  "cwtd", "att">;
def : MnemonicAlias<"cdq",  "cltd", "att">;
def : MnemonicAlias<"cdqe", "cltq", "att">;
def : MnemonicAlias<"cqo",  "cqto", "att">;

// In 64-bit mode lret maps to lretl; it is not ambiguous with lretq.
def : MnemonicAlias<"lret", "lretl", "att">, Requires<[Not16BitMode]>;

def : MnemonicAlias<"leaveq", "leave", "att">, Requires<[In64BitMode]>;

def : MnemonicAlias<"loopz",  "loope",  "att">;
def : MnemonicAlias<"loopnz", "loopne", "att">;

def : MnemonicAlias<"pop",   "popq",  "att">, Requires<[In64BitMode]>;
def : MnemonicAlias<"popf",  "popfq", "att">, Requires<[In64BitMode]>;
def : MnemonicAlias<"popfd", "popfl", "att">;

// FIXME: This is wrong for "push reg".  "push %bx" should turn into pushw in
// all modes.  However: "push (addr)" and "push $42" should default to
// pushl/pushq depending on the current mode.  Similar for "pop %bx"
def : MnemonicAlias<"push",   "pushq",  "att">, Requires<[In64BitMode]>;
def : MnemonicAlias<"pushf",  "pushfq", "att">, Requires<[In64BitMode]>;
def : MnemonicAlias<"pushfd", "pushfl", "att">;

def : MnemonicAlias<"repe",  "rep",   "att">;
def : MnemonicAlias<"repz",  "rep",   "att">;
def : MnemonicAlias<"repnz", "repne", "att">;

def : MnemonicAlias<"ret", "retq", "att">, Requires<[In64BitMode]>;

def : MnemonicAlias<"salb", "shlb", "att">;
def : MnemonicAlias<"salw", "shlw", "att">;
def : MnemonicAlias<"sall", "shll", "att">;
def : MnemonicAlias<"salq", "shlq", "att">;

def : MnemonicAlias<"smovb", "movsb", "att">;
def : MnemonicAlias<"smovw", "movsw", "att">;
def : MnemonicAlias<"smovl", "movsl", "att">;
def : MnemonicAlias<"smovq", "movsq", "att">;

def : MnemonicAlias<"ud2a",  "ud2",  "att">;
def : MnemonicAlias<"verrw", "verr", "att">;

// System instruction aliases.
def : MnemonicAlias<"iret",    "iretl",    "att">, Requires<[Not16BitMode]>;
def : MnemonicAlias<"sysret",  "sysretl",  "att">;
def : MnemonicAlias<"sysexit", "sysexitl", "att">;

def : MnemonicAlias<"lgdt", "lgdtq", "att">, Requires<[In64BitMode]>;
def : MnemonicAlias<"lidt", "lidtq", "att">, Requires<[In64BitMode]>;
def : MnemonicAlias<"sgdt", "sgdtq", "att">, Requires<[In64BitMode]>;
def : MnemonicAlias<"sidt", "sidtq", "att">, Requires<[In64BitMode]>;


class CondCodeAlias<string Prefix,string Suffix, string OldCond, string NewCond,
                    string VariantName>
  : MnemonicAlias<!strconcat(Prefix, OldCond, Suffix),
                  !strconcat(Prefix, NewCond, Suffix), VariantName>;

/// IntegerCondCodeMnemonicAlias - This multiclass defines a bunch of
/// MnemonicAlias's that canonicalize the condition code in a mnemonic, for
/// example "setz" -> "sete".
multiclass IntegerCondCodeMnemonicAlias<string Prefix, string Suffix,
                                        string V = ""> {
  def C   : CondCodeAlias<Prefix, Suffix, "c",   "b",  V>; // setc   -> setb
  def Z   : CondCodeAlias<Prefix, Suffix, "z" ,  "e",  V>; // setz   -> sete
  def NA  : CondCodeAlias<Prefix, Suffix, "na",  "be", V>; // setna  -> setbe
  def NB  : CondCodeAlias<Prefix, Suffix, "nb",  "ae", V>; // setnb  -> setae
  def NC  : CondCodeAlias<Prefix, Suffix, "nc",  "ae", V>; // setnc  -> setae
  def NG  : CondCodeAlias<Prefix, Suffix, "ng",  "le", V>; // setng  -> setle
  def NL  : CondCodeAlias<Prefix, Suffix, "nl",  "ge", V>; // setnl  -> setge
  def NZ  : CondCodeAlias<Prefix, Suffix, "nz",  "ne", V>; // setnz  -> setne
  def PE  : CondCodeAlias<Prefix, Suffix, "pe",  "p",  V>; // setpe  -> setp
  def PO  : CondCodeAlias<Prefix, Suffix, "po",  "np", V>; // setpo  -> setnp

  def NAE : CondCodeAlias<Prefix, Suffix, "nae", "b",  V>; // setnae -> setb
  def NBE : CondCodeAlias<Prefix, Suffix, "nbe", "a",  V>; // setnbe -> seta
  def NGE : CondCodeAlias<Prefix, Suffix, "nge", "l",  V>; // setnge -> setl
  def NLE : CondCodeAlias<Prefix, Suffix, "nle", "g",  V>; // setnle -> setg
}

// Aliases for set<CC>
defm : IntegerCondCodeMnemonicAlias<"set", "">;
// Aliases for j<CC>
defm : IntegerCondCodeMnemonicAlias<"j", "">;
// Aliases for cmov<CC>{w,l,q}
defm : IntegerCondCodeMnemonicAlias<"cmov", "w", "att">;
defm : IntegerCondCodeMnemonicAlias<"cmov", "l", "att">;
defm : IntegerCondCodeMnemonicAlias<"cmov", "q", "att">;
// No size suffix for intel-style asm.
defm : IntegerCondCodeMnemonicAlias<"cmov", "", "intel">;


//===----------------------------------------------------------------------===//
// Assembler Instruction Aliases
//===----------------------------------------------------------------------===//

//// aad/aam default to base 10 if no operand is specified.
//def : InstAlias<"aad", (AAD8i8 10)>;
//def : InstAlias<"aam", (AAM8i8 10)>;
//
//// Disambiguate the mem/imm form of bt-without-a-suffix as btl.
//// Likewise for btc/btr/bts.
//def : InstAlias<"bt {$imm, $mem|$mem, $imm}",
//                (BT32mi8 i32mem:$mem, i32i8imm:$imm), 0>;
//def : InstAlias<"btc {$imm, $mem|$mem, $imm}",
//                (BTC32mi8 i32mem:$mem, i32i8imm:$imm), 0>;
//def : InstAlias<"btr {$imm, $mem|$mem, $imm}",
//                (BTR32mi8 i32mem:$mem, i32i8imm:$imm), 0>;
//def : InstAlias<"bts {$imm, $mem|$mem, $imm}",
//                (BTS32mi8 i32mem:$mem, i32i8imm:$imm), 0>;
//
//// clr aliases.
//def : InstAlias<"clrq $reg", (XOR64rr GR64:$reg, GR64:$reg), 0>;
//
//// lods aliases. Accept the destination being omitted because it's implicit
//// in the mnemonic, or the mnemonic suffix being omitted because it's implicit
//// in the destination.
//def : InstAlias<"lodsb $src", (LODSB srcidx8:$src),  0>;
//def : InstAlias<"lodsw $src", (LODSW srcidx16:$src), 0>;
//def : InstAlias<"lods{l|d} $src", (LODSL srcidx32:$src), 0>;
//def : InstAlias<"lodsq $src", (LODSQ srcidx64:$src), 0>, Requires<[In64BitMode]>;
//def : InstAlias<"lods {$src, %al|al, $src}", (LODSB srcidx8:$src),  0>;
//def : InstAlias<"lods {$src, %ax|ax, $src}", (LODSW srcidx16:$src), 0>;
//def : InstAlias<"lods {$src, %eax|eax, $src}", (LODSL srcidx32:$src), 0>;
//def : InstAlias<"lods {$src, %rax|rax, $src}", (LODSQ srcidx64:$src), 0>, Requires<[In64BitMode]>;
//
//// stos aliases. Accept the source being omitted because it's implicit in
//// the mnemonic, or the mnemonic suffix being omitted because it's implicit
//// in the source.
//def : InstAlias<"stosb $dst", (STOSB dstidx8:$dst),  0>;
//def : InstAlias<"stosw $dst", (STOSW dstidx16:$dst), 0>;
//def : InstAlias<"stos{l|d} $dst", (STOSL dstidx32:$dst), 0>;
//def : InstAlias<"stosq $dst", (STOSQ dstidx64:$dst), 0>, Requires<[In64BitMode]>;
//def : InstAlias<"stos {%al, $dst|$dst, al}", (STOSB dstidx8:$dst),  0>;
//def : InstAlias<"stos {%ax, $dst|$dst, ax}", (STOSW dstidx16:$dst), 0>;
//def : InstAlias<"stos {%eax, $dst|$dst, eax}", (STOSL dstidx32:$dst), 0>;
//def : InstAlias<"stos {%rax, $dst|$dst, rax}", (STOSQ dstidx64:$dst), 0>, Requires<[In64BitMode]>;
//
//// scas aliases. Accept the destination being omitted because it's implicit
//// in the mnemonic, or the mnemonic suffix being omitted because it's implicit
//// in the destination.
//def : InstAlias<"scasb $dst", (SCASB dstidx8:$dst),  0>;
//def : InstAlias<"scasw $dst", (SCASW dstidx16:$dst), 0>;
//def : InstAlias<"scas{l|d} $dst", (SCASL dstidx32:$dst), 0>;
//def : InstAlias<"scasq $dst", (SCASQ dstidx64:$dst), 0>, Requires<[In64BitMode]>;
//def : InstAlias<"scas {$dst, %al|al, $dst}", (SCASB dstidx8:$dst),  0>;
//def : InstAlias<"scas {$dst, %ax|ax, $dst}", (SCASW dstidx16:$dst), 0>;
//def : InstAlias<"scas {$dst, %eax|eax, $dst}", (SCASL dstidx32:$dst), 0>;
//def : InstAlias<"scas {$dst, %rax|rax, $dst}", (SCASQ dstidx64:$dst), 0>, Requires<[In64BitMode]>;
//
//// div and idiv aliases for explicit A register.
//def : InstAlias<"div{q}\t{$src, %rax|rax, $src}", (DIV64r GR64:$src)>;
//def : InstAlias<"div{b}\t{$src, %al|al, $src}", (DIV8m  i8mem :$src)>;
//def : InstAlias<"div{w}\t{$src, %ax|ax, $src}", (DIV16m i16mem:$src)>;
//def : InstAlias<"div{l}\t{$src, %eax|eax, $src}", (DIV32m i32mem:$src)>;
//def : InstAlias<"div{q}\t{$src, %rax|rax, $src}", (DIV64m i64mem:$src)>;
//def : InstAlias<"idiv{q}\t{$src, %rax|rax, $src}", (IDIV64r GR64:$src)>;
//def : InstAlias<"idiv{b}\t{$src, %al|al, $src}", (IDIV8m  i8mem :$src)>;
//def : InstAlias<"idiv{w}\t{$src, %ax|ax, $src}", (IDIV16m i16mem:$src)>;
//def : InstAlias<"idiv{l}\t{$src, %eax|eax, $src}", (IDIV32m i32mem:$src)>;
//def : InstAlias<"idiv{q}\t{$src, %rax|rax, $src}", (IDIV64m i64mem:$src)>;
//
//
//
//// Various unary fpstack operations default to operating on on ST1.
//// For example, "fxch" -> "fxch %st(1)"
//def : InstAlias<"faddp",        (ADD_FPrST0  ST1), 0>;
//def : InstAlias<"fsub{|r}p",    (SUBR_FPrST0 ST1), 0>;
//def : InstAlias<"fsub{r|}p",    (SUB_FPrST0  ST1), 0>;
//def : InstAlias<"fmulp",        (MUL_FPrST0  ST1), 0>;
//def : InstAlias<"fdiv{|r}p",    (DIVR_FPrST0 ST1), 0>;
//def : InstAlias<"fdiv{r|}p",    (DIV_FPrST0  ST1), 0>;
//def : InstAlias<"fxch",         (XCH_F       ST1), 0>;
//def : InstAlias<"fcom",         (COM_FST0r   ST1), 0>;
//def : InstAlias<"fcomp",        (COMP_FST0r  ST1), 0>;
//def : InstAlias<"fcomi",        (COM_FIr     ST1), 0>;
//def : InstAlias<"fcompi",       (COM_FIPr    ST1), 0>;
//def : InstAlias<"fucom",        (UCOM_Fr     ST1), 0>;
//def : InstAlias<"fucomp",       (UCOM_FPr    ST1), 0>;
//def : InstAlias<"fucomi",       (UCOM_FIr    ST1), 0>;
//def : InstAlias<"fucompi",      (UCOM_FIPr   ST1), 0>;
//
//// Handle fmul/fadd/fsub/fdiv instructions with explicitly written st(0) op.
//// For example, "fadd %st(4), %st(0)" -> "fadd %st(4)".  We also disambiguate
//// instructions like "fadd %st(0), %st(0)" as "fadd %st(0)" for consistency with
//// gas.
//multiclass FpUnaryAlias<string Mnemonic, Instruction Inst, bit EmitAlias = 1> {
// def : InstAlias<!strconcat(Mnemonic, "\t{$op, %st(0)|st(0), $op}"),
//                 (Inst RST:$op), EmitAlias>;
// def : InstAlias<!strconcat(Mnemonic, "\t{%st(0), %st(0)|st(0), st(0)}"),
//                 (Inst ST0), EmitAlias>;
//}
//
//defm : FpUnaryAlias<"fadd",   ADD_FST0r>;
//defm : FpUnaryAlias<"faddp",  ADD_FPrST0, 0>;
//defm : FpUnaryAlias<"fsub",   SUB_FST0r>;
//defm : FpUnaryAlias<"fsub{|r}p",  SUBR_FPrST0>;
//defm : FpUnaryAlias<"fsubr",  SUBR_FST0r>;
//defm : FpUnaryAlias<"fsub{r|}p", SUB_FPrST0>;
//defm : FpUnaryAlias<"fmul",   MUL_FST0r>;
//defm : FpUnaryAlias<"fmulp",  MUL_FPrST0>;
//defm : FpUnaryAlias<"fdiv",   DIV_FST0r>;
//defm : FpUnaryAlias<"fdiv{|r}p",  DIVR_FPrST0>;
//defm : FpUnaryAlias<"fdivr",  DIVR_FST0r>;
//defm : FpUnaryAlias<"fdiv{r|}p", DIV_FPrST0>;
//defm : FpUnaryAlias<"fcomi",   COM_FIr, 0>;
//defm : FpUnaryAlias<"fucomi",  UCOM_FIr, 0>;
//defm : FpUnaryAlias<"fcompi",   COM_FIPr>;
//defm : FpUnaryAlias<"fucompi",  UCOM_FIPr>;
//
//
//// Handle "f{mulp,addp} st(0), $op" the same as "f{mulp,addp} $op", since they
//// commute.  We also allow fdiv[r]p/fsubrp even though they don't commute,
//// solely because gas supports it.
//def : InstAlias<"faddp\t{%st(0), $op|$op, st(0)}", (ADD_FPrST0 RST:$op), 0>;
//def : InstAlias<"fmulp\t{%st(0), $op|$op, st(0)}", (MUL_FPrST0 RST:$op)>;
//def : InstAlias<"fsub{|r}p\t{%st(0), $op|$op, st(0)}", (SUBR_FPrST0 RST:$op)>;
//def : InstAlias<"fsub{r|}p\t{%st(0), $op|$op, st(0)}", (SUB_FPrST0 RST:$op)>;
//def : InstAlias<"fdiv{|r}p\t{%st(0), $op|$op, st(0)}", (DIVR_FPrST0 RST:$op)>;
//def : InstAlias<"fdiv{r|}p\t{%st(0), $op|$op, st(0)}", (DIV_FPrST0 RST:$op)>;
//
//// We accept "fnstsw %eax" even though it only writes %ax.
//def : InstAlias<"fnstsw\t{%eax|eax}", (FNSTSW16r)>;
//def : InstAlias<"fnstsw\t{%al|al}" , (FNSTSW16r)>;
//def : InstAlias<"fnstsw"     , (FNSTSW16r)>;
//
//// lcall and ljmp aliases.  This seems to be an odd mapping in 64-bit mode, but
//// this is compatible with what GAS does.
//def : InstAlias<"lcall $seg, $off", (FARCALL32i i32imm:$off, i16imm:$seg)>, Requires<[Not16BitMode]>;
//def : InstAlias<"ljmp $seg, $off",  (FARJMP32i  i32imm:$off, i16imm:$seg)>, Requires<[Not16BitMode]>;
//def : InstAlias<"lcall *$dst",      (FARCALL32m opaque48mem:$dst)>, Requires<[Not16BitMode]>;
//def : InstAlias<"ljmp *$dst",       (FARJMP32m  opaque48mem:$dst)>, Requires<[Not16BitMode]>;
//
def : InstAlias<"call *$dst",       (CALL64m i16mem:$dst)>, Requires<[In64BitMode]>;
def : InstAlias<"jmp *$dst",        (JMP64m  i16mem:$dst)>, Requires<[In64BitMode]>;
//
//
//// "imul <imm>, B" is an alias for "imul <imm>, B, B".
def : InstAlias<"imulq $imm, $r",(IMUL64rri32 GR64:$r, GR64:$r,i64i32imm:$imm)>;
//def : InstAlias<"imulq $imm, $r", (IMUL64rri8 GR64:$r, GR64:$r, i64i8imm:$imm)>;
//
//// inb %dx -> inb %al, %dx
//def : InstAlias<"inb\t{%dx|dx}", (IN8rr), 0>;
//def : InstAlias<"inw\t{%dx|dx}", (IN16rr), 0>;
//def : InstAlias<"inl\t{%dx|dx}", (IN32rr), 0>;
//def : InstAlias<"inb\t$port", (IN8ri i8imm:$port), 0>;
//def : InstAlias<"inw\t$port", (IN16ri i8imm:$port), 0>;
//def : InstAlias<"inl\t$port", (IN32ri i8imm:$port), 0>;
//
//
//// jmp and call aliases for lcall and ljmp.  jmp $42,$5 -> ljmp
//def : InstAlias<"call $seg, $off",  (FARCALL32i i32imm:$off, i16imm:$seg)>, Requires<[Not16BitMode]>;
//def : InstAlias<"jmp $seg, $off",   (FARJMP32i  i32imm:$off, i16imm:$seg)>, Requires<[Not16BitMode]>;
//def : InstAlias<"callw $seg, $off", (FARCALL16i i16imm:$off, i16imm:$seg)>;
//def : InstAlias<"jmpw $seg, $off",  (FARJMP16i  i16imm:$off, i16imm:$seg)>;
//def : InstAlias<"calll $seg, $off", (FARCALL32i i32imm:$off, i16imm:$seg)>;
//def : InstAlias<"jmpl $seg, $off",  (FARJMP32i  i32imm:$off, i16imm:$seg)>;
//
//// Force mov without a suffix with a segment and mem to prefer the 'l' form of
//// the move.  All segment/mem forms are equivalent, this has the shortest
//// encoding.
//def : InstAlias<"mov $mem, $seg", (MOV32sm SEGMENT_REG:$seg, i32mem:$mem)>;
//def : InstAlias<"mov $seg, $mem", (MOV32ms i32mem:$mem, SEGMENT_REG:$seg)>;
//
// Match 'movq <largeimm>, <reg>' as an alias for movabsq.
def : InstAlias<"movq $imm, $reg", (MOV64ri GR64:$reg, i64imm:$imm)>;
//
//// outb %dx -> outb %al, %dx
//def : InstAlias<"outb\t{%dx|dx}", (OUT8rr), 0>;
//def : InstAlias<"outw\t{%dx|dx}", (OUT16rr), 0>;
//def : InstAlias<"outl\t{%dx|dx}", (OUT32rr), 0>;
//def : InstAlias<"outb\t$port", (OUT8ir i8imm:$port), 0>;
//def : InstAlias<"outw\t$port", (OUT16ir i8imm:$port), 0>;
//def : InstAlias<"outl\t$port", (OUT32ir i8imm:$port), 0>;
//
//// 'sldt <mem>' can be encoded with either sldtw or sldtq with the same
//// effect (both store to a 16-bit mem).  Force to sldtw to avoid ambiguity
//// errors, since its encoding is the most compact.
//def : InstAlias<"sldt $mem", (SLDT16m i16mem:$mem)>;

//// shld/shrd op,op -> shld op, op, CL
//def : InstAlias<"shld{q}\t{$r2, $r1|$r1, $r2}", (SHLD64rrCL GR64:$r1, GR64:$r2), 0>;
//def : InstAlias<"shrd{q}\t{$r2, $r1|$r1, $r2}", (SHRD64rrCL GR64:$r1, GR64:$r2), 0>;
//
//def : InstAlias<"shld{q}\t{$reg, $mem|$mem, $reg}", (SHLD64mrCL i64mem:$mem, GR64:$reg), 0>;
//def : InstAlias<"shrd{q}\t{$reg, $mem|$mem, $reg}", (SHRD64mrCL i64mem:$mem, GR64:$reg), 0>;

///*  FIXME: This is disabled because the asm matcher is currently incapable of
// *  matching a fixed immediate like $1.
//// "shl X, $1" is an alias for "shl X".
//multiclass ShiftRotateByOneAlias<string Mnemonic, string Opc> {
//// def : InstAlias<!strconcat(Mnemonic, "q $op, $$1"),
////                 (!cast<Instruction>(!strconcat(Opc, "64r1")) GR64:$op)>;
//// def : InstAlias<!strconcat(Mnemonic, "b $op, $$1"),
////                 (!cast<Instruction>(!strconcat(Opc, "8m1")) i8mem:$op)>;
//// def : InstAlias<!strconcat(Mnemonic, "w $op, $$1"),
////                 (!cast<Instruction>(!strconcat(Opc, "16m1")) i16mem:$op)>;
//// def : InstAlias<!strconcat(Mnemonic, "l $op, $$1"),
////                 (!cast<Instruction>(!strconcat(Opc, "32m1")) i32mem:$op)>;
// def : InstAlias<!strconcat(Mnemonic, "q $op, $$1"),
//                 (!cast<Instruction>(!strconcat(Opc, "64m1")) i64mem:$op)>;
//}
//
//defm : ShiftRotateByOneAlias<"rcl", "RCL">;
//defm : ShiftRotateByOneAlias<"rcr", "RCR">;
//defm : ShiftRotateByOneAlias<"rol", "ROL">;
//defm : ShiftRotateByOneAlias<"ror", "ROR">;
//FIXME */
//
// test: We accept "testX <reg>, <mem>" and "testX <mem>, <reg>" as synonyms.
def : InstAlias<"test{q}\t{$val, $mem|$mem, $val}", (TEST64rm GR64:$val, i64mem:$mem)>;
//
//// xchg: We accept "xchgX <reg>, <mem>" and "xchgX <mem>, <reg>" as synonyms.
//def : InstAlias<"xchg{q}\t{$mem, $val|$val, $mem}", (XCHG64rm GR64:$val, i64mem:$mem)>;
//
//// xchg: We accept "xchgX <reg>, %eax" and "xchgX %eax, <reg>" as synonyms.
//def : InstAlias<"xchg{q}\t{%rax, $src|$src, rax}", (XCHG64ar GR64:$src)>;
