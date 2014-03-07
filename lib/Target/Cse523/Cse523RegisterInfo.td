//===- Cse523RegisterInfo.td - Describe the Cse523 Register File --*- tablegen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the Cse523 Register file, defining the registers themselves,
// aliases between the registers, and the register classes built out of the
// registers.
//
//===----------------------------------------------------------------------===//

class Cse523Reg<string n, bits<16> Enc, list<Register> subregs = []> : Register<n> {
  let Namespace = "Cse523";
  let HWEncoding = Enc;
  let SubRegs = subregs;
}

//===----------------------------------------------------------------------===//
//  Register definitions...
//

// In the register alias definitions below, we define which registers alias
// which others.  We only specify which registers the small registers alias,
// because the register file generator is smart enough to figure out that
// AL aliases AX if we tell it that AX aliased AL (for example).

// Dwarf numbering is different for 32-bit and 64-bit, and there are
// variations by target as well. Currently the first entry is for Cse523-64,
// second - for EH on Cse523-32/Darwin and third is 'generic' one (Cse523-32/Linux
// and debug information on Cse523-32/Darwin)

// 64-bit registers, Cse523-64 only
let Namespace = "Cse523" in {
def RAX : Cse523Reg<"rax", 0>, DwarfRegNum<[0]>;
def RDX : Cse523Reg<"rdx", 2>, DwarfRegNum<[1]>;
def RCX : Cse523Reg<"rcx", 1>, DwarfRegNum<[2]>;
def RBX : Cse523Reg<"rbx", 3>, DwarfRegNum<[3]>;
def RSI : Cse523Reg<"rsi", 6>, DwarfRegNum<[4]>;
def RDI : Cse523Reg<"rdi", 7>, DwarfRegNum<[5]>;
def RBP : Cse523Reg<"rbp", 5>, DwarfRegNum<[6]>;
def RSP : Cse523Reg<"rsp", 4>, DwarfRegNum<[7]>;

// These also require REX.
let CostPerUse = 1 in {
def R8  : Cse523Reg<"r8",   8>, DwarfRegNum<[8]>;
def R9  : Cse523Reg<"r9",   9>, DwarfRegNum<[9]>;
def R10 : Cse523Reg<"r10", 10>, DwarfRegNum<[10]>;
def R11 : Cse523Reg<"r11", 11>, DwarfRegNum<[11]>;
def R12 : Cse523Reg<"r12", 12>, DwarfRegNum<[12]>;
def R13 : Cse523Reg<"r13", 13>, DwarfRegNum<[13]>;
def R14 : Cse523Reg<"r14", 14>, DwarfRegNum<[14]>;
def R15 : Cse523Reg<"r15", 15>, DwarfRegNum<[15]>;
def RIP : Cse523Reg<"rip",  0>, DwarfRegNum<[16]>;
}


// Status flags register
def RFLAGS : Cse523Reg<"flags", 0>;

// Segment registers
def CS : Cse523Reg<"cs", 1>;
def DS : Cse523Reg<"ds", 3>;
def SS : Cse523Reg<"ss", 2>;
def ES : Cse523Reg<"es", 0>;
def FS : Cse523Reg<"fs", 4>;
def GS : Cse523Reg<"gs", 5>;

// Debug registers
def DR0 : Cse523Reg<"dr0", 0>;
def DR1 : Cse523Reg<"dr1", 1>;
def DR2 : Cse523Reg<"dr2", 2>;
def DR3 : Cse523Reg<"dr3", 3>;
def DR4 : Cse523Reg<"dr4", 4>;
def DR5 : Cse523Reg<"dr5", 5>;
def DR6 : Cse523Reg<"dr6", 6>;
def DR7 : Cse523Reg<"dr7", 7>;

// Control registers
def CR0  : Cse523Reg<"cr0",   0>;
def CR1  : Cse523Reg<"cr1",   1>;
def CR2  : Cse523Reg<"cr2",   2>;
def CR3  : Cse523Reg<"cr3",   3>;
def CR4  : Cse523Reg<"cr4",   4>;
def CR5  : Cse523Reg<"cr5",   5>;
def CR6  : Cse523Reg<"cr6",   6>;
def CR7  : Cse523Reg<"cr7",   7>;
def CR8  : Cse523Reg<"cr8",   8>;
def CR9  : Cse523Reg<"cr9",   9>;
def CR10 : Cse523Reg<"cr10", 10>;
def CR11 : Cse523Reg<"cr11", 11>;
def CR12 : Cse523Reg<"cr12", 12>;
def CR13 : Cse523Reg<"cr13", 13>;
def CR14 : Cse523Reg<"cr14", 14>;
def CR15 : Cse523Reg<"cr15", 15>;

// Pseudo index registers
def RIZ : Cse523Reg<"riz", 4>;
}

