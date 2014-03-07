//===-- Cse523InstPrinter.cpp - Convert Cse523 MCInst to assembly syntax -----==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an Cse523 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "Cse523InstPrinter.h"
#include "Cse523.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;


#define GET_INSTRUCTION_NAME
#define PRINT_ALIAS_INSTR
//#include "Cse523GenAsmWriter.inc"

void Cse523InstPrinter::printRegName(raw_ostream &OS, unsigned RegNo) const
{
  OS << '%' << StringRef(getRegisterName(RegNo)).lower();
}

void Cse523InstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                               StringRef Annot)
{
  if (!printAliasInstr(MI, O) && !printCse523AliasInstr(MI, O))
    printInstruction(MI, O);
  printAnnotation(O, Annot);
}

bool Cse523InstPrinter::printCse523AliasInstr(const MCInst *MI, raw_ostream &O)
{
  switch (MI->getOpcode()) {
  default: return false;
  //case SP::JMPLrr:
  //case SP::JMPLri: {
  //  if (MI->getNumOperands() != 3)
  //    return false;
  //  if (!MI->getOperand(0).isReg())
  //    return false;
  //  switch (MI->getOperand(0).getReg()) {
  //  default: return false;
  //  case SP::G0: // jmp $addr
  //    O << "\tjmp "; printMemOperand(MI, 1, O);
  //    return true;
  //  case SP::O7: // call $addr
  //    O << "\tcall "; printMemOperand(MI, 1, O);
  //    return true;
  //  }
  //}
  }
}

void Cse523InstPrinter::printOperand(const MCInst *MI, int opNum,
                                    raw_ostream &O)
{
  const MCOperand &MO = MI->getOperand (opNum);

  if (MO.isReg()) {
    printRegName(O, MO.getReg());
    return ;
  }

  if (MO.isImm()) {
    O << (int)MO.getImm();
    return;
  }

  assert(MO.isExpr() && "Unknown operand kind in printOperand");
  MO.getExpr()->print(O);
}

void Cse523InstPrinter::printMemOperand(const MCInst *MI, int opNum,
                                      raw_ostream &O, const char *Modifier)
{
  printOperand(MI, opNum, O);

  // If this is an ADD operand, emit it like normal operands.
  if (Modifier && !strcmp(Modifier, "arith")) {
    O << ", ";
    printOperand(MI, opNum+1, O);
    return;
  }
  const MCOperand &MO = MI->getOperand(opNum+1);

//  if (MO.isReg() && MO.getReg() == SP::G0)
//    return;   // don't print "+%g0"
  if (MO.isImm() && MO.getImm() == 0)
    return;   // don't print "+0"

  O << "+";

  printOperand(MI, opNum+1, O);
}

void Cse523InstPrinter::printCCOperand(const MCInst *MI, int opNum,
                                     raw_ostream &O)
{
  int CC = (int)MI->getOperand(opNum).getImm();
  switch (MI->getOpcode()) {
  default: break;
//  case SP::FBCOND:
//  case SP::MOVFCCrr:
//  case SP::MOVFCCri:
//  case SP::FMOVS_FCC:
//  case SP::FMOVD_FCC:
//  case SP::FMOVQ_FCC:  // Make sure CC is a fp conditional flag.
//    CC = (CC < 16) ? (CC + 16) : CC;
//    break;
  }
  //O << CSE523CondCodeToString((SPCC::CondCodes)CC);
}

bool Cse523InstPrinter::printGetPCX(const MCInst *MI, unsigned opNum,
                                  raw_ostream &O)
{
  assert(0 && "FIXME: Implement Cse523InstPrinter::printGetPCX.");
  return true;
}
