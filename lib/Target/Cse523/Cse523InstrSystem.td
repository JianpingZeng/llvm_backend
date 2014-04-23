//===-- Cse523InstrSystem.td - System Instructions ------------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the Cse523 instructions that are generally used in
// privileged modes.  These are not typically used by the compiler, but are
// supported for the assembler and disassembler.
//
//===----------------------------------------------------------------------===//

let SchedRW = [WriteSystem] in {
let Defs = [RAX, RDX] in
  def RDTSC : I<0x31, RawFrm, (outs), (ins), "rdtsc", [(Cse523rdtsc)], IIC_RDTSC>,
              TB;

let Defs = [RAX, RCX, RDX] in
  def RDTSCP : I<0x01, MRM_F9, (outs), (ins), "rdtscp", []>, TB;

// CPU flow control instructions

let isTerminator = 1, isBarrier = 1, hasCtrlDep = 1 in {
  def TRAP    : I<0x0B, RawFrm, (outs), (ins), "ud2", [(trap)]>, TB;
  def UD2B    : I<0xB9, RawFrm, (outs), (ins), "ud2b", []>, TB;
}

def HLT : I<0xF4, RawFrm, (outs), (ins), "hlt", [], IIC_HLT>;
def RSM : I<0xAA, RawFrm, (outs), (ins), "rsm", [], IIC_RSM>, TB;

// Interrupt and SysCall Instructions.
let Uses = [EFLAGS] in
  def INTO : I<0xce, RawFrm, (outs), (ins), "into", []>;
} // SchedRW

// The long form of "int $3" turns into int3 as a size optimization.
// FIXME: This doesn't work because InstAlias can't match immediate constants.
//def : InstAlias<"int\t$3", (INT3)>;

let SchedRW = [WriteSystem] in {
def SYSCALL  : I<0x05, RawFrm, (outs), (ins), "syscall", [], IIC_SYSCALL>, TB;
def SYSRET64 :RI<0x07, RawFrm, (outs), (ins), "sysret{q}", [], IIC_SYSCALL>, TB,
               Requires<[In64BitMode]>;

def SYSENTER : I<0x34, RawFrm, (outs), (ins), "sysenter", [],
                 IIC_SYS_ENTER_EXIT>, TB;

def SYSEXIT64 :RI<0x35, RawFrm, (outs), (ins), "sysexit{q}", [],
                 IIC_SYS_ENTER_EXIT>, TB, Requires<[In64BitMode]>;

def IRET64 : RI<0xcf, RawFrm, (outs), (ins), "iretq", [], IIC_IRET>,
             Requires<[In64BitMode]>;
} // SchedRW


//===----------------------------------------------------------------------===//
//  Input/Output Instructions. inw outw not supported
//

//===----------------------------------------------------------------------===//
// Moves to and from debug registers

let SchedRW = [WriteSystem] in {
def MOV64rd : I<0x21, MRMDestReg, (outs GR64:$dst), (ins DEBUG_REG:$src),
                "mov{q}\t{$src, $dst|$dst, $src}", [], IIC_MOV_REG_DR>, TB,
                Requires<[In64BitMode]>;

def MOV64dr : I<0x23, MRMSrcReg, (outs DEBUG_REG:$dst), (ins GR64:$src),
                "mov{q}\t{$src, $dst|$dst, $src}", [], IIC_MOV_DR_REG>, TB,
                Requires<[In64BitMode]>;
} // SchedRW

//===----------------------------------------------------------------------===//
// Moves to and from control registers

let SchedRW = [WriteSystem] in {
def MOV64rc : I<0x20, MRMDestReg, (outs GR64:$dst), (ins CONTROL_REG:$src),
                "mov{q}\t{$src, $dst|$dst, $src}", [], IIC_MOV_REG_CR>, TB,
                Requires<[In64BitMode]>;

def MOV64cr : I<0x22, MRMSrcReg, (outs CONTROL_REG:$dst), (ins GR64:$src),
                "mov{q}\t{$src, $dst|$dst, $src}", [], IIC_MOV_CR_REG>, TB,
                Requires<[In64BitMode]>;
} // SchedRW

