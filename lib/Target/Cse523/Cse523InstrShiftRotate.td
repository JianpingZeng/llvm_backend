//===-- Cse523InstrShiftRotate.td - Shift and Rotate Instrs ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the shift and rotate instructions.
//
//===----------------------------------------------------------------------===//

// FIXME: Someone needs to smear multipattern goodness all over this file.

let Defs = [EFLAGS] in {

let Constraints = "$src1 = $dst", SchedRW = [WriteShift] in {
let Uses = [RCX] in {
def SHL64rCL : RI<0xD3, MRM4r, (outs GR64:$dst), (ins GR64:$src1),
                  "shl{q}\t{%cl, $dst|$dst, rcx}",
                  [(set GR64:$dst, (shl GR64:$src1, RCX))], IIC_SR>;
} // Uses = [CL]

let isConvertibleToThreeAddress = 1 in {   // Can transform into LEA.
def SHL64ri  : RIi8<0xC1, MRM4r, (outs GR64:$dst),
                    (ins GR64:$src1, i8imm:$src2),
                    "shl{q}\t{$src2, $dst|$dst, $src2}",
                    [(set GR64:$dst, (shl GR64:$src1, (i8 imm:$src2)))],
                    IIC_SR>;

// NOTE: We don't include patterns for shifts of a register by one, because
// 'add reg,reg' is cheaper (and we have a Pat pattern for shift-by-one).
let hasSideEffects = 0 in {
def SHL64r1  : RI<0xD1, MRM4r, (outs GR64:$dst), (ins GR64:$src1),
                 "shl{q}\t$dst", [], IIC_SR>;
} // hasSideEffects = 0
} // isConvertibleToThreeAddress = 1
} // Constraints = "$src = $dst", SchedRW


let SchedRW = [WriteShiftLd, WriteRMW] in {
// FIXME: Why do we need an explicit "Uses = [CL]" when the instr has a pattern
// using CL?
let Uses = [RCX] in {
def SHL64mCL : RI<0xD3, MRM4m, (outs), (ins i64mem:$dst),
                  "shl{q}\t{%cl, $dst|$dst, rcx}",
                  [(store (shl (loadi64 addr:$dst), RCX), addr:$dst)], IIC_SR>;
}
def SHL64mi : RIi8<0xC1, MRM4m, (outs), (ins i64mem:$dst, i8imm:$src),
                  "shl{q}\t{$src, $dst|$dst, $src}",
                 [(store (shl (loadi64 addr:$dst), (i8 imm:$src)), addr:$dst)],
                 IIC_SR>;

// Shift by 1
def SHL64m1 : RI<0xD1, MRM4m, (outs), (ins i64mem:$dst),
                  "shl{q}\t$dst",
                 [(store (shl (loadi64 addr:$dst), (i8 1)), addr:$dst)],
                 IIC_SR>;
} // SchedRW

let Constraints = "$src1 = $dst", SchedRW = [WriteShift] in {
let Uses = [RCX] in {
def SHR64rCL : RI<0xD3, MRM5r, (outs GR64:$dst), (ins GR64:$src1),
                  "shr{q}\t{%cl, $dst|$dst, rcx}",
                  [(set GR64:$dst, (srl GR64:$src1, RCX))], IIC_SR>;
}

def SHR64ri : RIi8<0xC1, MRM5r, (outs GR64:$dst), (ins GR64:$src1, i8imm:$src2),
                  "shr{q}\t{$src2, $dst|$dst, $src2}",
                  [(set GR64:$dst, (srl GR64:$src1, (i8 imm:$src2)))], IIC_SR>;

// Shift right by 1
def SHR64r1  : RI<0xD1, MRM5r, (outs GR64:$dst), (ins GR64:$src1),
                 "shr{q}\t$dst",
                 [(set GR64:$dst, (srl GR64:$src1, (i8 1)))], IIC_SR>;
} // Constraints = "$src = $dst", SchedRW


let SchedRW = [WriteShiftLd, WriteRMW] in {
let Uses = [RCX] in {
def SHR64mCL : RI<0xD3, MRM5m, (outs), (ins i64mem:$dst),
                  "shr{q}\t{%cl, $dst|$dst, rcx}",
                  [(store (srl (loadi64 addr:$dst), RCX), addr:$dst)], IIC_SR>;
}
def SHR64mi : RIi8<0xC1, MRM5m, (outs), (ins i64mem:$dst, i8imm:$src),
                  "shr{q}\t{$src, $dst|$dst, $src}",
                 [(store (srl (loadi64 addr:$dst), (i8 imm:$src)), addr:$dst)],
                 IIC_SR>;

// Shift by 1
def SHR64m1 : RI<0xD1, MRM5m, (outs), (ins i64mem:$dst),
                  "shr{q}\t$dst",
                 [(store (srl (loadi64 addr:$dst), (i8 1)), addr:$dst)],
                 IIC_SR>;
} // SchedRW

let Constraints = "$src1 = $dst", SchedRW = [WriteShift] in {
let Uses = [RCX] in {
def SAR64rCL : RI<0xD3, MRM7r, (outs GR64:$dst), (ins GR64:$src1),
                 "sar{q}\t{%cl, $dst|$dst, rcx}",
                 [(set GR64:$dst, (sra GR64:$src1, RCX))],
                 IIC_SR>;
}

def SAR64ri  : RIi8<0xC1, MRM7r, (outs GR64:$dst),
                    (ins GR64:$src1, i8imm:$src2),
                    "sar{q}\t{$src2, $dst|$dst, $src2}",
                    [(set GR64:$dst, (sra GR64:$src1, (i8 imm:$src2)))],
                    IIC_SR>;

// Shift by 1
def SAR64r1  : RI<0xD1, MRM7r, (outs GR64:$dst), (ins GR64:$src1),
                 "sar{q}\t$dst",
                 [(set GR64:$dst, (sra GR64:$src1, (i8 1)))],
                 IIC_SR>;
} // Constraints = "$src = $dst", SchedRW


let SchedRW = [WriteShiftLd, WriteRMW] in {
let Uses = [RCX] in {
def SAR64mCL : RI<0xD3, MRM7m, (outs), (ins i64mem:$dst), 
                 "sar{q}\t{%cl, $dst|$dst, rcx}",
                 [(store (sra (loadi64 addr:$dst), RCX), addr:$dst)],
                 IIC_SR>;
}
def SAR64mi  : RIi8<0xC1, MRM7m, (outs), (ins i64mem:$dst, i8imm:$src),
                    "sar{q}\t{$src, $dst|$dst, $src}",
                 [(store (sra (loadi64 addr:$dst), (i8 imm:$src)), addr:$dst)],
                 IIC_SR>;

// Shift by 1
def SAR64m1 : RI<0xD1, MRM7m, (outs), (ins i64mem:$dst),
                  "sar{q}\t$dst",
                 [(store (sra (loadi64 addr:$dst), (i8 1)), addr:$dst)],
                 IIC_SR>;
} // SchedRW

//===----------------------------------------------------------------------===//
// Rotate instructions
//===----------------------------------------------------------------------===//

let hasSideEffects = 0 in {
let Constraints = "$src1 = $dst", SchedRW = [WriteShift] in {
def RCL64r1 : RI<0xD1, MRM2r, (outs GR64:$dst), (ins GR64:$src1),
                 "rcl{q}\t$dst", [], IIC_SR>;
def RCL64ri : RIi8<0xC1, MRM2r, (outs GR64:$dst), (ins GR64:$src1, i8imm:$cnt),
                   "rcl{q}\t{$cnt, $dst|$dst, $cnt}", [], IIC_SR>;
let Uses = [RCX] in
def RCL64rCL : RI<0xD3, MRM2r, (outs GR64:$dst), (ins GR64:$src1),
                  "rcl{q}\t{%cl, $dst|$dst, rcx}", [], IIC_SR>;
def RCR64r1 : RI<0xD1, MRM3r, (outs GR64:$dst), (ins GR64:$src1),
                 "rcr{q}\t$dst", [], IIC_SR>;
def RCR64ri : RIi8<0xC1, MRM3r, (outs GR64:$dst), (ins GR64:$src1, i8imm:$cnt),
                   "rcr{q}\t{$cnt, $dst|$dst, $cnt}", [], IIC_SR>;
let Uses = [RCX] in
def RCR64rCL : RI<0xD3, MRM3r, (outs GR64:$dst), (ins GR64:$src1),
                  "rcr{q}\t{%cl, $dst|$dst, rcx}", [], IIC_SR>;

} // Constraints = "$src = $dst"

let SchedRW = [WriteShiftLd, WriteRMW] in {
def RCL64m1 : RI<0xD1, MRM2m, (outs), (ins i64mem:$dst),
                 "rcl{q}\t$dst", [], IIC_SR>;
def RCL64mi : RIi8<0xC1, MRM2m, (outs), (ins i64mem:$dst, i8imm:$cnt),
                   "rcl{q}\t{$cnt, $dst|$dst, $cnt}", [], IIC_SR>;

def RCR64m1 : RI<0xD1, MRM3m, (outs), (ins i64mem:$dst),
                 "rcr{q}\t$dst", [], IIC_SR>;
def RCR64mi : RIi8<0xC1, MRM3m, (outs), (ins i64mem:$dst, i8imm:$cnt),
                   "rcr{q}\t{$cnt, $dst|$dst, $cnt}", [], IIC_SR>;

let Uses = [RCX] in {
def RCL64mCL : RI<0xD3, MRM2m, (outs), (ins i64mem:$dst),
                  "rcl{q}\t{%cl, $dst|$dst, rcx}", [], IIC_SR>;
def RCR64mCL : RI<0xD3, MRM3m, (outs), (ins i64mem:$dst),
                  "rcr{q}\t{%cl, $dst|$dst, rcx}", [], IIC_SR>;
}
} // SchedRW
} // hasSideEffects = 0

let Constraints = "$src1 = $dst", SchedRW = [WriteShift] in {
// FIXME: provide shorter instructions when imm8 == 1
let Uses = [RCX] in {
def ROL64rCL : RI<0xD3, MRM0r, (outs GR64:$dst), (ins GR64:$src1),
                  "rol{q}\t{%cl, $dst|$dst, rcx}",
                  [(set GR64:$dst, (rotl GR64:$src1, RCX))], IIC_SR>;
}

def ROL64ri  : RIi8<0xC1, MRM0r, (outs GR64:$dst), 
                    (ins GR64:$src1, i8imm:$src2),
                    "rol{q}\t{$src2, $dst|$dst, $src2}",
                    [(set GR64:$dst, (rotl GR64:$src1, (i8 imm:$src2)))],
                    IIC_SR>;

// Rotate by 1
def ROL64r1  : RI<0xD1, MRM0r, (outs GR64:$dst), (ins GR64:$src1),
                  "rol{q}\t$dst",
                  [(set GR64:$dst, (rotl GR64:$src1, (i8 1)))],
                  IIC_SR>;
} // Constraints = "$src = $dst", SchedRW

let SchedRW = [WriteShiftLd, WriteRMW] in {
let Uses = [RCX] in {
def ROL64mCL :  RI<0xD3, MRM0m, (outs), (ins i64mem:$dst),
                   "rol{q}\t{%cl, $dst|$dst, rcx}",
                   [(store (rotl (loadi64 addr:$dst), RCX), addr:$dst)],
                   IIC_SR>;
}
def ROL64mi  : RIi8<0xC1, MRM0m, (outs), (ins i64mem:$dst, i8imm:$src1),
                    "rol{q}\t{$src1, $dst|$dst, $src1}",
                [(store (rotl (loadi64 addr:$dst), (i8 imm:$src1)), addr:$dst)],
                IIC_SR>;

// Rotate by 1
def ROL64m1  : RI<0xD1, MRM0m, (outs), (ins i64mem:$dst),
                 "rol{q}\t$dst",
               [(store (rotl (loadi64 addr:$dst), (i8 1)), addr:$dst)],
               IIC_SR>;
} // SchedRW

let Constraints = "$src1 = $dst", SchedRW = [WriteShift] in {
let Uses = [RCX] in {
def ROR64rCL : RI<0xD3, MRM1r, (outs GR64:$dst), (ins GR64:$src1),
                  "ror{q}\t{%cl, $dst|$dst, rcx}",
                  [(set GR64:$dst, (rotr GR64:$src1, RCX))], IIC_SR>;
}

def ROR64ri  : RIi8<0xC1, MRM1r, (outs GR64:$dst), 
                    (ins GR64:$src1, i8imm:$src2),
                    "ror{q}\t{$src2, $dst|$dst, $src2}",
                    [(set GR64:$dst, (rotr GR64:$src1, (i8 imm:$src2)))],
                    IIC_SR>;

// Rotate by 1
def ROR64r1  : RI<0xD1, MRM1r, (outs GR64:$dst), (ins GR64:$src1),
                  "ror{q}\t$dst",
                  [(set GR64:$dst, (rotr GR64:$src1, (i8 1)))],
                  IIC_SR>;
} // Constraints = "$src = $dst", SchedRW

let SchedRW = [WriteShiftLd, WriteRMW] in {
let Uses = [RCX] in {
def ROR64mCL : RI<0xD3, MRM1m, (outs), (ins i64mem:$dst), 
                  "ror{q}\t{%cl, $dst|$dst, rcx}",
                  [(store (rotr (loadi64 addr:$dst), RCX), addr:$dst)],
                  IIC_SR>;
}
def ROR64mi  : RIi8<0xC1, MRM1m, (outs), (ins i64mem:$dst, i8imm:$src),
                    "ror{q}\t{$src, $dst|$dst, $src}",
                [(store (rotr (loadi64 addr:$dst), (i8 imm:$src)), addr:$dst)],
                IIC_SR>;

// Rotate by 1
def ROR64m1  : RI<0xD1, MRM1m, (outs), (ins i64mem:$dst),
                 "ror{q}\t$dst",
               [(store (rotr (loadi64 addr:$dst), (i8 1)), addr:$dst)],
               IIC_SR>;
} // SchedRW


////===----------------------------------------------------------------------===//
//// Double shift instructions (generalizations of rotate)
////===----------------------------------------------------------------------===//
//
//let Constraints = "$src1 = $dst", SchedRW = [WriteShift] in {
//
//let Uses = [CL] in {
//def SHLD16rrCL : I<0xA5, MRMDestReg, (outs GR16:$dst), 
//                   (ins GR16:$src1, GR16:$src2),
//                   "shld{w}\t{%cl, $src2, $dst|$dst, $src2, cl}",
//                   [(set GR16:$dst, (Cse523shld GR16:$src1, GR16:$src2, CL))],
//                    IIC_SHD16_REG_CL>,
//                   TB, OpSize16;
//def SHRD16rrCL : I<0xAD, MRMDestReg, (outs GR16:$dst), 
//                   (ins GR16:$src1, GR16:$src2),
//                   "shrd{w}\t{%cl, $src2, $dst|$dst, $src2, cl}",
//                   [(set GR16:$dst, (Cse523shrd GR16:$src1, GR16:$src2, CL))],
//                    IIC_SHD16_REG_CL>,
//                   TB, OpSize16;
//def SHLD32rrCL : I<0xA5, MRMDestReg, (outs GR32:$dst), 
//                   (ins GR32:$src1, GR32:$src2),
//                   "shld{l}\t{%cl, $src2, $dst|$dst, $src2, cl}",
//                   [(set GR32:$dst, (Cse523shld GR32:$src1, GR32:$src2, CL))],
//                    IIC_SHD32_REG_CL>, TB, OpSize32;
//def SHRD32rrCL : I<0xAD, MRMDestReg, (outs GR32:$dst),
//                   (ins GR32:$src1, GR32:$src2),
//                   "shrd{l}\t{%cl, $src2, $dst|$dst, $src2, cl}",
//                   [(set GR32:$dst, (Cse523shrd GR32:$src1, GR32:$src2, CL))],
//                   IIC_SHD32_REG_CL>, TB, OpSize32;
//def SHLD64rrCL : RI<0xA5, MRMDestReg, (outs GR64:$dst), 
//                    (ins GR64:$src1, GR64:$src2),
//                    "shld{q}\t{%cl, $src2, $dst|$dst, $src2, cl}",
//                    [(set GR64:$dst, (Cse523shld GR64:$src1, GR64:$src2, CL))],
//                    IIC_SHD64_REG_CL>, 
//                    TB;
//def SHRD64rrCL : RI<0xAD, MRMDestReg, (outs GR64:$dst), 
//                    (ins GR64:$src1, GR64:$src2),
//                    "shrd{q}\t{%cl, $src2, $dst|$dst, $src2, cl}",
//                    [(set GR64:$dst, (Cse523shrd GR64:$src1, GR64:$src2, CL))],
//                    IIC_SHD64_REG_CL>, 
//                    TB;
//}
//
//let isCommutable = 1 in {  // These instructions commute to each other.
//def SHLD16rri8 : Ii8<0xA4, MRMDestReg,
//                     (outs GR16:$dst), 
//                     (ins GR16:$src1, GR16:$src2, i8imm:$src3),
//                     "shld{w}\t{$src3, $src2, $dst|$dst, $src2, $src3}",
//                     [(set GR16:$dst, (Cse523shld GR16:$src1, GR16:$src2,
//                                      (i8 imm:$src3)))], IIC_SHD16_REG_IM>,
//                     TB, OpSize16;
//def SHRD16rri8 : Ii8<0xAC, MRMDestReg,
//                     (outs GR16:$dst), 
//                     (ins GR16:$src1, GR16:$src2, i8imm:$src3),
//                     "shrd{w}\t{$src3, $src2, $dst|$dst, $src2, $src3}",
//                     [(set GR16:$dst, (Cse523shrd GR16:$src1, GR16:$src2,
//                                      (i8 imm:$src3)))], IIC_SHD16_REG_IM>,
//                     TB, OpSize16;
//def SHLD32rri8 : Ii8<0xA4, MRMDestReg,
//                     (outs GR32:$dst), 
//                     (ins GR32:$src1, GR32:$src2, i8imm:$src3),
//                     "shld{l}\t{$src3, $src2, $dst|$dst, $src2, $src3}",
//                     [(set GR32:$dst, (Cse523shld GR32:$src1, GR32:$src2,
//                                      (i8 imm:$src3)))], IIC_SHD32_REG_IM>,
//                 TB, OpSize32;
//def SHRD32rri8 : Ii8<0xAC, MRMDestReg,
//                     (outs GR32:$dst), 
//                     (ins GR32:$src1, GR32:$src2, i8imm:$src3),
//                     "shrd{l}\t{$src3, $src2, $dst|$dst, $src2, $src3}",
//                     [(set GR32:$dst, (Cse523shrd GR32:$src1, GR32:$src2,
//                                      (i8 imm:$src3)))], IIC_SHD32_REG_IM>,
//                 TB, OpSize32;
//def SHLD64rri8 : RIi8<0xA4, MRMDestReg,
//                      (outs GR64:$dst), 
//                      (ins GR64:$src1, GR64:$src2, i8imm:$src3),
//                      "shld{q}\t{$src3, $src2, $dst|$dst, $src2, $src3}",
//                      [(set GR64:$dst, (Cse523shld GR64:$src1, GR64:$src2,
//                                       (i8 imm:$src3)))], IIC_SHD64_REG_IM>,
//                 TB;
//def SHRD64rri8 : RIi8<0xAC, MRMDestReg,
//                      (outs GR64:$dst), 
//                      (ins GR64:$src1, GR64:$src2, i8imm:$src3),
//                      "shrd{q}\t{$src3, $src2, $dst|$dst, $src2, $src3}",
//                      [(set GR64:$dst, (Cse523shrd GR64:$src1, GR64:$src2,
//                                       (i8 imm:$src3)))], IIC_SHD64_REG_IM>,
//                 TB;
//}
//} // Constraints = "$src = $dst", SchedRW
//
//let SchedRW = [WriteShiftLd, WriteRMW] in {
//let Uses = [CL] in {
//def SHLD16mrCL : I<0xA5, MRMDestMem, (outs), (ins i16mem:$dst, GR16:$src2),
//                   "shld{w}\t{%cl, $src2, $dst|$dst, $src2, cl}",
//                   [(store (Cse523shld (loadi16 addr:$dst), GR16:$src2, CL),
//                     addr:$dst)], IIC_SHD16_MEM_CL>, TB, OpSize16;
//def SHRD16mrCL : I<0xAD, MRMDestMem, (outs), (ins i16mem:$dst, GR16:$src2),
//                  "shrd{w}\t{%cl, $src2, $dst|$dst, $src2, cl}",
//                  [(store (Cse523shrd (loadi16 addr:$dst), GR16:$src2, CL),
//                    addr:$dst)], IIC_SHD16_MEM_CL>, TB, OpSize16;
//
//def SHLD32mrCL : I<0xA5, MRMDestMem, (outs), (ins i32mem:$dst, GR32:$src2),
//                   "shld{l}\t{%cl, $src2, $dst|$dst, $src2, cl}",
//                   [(store (Cse523shld (loadi32 addr:$dst), GR32:$src2, CL),
//                     addr:$dst)], IIC_SHD32_MEM_CL>, TB, OpSize32;
//def SHRD32mrCL : I<0xAD, MRMDestMem, (outs), (ins i32mem:$dst, GR32:$src2),
//                  "shrd{l}\t{%cl, $src2, $dst|$dst, $src2, cl}",
//                  [(store (Cse523shrd (loadi32 addr:$dst), GR32:$src2, CL),
//                    addr:$dst)], IIC_SHD32_MEM_CL>, TB, OpSize32;
//                    
//def SHLD64mrCL : RI<0xA5, MRMDestMem, (outs), (ins i64mem:$dst, GR64:$src2),
//                    "shld{q}\t{%cl, $src2, $dst|$dst, $src2, cl}",
//                    [(store (Cse523shld (loadi64 addr:$dst), GR64:$src2, CL),
//                      addr:$dst)], IIC_SHD64_MEM_CL>, TB;
//def SHRD64mrCL : RI<0xAD, MRMDestMem, (outs), (ins i64mem:$dst, GR64:$src2),
//                    "shrd{q}\t{%cl, $src2, $dst|$dst, $src2, cl}",
//                    [(store (Cse523shrd (loadi64 addr:$dst), GR64:$src2, CL),
//                      addr:$dst)], IIC_SHD64_MEM_CL>, TB;
//}
//
//def SHLD16mri8 : Ii8<0xA4, MRMDestMem,
//                    (outs), (ins i16mem:$dst, GR16:$src2, i8imm:$src3),
//                    "shld{w}\t{$src3, $src2, $dst|$dst, $src2, $src3}",
//                    [(store (Cse523shld (loadi16 addr:$dst), GR16:$src2,
//                                      (i8 imm:$src3)), addr:$dst)],
//                                      IIC_SHD16_MEM_IM>,
//                    TB, OpSize16;
//def SHRD16mri8 : Ii8<0xAC, MRMDestMem, 
//                     (outs), (ins i16mem:$dst, GR16:$src2, i8imm:$src3),
//                     "shrd{w}\t{$src3, $src2, $dst|$dst, $src2, $src3}",
//                    [(store (Cse523shrd (loadi16 addr:$dst), GR16:$src2,
//                                      (i8 imm:$src3)), addr:$dst)],
//                                      IIC_SHD16_MEM_IM>,
//                     TB, OpSize16;
//
//def SHLD32mri8 : Ii8<0xA4, MRMDestMem,
//                    (outs), (ins i32mem:$dst, GR32:$src2, i8imm:$src3),
//                    "shld{l}\t{$src3, $src2, $dst|$dst, $src2, $src3}",
//                    [(store (Cse523shld (loadi32 addr:$dst), GR32:$src2,
//                                      (i8 imm:$src3)), addr:$dst)],
//                                      IIC_SHD32_MEM_IM>,
//                    TB, OpSize32;
//def SHRD32mri8 : Ii8<0xAC, MRMDestMem, 
//                     (outs), (ins i32mem:$dst, GR32:$src2, i8imm:$src3),
//                     "shrd{l}\t{$src3, $src2, $dst|$dst, $src2, $src3}",
//                     [(store (Cse523shrd (loadi32 addr:$dst), GR32:$src2,
//                                       (i8 imm:$src3)), addr:$dst)],
//                                       IIC_SHD32_MEM_IM>,
//                     TB, OpSize32;
//
//def SHLD64mri8 : RIi8<0xA4, MRMDestMem,
//                      (outs), (ins i64mem:$dst, GR64:$src2, i8imm:$src3),
//                      "shld{q}\t{$src3, $src2, $dst|$dst, $src2, $src3}",
//                      [(store (Cse523shld (loadi64 addr:$dst), GR64:$src2,
//                                       (i8 imm:$src3)), addr:$dst)],
//                                       IIC_SHD64_MEM_IM>,
//                 TB;
//def SHRD64mri8 : RIi8<0xAC, MRMDestMem, 
//                      (outs), (ins i64mem:$dst, GR64:$src2, i8imm:$src3),
//                      "shrd{q}\t{$src3, $src2, $dst|$dst, $src2, $src3}",
//                      [(store (Cse523shrd (loadi64 addr:$dst), GR64:$src2,
//                                       (i8 imm:$src3)), addr:$dst)],
//                                       IIC_SHD64_MEM_IM>,
//                 TB;
//} // SchedRW

} // Defs = [EFLAGS]
//
//def ROT32L2R_imm8  : SDNodeXForm<imm, [{
//  // Convert a ROTL shamt to a ROTR shamt on 32-bit integer.
//  return getI8Imm(32 - N->getZExtValue());
//}]>;
//
//def ROT64L2R_imm8  : SDNodeXForm<imm, [{
//  // Convert a ROTL shamt to a ROTR shamt on 64-bit integer.
//  return getI8Imm(64 - N->getZExtValue());
//}]>;
//
//multiclass bmi_rotate<string asm, RegisterClass RC, Cse523MemOperand cse523memop> {
//let neverHasSideEffects = 1 in {
//  def ri : Ii8<0xF0, MRMSrcReg, (outs RC:$dst), (ins RC:$src1, i8imm:$src2),
//               !strconcat(asm, "\t{$src2, $src1, $dst|$dst, $src1, $src2}"),
//               []>, TAXD, VEX, Sched<[WriteShift]>;
//  let mayLoad = 1 in
//  def mi : Ii8<0xF0, MRMSrcMem, (outs RC:$dst),
//               (ins cse523memop:$src1, i8imm:$src2),
//               !strconcat(asm, "\t{$src2, $src1, $dst|$dst, $src1, $src2}"),
//               []>, TAXD, VEX, Sched<[WriteShiftLd]>;
//}
//}
//
//multiclass bmi_shift<string asm, RegisterClass RC, Cse523MemOperand cse523memop> {
//let neverHasSideEffects = 1 in {
//  def rr : I<0xF7, MRMSrcReg, (outs RC:$dst), (ins RC:$src1, RC:$src2),
//             !strconcat(asm, "\t{$src2, $src1, $dst|$dst, $src1, $src2}"), []>,
//             VEX_4VOp3, Sched<[WriteShift]>;
//  let mayLoad = 1 in
//  def rm : I<0xF7, MRMSrcMem, (outs RC:$dst), (ins cse523memop:$src1, RC:$src2),
//             !strconcat(asm, "\t{$src2, $src1, $dst|$dst, $src1, $src2}"), []>,
//             VEX_4VOp3,
//             Sched<[WriteShiftLd,
//                    // cse523memop:$src1
//                    ReadDefault, ReadDefault, ReadDefault, ReadDefault,
//                    ReadDefault,
//                    // RC:$src1
//                    ReadAfterLd]>;
//}
//}
//
//let Predicates = [HasBMI2] in {
//  defm RORX32 : bmi_rotate<"rorx{l}", GR32, i32mem>;
//  defm RORX64 : bmi_rotate<"rorx{q}", GR64, i64mem>, VEX_W;
//  defm SARX32 : bmi_shift<"sarx{l}", GR32, i32mem>, T8XS;
//  defm SARX64 : bmi_shift<"sarx{q}", GR64, i64mem>, T8XS, VEX_W;
//  defm SHRX32 : bmi_shift<"shrx{l}", GR32, i32mem>, T8XD;
//  defm SHRX64 : bmi_shift<"shrx{q}", GR64, i64mem>, T8XD, VEX_W;
//  defm SHLX32 : bmi_shift<"shlx{l}", GR32, i32mem>, T8PD;
//  defm SHLX64 : bmi_shift<"shlx{q}", GR64, i64mem>, T8PD, VEX_W;
//
//  // Prefer RORX which is non-destructive and doesn't update EFLAGS.
//  let AddedComplexity = 10 in {
//    def : Pat<(rotl GR32:$src, (i8 imm:$shamt)),
//              (RORX32ri GR32:$src, (ROT32L2R_imm8 imm:$shamt))>;
//    def : Pat<(rotl GR64:$src, (i8 imm:$shamt)),
//              (RORX64ri GR64:$src, (ROT64L2R_imm8 imm:$shamt))>;
//  }
//
//  def : Pat<(rotl (loadi32 addr:$src), (i8 imm:$shamt)),
//            (RORX32mi addr:$src, (ROT32L2R_imm8 imm:$shamt))>;
//  def : Pat<(rotl (loadi64 addr:$src), (i8 imm:$shamt)),
//            (RORX64mi addr:$src, (ROT64L2R_imm8 imm:$shamt))>;
//
//  // Prefer SARX/SHRX/SHLX over SAR/SHR/SHL with variable shift BUT not
//  // immedidate shift, i.e. the following code is considered better
//  //
//  //  mov %edi, %esi
//  //  shl $imm, %esi
//  //  ... %edi, ...
//  //
//  // than
//  //
//  //  movb $imm, %sil
//  //  shlx %sil, %edi, %esi
//  //  ... %edi, ...
//  //
//  let AddedComplexity = 1 in {
//    def : Pat<(sra GR32:$src1, GR8:$src2),
//              (SARX32rr GR32:$src1,
//                        (INSERT_SUBREG
//                          (i32 (IMPLICIT_DEF)), GR8:$src2, sub_8bit))>;
//    def : Pat<(sra GR64:$src1, GR8:$src2),
//              (SARX64rr GR64:$src1,
//                        (INSERT_SUBREG
//                          (i64 (IMPLICIT_DEF)), GR8:$src2, sub_8bit))>;
//
//    def : Pat<(srl GR32:$src1, GR8:$src2),
//              (SHRX32rr GR32:$src1,
//                        (INSERT_SUBREG
//                          (i32 (IMPLICIT_DEF)), GR8:$src2, sub_8bit))>;
//    def : Pat<(srl GR64:$src1, GR8:$src2),
//              (SHRX64rr GR64:$src1,
//                        (INSERT_SUBREG
//                          (i64 (IMPLICIT_DEF)), GR8:$src2, sub_8bit))>;
//
//    def : Pat<(shl GR32:$src1, GR8:$src2),
//              (SHLX32rr GR32:$src1,
//                        (INSERT_SUBREG
//                          (i32 (IMPLICIT_DEF)), GR8:$src2, sub_8bit))>;
//    def : Pat<(shl GR64:$src1, GR8:$src2),
//              (SHLX64rr GR64:$src1,
//                        (INSERT_SUBREG
//                          (i64 (IMPLICIT_DEF)), GR8:$src2, sub_8bit))>;
//  }
//
//  // Patterns on SARXrm/SHRXrm/SHLXrm are explicitly omitted to favor
//  //
//  //  mov (%ecx), %esi
//  //  shl $imm, $esi
//  //
//  // over
//  //
//  //  movb $imm %al
//  //  shlx %al, (%ecx), %esi
//  //
//  // As SARXrr/SHRXrr/SHLXrr is favored on variable shift, the peephole
//  // optimization will fold them into SARXrm/SHRXrm/SHLXrm if possible.
//}
