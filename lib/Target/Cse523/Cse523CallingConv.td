//===-- Cse523CallingConv.td - Calling Conventions Cse523 32/64 --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the Cse523-32 and Cse523-64
// architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Return Value Calling Convention
//===----------------------------------------------------------------------===//

def RetCC_Cse523 : CallingConv<[

  CCIfType<[i8, i16, i32], CCPromoteToType<i64>>,
  CCIfType<[i64], CCAssignToReg<[RAX, RDX, RCX]>>
]>;

//===----------------------------------------------------------------------===//
// Calling Convention
//===----------------------------------------------------------------------===//

def CC_Cse523 : CallingConv<[
  // Handles byval parameters. <bytes, alignment>
  CCIfByVal<CCPassByVal<8, 8>>,

  // Promote i8/i16 arguments to i32.
  CCIfType<[i8, i16, i32], CCPromoteToType<i64>>,

  // The 'nest' parameter, if any, is passed in R10.
  CCIfNest<CCAssignToReg<[R10]>>,

  // The first 6 integer arguments are passed in integer registers.
  CCIfType<[i64], CCAssignToReg<[RDI, RSI, RDX, RCX, R8 , R9 ]>>,

  // Integer/FP values get stored in stack slots that are 8 bytes in size and
  // 8-byte aligned if there are no more registers to hold them.
  CCIfType<[i64], CCAssignToStack<8, 8>>
]>;

//===----------------------------------------------------------------------===//
// Callee-saved Registers.
//===----------------------------------------------------------------------===//

def CSR_NoRegs : CalleeSavedRegs<(add)>;

def CSR_64 : CalleeSavedRegs<(add RBX, R12, R13, R14, R15, RBP)>;

def CSR_64EHRet : CalleeSavedRegs<(add RAX, RDX, CSR_64)>;

// All GPRs - except r11
def CSR_64_RT_MostRegs : CalleeSavedRegs<(add CSR_64, RAX, RCX, RDX, RSI, RDI,
                                              R8, R9, R10, RSP)>;

// All registers - except r11
def CSR_64_RT_AllRegs     : CalleeSavedRegs<(add CSR_64_RT_MostRegs)>;

def CSR_64_MostRegs : CalleeSavedRegs<(add RBX, RCX, RDX, RSI, RDI, R8, R9, R10,
                                           R11, R12, R13, R14, R15, RBP)>;

def CSR_64_AllRegs     : CalleeSavedRegs<(add CSR_64_MostRegs, RAX, RSP)>;