//===----------------------------------------------------------------------===//
// Segment override instruction prefixes

def CS_PREFIX : I<0x2E, RawFrm, (outs), (ins), "cs", []>;
def SS_PREFIX : I<0x36, RawFrm, (outs), (ins), "ss", []>;
def DS_PREFIX : I<0x3E, RawFrm, (outs), (ins), "ds", []>;
def ES_PREFIX : I<0x26, RawFrm, (outs), (ins), "es", []>;
def FS_PREFIX : I<0x64, RawFrm, (outs), (ins), "fs", []>;
def GS_PREFIX : I<0x65, RawFrm, (outs), (ins), "gs", []>;


//===----------------------------------------------------------------------===//
// Moves to and from segment registers.
//

let SchedRW = [WriteMove] in {
def MOV64rs : RI<0x8C, MRMDestReg, (outs GR64:$dst), (ins SEGMENT_REG:$src),
                 "mov{q}\t{$src, $dst|$dst, $src}", [], IIC_MOV_REG_SR>;

def MOV64ms : RI<0x8C, MRMDestMem, (outs i64mem:$dst), (ins SEGMENT_REG:$src),
                 "mov{q}\t{$src, $dst|$dst, $src}", [], IIC_MOV_MEM_SR>;

def MOV64sr : RI<0x8E, MRMSrcReg, (outs SEGMENT_REG:$dst), (ins GR64:$src),
                 "mov{q}\t{$src, $dst|$dst, $src}", [], IIC_MOV_SR_REG>;

def MOV64sm : RI<0x8E, MRMSrcMem, (outs SEGMENT_REG:$dst), (ins i64mem:$src),
                 "mov{q}\t{$src, $dst|$dst, $src}", [], IIC_MOV_SR_MEM>;
} // SchedRW

let SchedRW = [WriteSystem] in {

//===----------------------------------------------------------------------===//
// Segmentation support instructions. SWAPGS, LAR Not supported

// i16mem operand in LAR64rm and GR32 operand in LAR32rr is not a typo.
def LAR64rm : RI<0x02, MRMSrcMem, (outs GR64:$dst), (ins i16mem:$src), 
                 "lar{q}\t{$src, $dst|$dst, $src}", [], IIC_LAR_RM>, TB;

def LSL64rm : RI<0x03, MRMSrcMem, (outs GR64:$dst), (ins i64mem:$src),
                 "lsl{q}\t{$src, $dst|$dst, $src}", [], IIC_LSL_RM>, TB; 
def LSL64rr : RI<0x03, MRMSrcReg, (outs GR64:$dst), (ins GR64:$src),
                 "lsl{q}\t{$src, $dst|$dst, $src}", [], IIC_LSL_RR>, TB;

def INVLPG : I<0x01, MRM7m, (outs), (ins i8mem:$addr), "invlpg\t$addr",
               [], IIC_INVLPG>, TB;

def STR64r : RI<0x00, MRM1r, (outs GR64:$dst), (ins),
                "str{q}\t$dst", [], IIC_STR>, TB;
             
def PUSHFS64 : I<0xa0, RawFrm, (outs), (ins),
                 "push{q}\t{%fs|fs}", [], IIC_PUSH_SR>, TB,
               OpSize32, Requires<[In64BitMode]>;
def PUSHGS64 : I<0xa8, RawFrm, (outs), (ins),
                 "push{q}\t{%gs|gs}", [], IIC_PUSH_SR>, TB,
               OpSize32, Requires<[In64BitMode]>;

def POPFS64 : I<0xa1, RawFrm, (outs), (ins),
                "pop{q}\t{%fs|fs}", [], IIC_POP_SR>, TB,
              OpSize32, Requires<[In64BitMode]>;

def POPGS64 : I<0xa9, RawFrm, (outs), (ins),
                "pop{q}\t{%gs|gs}", [], IIC_POP_SR>, TB,
              OpSize32, Requires<[In64BitMode]>;

def LSS64rm : RI<0xb2, MRMSrcMem, (outs GR64:$dst), (ins opaque80mem:$src),
                 "lss{q}\t{$src, $dst|$dst, $src}", [], IIC_LXS>, TB;
                
def LFS64rm : RI<0xb4, MRMSrcMem, (outs GR64:$dst), (ins opaque80mem:$src),
                 "lfs{q}\t{$src, $dst|$dst, $src}", [], IIC_LXS>, TB;
                
def LGS64rm : RI<0xb5, MRMSrcMem, (outs GR64:$dst), (ins opaque80mem:$src),
                 "lgs{q}\t{$src, $dst|$dst, $src}", [], IIC_LXS>, TB;

} // SchedRW

