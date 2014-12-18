//===-- Cse523InstrExtension.td - Sign and Zero Extensions ----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the sign and zero extension operations.
//
//===----------------------------------------------------------------------===//

let neverHasSideEffects = 1 in {
  let Defs = [RAX,RDX], Uses = [RAX] in
  def CQO  : RI<0x99, RawFrm, (outs), (ins),
                "{cqto|cqo}", [], IIC_CBW>; // RDX:RAX = signext(RAX)
}

def MOVSX64rm8 : RI<0xBE, MRMSrcMem, (outs GR64:$dst), (ins i8mem :$src),
                    "movs{bq|x}\t{$src, $dst|$dst, $src}",
                    [(set GR64:$dst, (sextloadi64i8 addr:$src))], IIC_MOVSX>,
                    TB, Sched<[WriteALULd]>;
def MOVSX64rm16: RI<0xBF, MRMSrcMem, (outs GR64:$dst), (ins i16mem:$src),
                    "movs{wq|x}\t{$src, $dst|$dst, $src}",
                    [(set GR64:$dst, (sextloadi64i16 addr:$src))], IIC_MOVSX>,
                    TB, Sched<[WriteALULd]>;
def MOVSX64rm32: RI<0x63, MRMSrcMem, (outs GR64:$dst), (ins i32mem:$src),
                    "movs{lq|xd}\t{$src, $dst|$dst, $src}",
                    [(set GR64:$dst, (sextloadi64i32 addr:$src))], IIC_MOVSX>,
                    Sched<[WriteALULd]>;

def MOVZX64rm8 : I<0xB6, MRMSrcMem, (outs GR32:$dst), (ins i8mem :$src),
                   "movz{bl|x}\t{$src, $dst|$dst, $src}",
                   [(set GR32:$dst, (zextloadi64i8 addr:$src))], IIC_MOVZX>,
                   TB, Sched<[WriteALULd]>;
def MOVZX64rm16 : I<0xB7, MRMSrcMem, (outs GR32:$dst), (ins i16mem :$src),
                   "movz{bl|x}\t{$src, $dst|$dst, $src}",
                   [(set GR32:$dst, (zextloadi64i16 addr:$src))], IIC_MOVZX>,
                   TB, Sched<[WriteALULd]>;

def : Pat<(extloadi64i8 addr:$src), (MOVZX64rm8 addr:$src)>;
def : Pat<(extloadi64i16 addr:$src), (MOVZX64rm16 addr:$src)>;
def : Pat<(zextloadi64i32 addr:$src), (MOV32rm addr:$src)>;
