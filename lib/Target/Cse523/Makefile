##===- lib/Target/Cse523/Makefile ---------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMCse523CodeGen
TARGET = Cse523

# Make sure that tblgen is run, first thing.
#BUILT_SOURCES = Cse523GenRegisterInfo.inc Cse523GenInstrInfo.inc \
#		Cse523GenAsmWriter.inc Cse523GenAsmMatcher.inc \
#		Cse523GenDAGISel.inc Cse523GenDisassemblerTables.inc \
#		Cse523GenSubtargetInfo.inc Cse523GenCallingConv.inc \
#		Cse523GenCodeEmitter.inc Cse523GenMCCodeEmitter.inc

#DIRS = InstPrinter AsmParser Disassembler TargetInfo MCTargetDesc
DIRS = InstPrinter AsmParser TargetInfo MCTargetDesc

include $(LEVEL)/Makefile.common