//===----------------------------------------------------------------------===//
// Descriptor-table support instructions

let SchedRW = [WriteSystem] in {
def SGDT64m : I<0x01, MRM0m, (outs opaque80mem:$dst), (ins),
              "sgdt{q}\t$dst", [], IIC_SGDT>, TB, Requires <[In64BitMode]>;
def SIDT64m : I<0x01, MRM1m, (outs opaque80mem:$dst), (ins),
              "sidt{q}\t$dst", []>, TB, Requires <[In64BitMode]>;
                
// LLDT is not interpreted specially in 64-bit mode because there is no sign
//   extension.
def SLDT64r : RI<0x00, MRM0r, (outs GR64:$dst), (ins),
                 "sldt{q}\t$dst", [], IIC_SLDT>, TB;
def SLDT64m : RI<0x00, MRM0m, (outs i16mem:$dst), (ins),
                 "sldt{q}\t$dst", [], IIC_SLDT>, TB;

def LGDT64m : I<0x01, MRM2m, (outs), (ins opaque80mem:$src),
              "lgdt{q}\t$src", [], IIC_LGDT>, TB, Requires<[In64BitMode]>;
def LIDT64m : I<0x01, MRM3m, (outs), (ins opaque80mem:$src),
              "lidt{q}\t$src", [], IIC_LIDT>, TB, Requires<[In64BitMode]>;
} // SchedRW

//===----------------------------------------------------------------------===//
// Specialized register support
let SchedRW = [WriteSystem] in {
def WRMSR : I<0x30, RawFrm, (outs), (ins), "wrmsr", [], IIC_WRMSR>, TB;
def RDMSR : I<0x32, RawFrm, (outs), (ins), "rdmsr", [], IIC_RDMSR>, TB;
def RDPMC : I<0x33, RawFrm, (outs), (ins), "rdpmc", [], IIC_RDPMC>, TB;

// no m form encodable; use SMSW16m
def SMSW64r : RI<0x01, MRM4r, (outs GR64:$dst), (ins), 
                 "smsw{q}\t$dst", [], IIC_SMSW>, TB;

let Defs = [RAX, RBX, RCX, RDX], Uses = [RAX, RCX] in
  def CPUID64 : I<0xA2, RawFrm, (outs), (ins), "cpuid", [], IIC_CPUID>, TB,
  Requires<[In64BitMode]>;
} // SchedRW

//===----------------------------------------------------------------------===//
// Cache instructions
let SchedRW = [WriteSystem] in {
def INVD : I<0x08, RawFrm, (outs), (ins), "invd", [], IIC_INVD>, TB;
def WBINVD : I<0x09, RawFrm, (outs), (ins), "wbinvd", [], IIC_INVD>, TB;
} // SchedRW

//===----------------------------------------------------------------------===//
// XSAVE instructions: Not supported

//===----------------------------------------------------------------------===//
// VIA PadLock crypto instructions: Not supported

//===----------------------------------------------------------------------===//
// FS/GS Base Instructions: Not Supported

//===----------------------------------------------------------------------===//
// INVPCID Instruction: Not Supported

//===----------------------------------------------------------------------===//
// SMAP Instruction
let Defs = [EFLAGS], Uses = [EFLAGS] in {
  def CLAC : I<0x01, MRM_CA, (outs), (ins), "clac", []>, TB;
  def STAC : I<0x01, MRM_CB, (outs), (ins), "stac", []>, TB;
}