//===----------------------------------------------------------------------===//
// Register Class Definitions... now that we have all of the pieces, define the
// top-level register classes.  The order specified in the register list is
// implicitly defined to be the register allocation order.
//

// List call-clobbered registers before callee-save registers. RBX, RBP, (and
// R12, R13, R14, and R15 for Cse523-64) are callee-save registers.
// In 64-mode, there are 12 additional i8 registers, SIL, DIL, BPL, SPL, and
// R8B, ... R15B.
// Allocate R12 and R13 last, as these require an extra byte when
// encoded in x86_64 instructions.
// FIXME: Allow AH, CH, DH, BH to be used as general-purpose registers in
// 64-bit mode. The main complication is that they cannot be encoded in an
// instruction requiring a REX prefix, while SIL, DIL, BPL, R8D, etc.
// require a REX prefix. For example, "addb %ah, %dil" and "movzbl %ah, %r8d"
// cannot be encoded.

// GR64 - 64-bit GPRs. This oddly includes RIP, which isn't accurate, since
// RIP isn't really a register and it can't be used anywhere except in an
// address, but it doesn't cause trouble.
def GR64 : RegisterClass<"Cse523", [i64], 64,
                         (add RAX, RCX, RDX, RSI, RDI, R8, R9, R10, R11,
                              RBX, R14, R15, R12, R13, RBP, RSP, RIP)>;

// Segment registers for use by MOV instructions (and others) that have a
//   segment register as one operand.  Always contain a 16-bit segment
//   descriptor.
def SEGMENT_REG : RegisterClass<"Cse523", [i16], 16, (add CS, DS, SS, ES, FS, GS)>;

// Debug registers.
def DEBUG_REG : RegisterClass<"Cse523", [i32], 32, (sequence "DR%u", 0, 7)>;

// Control registers.
def CONTROL_REG : RegisterClass<"Cse523", [i64], 64, (sequence "CR%u", 0, 15)>;

// GR8_ABCD_L, GR8_ABCD_H, GR16_ABCD, GR32_ABCD, GR64_ABCD - Subclasses of
// GR8, GR16, GR32, and GR64 which contain just the "a" "b", "c", and "d"
// registers. On x86-32, GR16_ABCD and GR32_ABCD are classes for registers
// that support 8-bit subreg operations. On x86-64, GR16_ABCD, GR32_ABCD,
// and GR64_ABCD are classes for registers that support 8-bit h-register
// operations.
def GR64_ABCD : RegisterClass<"Cse523", [i64], 64, (add RAX, RCX, RDX, RBX)>;
def GR64_TC   : RegisterClass<"Cse523", [i64], 64, (add RAX, RCX, RDX, RSI, RDI,
                                                     R8, R9, R11, RIP)>;
def GR64_TCW64 : RegisterClass<"Cse523", [i64], 64, (add RAX, RCX, RDX,
                                                      R8, R9, R11)>;

// GR64_NOREX - GR64 registers which do not require a REX prefix.
def GR64_NOREX : RegisterClass<"Cse523", [i64], 64,
                            (add RAX, RCX, RDX, RSI, RDI, RBX, RBP, RSP, RIP)>;

// GR64_NOSP - GR64 registers except RSP (and RIP).
def GR64_NOSP : RegisterClass<"Cse523", [i64], 64, (sub GR64, RSP, RIP)>;

// GR64_NOREX_NOSP - GR64_NOREX registers except RSP.
def GR64_NOREX_NOSP : RegisterClass<"Cse523", [i64], 64,
                                    (and GR64_NOREX, GR64_NOSP)>;

// Status flags registers.
def CCR : RegisterClass<"Cse523", [i64], 64, (add RFLAGS)> {
  let CopyCost = -1;  // Don't allow copying of status registers.
  let isAllocatable = 0;
}

