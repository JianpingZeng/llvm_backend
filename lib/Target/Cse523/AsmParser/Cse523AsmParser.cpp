//===-- Cse523AsmParser.cpp - Parse Cse523 assembly to MCInst instructions --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Cse523MCTargetDesc.h"
#include "MCTargetDesc/Cse523MCExpr.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCTargetAsmParser.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

// The generated AsmMatcher Cse523GenAsmMatcher uses "Cse523" as the target
// namespace. But CSE523 backend uses "SP" as its namespace.
namespace llvm {
  namespace Cse523 {
    //using namespace SP;
  }
}

namespace {
class Cse523Operand;
class Cse523AsmParser : public MCTargetAsmParser {

  MCSubtargetInfo &STI;
  MCAsmParser &Parser;

  /// @name Auto-generated Match Functions
  /// {

//#define GET_ASSEMBLER_HEADER
//#include "Cse523GenAsmMatcher.inc"

  /// }

  // public interface of the MCTargetAsmParser.
//  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
//                               SmallVectorImpl<MCParsedAsmOperand*> &Operands,
//                               MCStreamer &Out, unsigned &ErrorInfo,
//                               bool MatchingInlineAsm);
//  bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc);
//  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
//                        SMLoc NameLoc,
//                        SmallVectorImpl<MCParsedAsmOperand*> &Operands);
//  bool ParseDirective(AsmToken DirectiveID);
//
//  virtual unsigned validateTargetOperandClass(MCParsedAsmOperand *Op,
//                                              unsigned Kind);
//
//  // Custom parse functions for Cse523 specific operands.
//  OperandMatchResultTy
//  parseMEMOperand(SmallVectorImpl<MCParsedAsmOperand*> &Operands);
//
//  OperandMatchResultTy
//  parseOperand(SmallVectorImpl<MCParsedAsmOperand*> &Operands,
//               StringRef Name);
//
//  OperandMatchResultTy
//  parseCse523AsmOperand(Cse523Operand *&Operand);
//
//  // returns true if Tok is matched to a register and returns register in RegNo.
//  bool matchRegisterName(const AsmToken &Tok, unsigned &RegNo,
//                         unsigned &RegKind);
//
//  bool matchCse523AsmModifiers(const MCExpr *&EVal, SMLoc &EndLoc);

public:
  Cse523AsmParser(MCSubtargetInfo &sti, MCAsmParser &parser,
                const MCInstrInfo &MII)
      : MCTargetAsmParser(), STI(sti), Parser(parser) {
    // Initialize the set of available features.
    //setAvailableFeatures(ComputeAvailableFeatures(STI.getFeatureBits()));
  }

};

//  static unsigned IntRegs[32] = {
//    Cse523::G0, Cse523::G1, Cse523::G2, Cse523::G3,
//    Cse523::G4, Cse523::G5, Cse523::G6, Cse523::G7,
//    Cse523::O0, Cse523::O1, Cse523::O2, Cse523::O3,
//    Cse523::O4, Cse523::O5, Cse523::O6, Cse523::O7,
//    Cse523::L0, Cse523::L1, Cse523::L2, Cse523::L3,
//    Cse523::L4, Cse523::L5, Cse523::L6, Cse523::L7,
//    Cse523::I0, Cse523::I1, Cse523::I2, Cse523::I3,
//    Cse523::I4, Cse523::I5, Cse523::I6, Cse523::I7 };
//
//  static unsigned FloatRegs[32] = {
//    Cse523::F0,  Cse523::F1,  Cse523::F2,  Cse523::F3,
//    Cse523::F4,  Cse523::F5,  Cse523::F6,  Cse523::F7,
//    Cse523::F8,  Cse523::F9,  Cse523::F10, Cse523::F11,
//    Cse523::F12, Cse523::F13, Cse523::F14, Cse523::F15,
//    Cse523::F16, Cse523::F17, Cse523::F18, Cse523::F19,
//    Cse523::F20, Cse523::F21, Cse523::F22, Cse523::F23,
//    Cse523::F24, Cse523::F25, Cse523::F26, Cse523::F27,
//    Cse523::F28, Cse523::F29, Cse523::F30, Cse523::F31 };
//
//  static unsigned DoubleRegs[32] = {
//    Cse523::D0,  Cse523::D1,  Cse523::D2,  Cse523::D3,
//    Cse523::D4,  Cse523::D5,  Cse523::D6,  Cse523::D7,
//    Cse523::D8,  Cse523::D7,  Cse523::D8,  Cse523::D9,
//    Cse523::D12, Cse523::D13, Cse523::D14, Cse523::D15,
//    Cse523::D16, Cse523::D17, Cse523::D18, Cse523::D19,
//    Cse523::D20, Cse523::D21, Cse523::D22, Cse523::D23,
//    Cse523::D24, Cse523::D25, Cse523::D26, Cse523::D27,
//    Cse523::D28, Cse523::D29, Cse523::D30, Cse523::D31 };
//
//  static unsigned QuadFPRegs[32] = {
//    Cse523::Q0,  Cse523::Q1,  Cse523::Q2,  Cse523::Q3,
//    Cse523::Q4,  Cse523::Q5,  Cse523::Q6,  Cse523::Q7,
//    Cse523::Q8,  Cse523::Q9,  Cse523::Q10, Cse523::Q11,
//    Cse523::Q12, Cse523::Q13, Cse523::Q14, Cse523::Q15 };
//
//
///// Cse523Operand - Instances of this class represent a parsed Cse523 machine
///// instruction.
//class Cse523Operand : public MCParsedAsmOperand {
//public:
//  enum RegisterKind {
//    rk_None,
//    rk_IntReg,
//    rk_FloatReg,
//    rk_DoubleReg,
//    rk_QuadReg,
//    rk_CCReg,
//    rk_Y
//  };
//private:
//  enum KindTy {
//    k_Token,
//    k_Register,
//    k_Immediate,
//    k_MemoryReg,
//    k_MemoryImm
//  } Kind;
//
//  SMLoc StartLoc, EndLoc;
//
//  Cse523Operand(KindTy K) : MCParsedAsmOperand(), Kind(K) {}
//
//  struct Token {
//    const char *Data;
//    unsigned Length;
//  };
//
//  struct RegOp {
//    unsigned RegNum;
//    RegisterKind Kind;
//  };
//
//  struct ImmOp {
//    const MCExpr *Val;
//  };
//
//  struct MemOp {
//    unsigned Base;
//    unsigned OffsetReg;
//    const MCExpr *Off;
//  };
//
//  union {
//    struct Token Tok;
//    struct RegOp Reg;
//    struct ImmOp Imm;
//    struct MemOp Mem;
//  };
//public:
//  bool isToken() const { return Kind == k_Token; }
//  bool isReg() const { return Kind == k_Register; }
//  bool isImm() const { return Kind == k_Immediate; }
//  bool isMem() const { return isMEMrr() || isMEMri(); }
//  bool isMEMrr() const { return Kind == k_MemoryReg; }
//  bool isMEMri() const { return Kind == k_MemoryImm; }
//
//  bool isFloatReg() const {
//    return (Kind == k_Register && Reg.Kind == rk_FloatReg);
//  }
//
//  bool isFloatOrDoubleReg() const {
//    return (Kind == k_Register && (Reg.Kind == rk_FloatReg
//                                   || Reg.Kind == rk_DoubleReg));
//  }
//
//
//  StringRef getToken() const {
//    assert(Kind == k_Token && "Invalid access!");
//    return StringRef(Tok.Data, Tok.Length);
//  }
//
//  unsigned getReg() const {
//    assert((Kind == k_Register) && "Invalid access!");
//    return Reg.RegNum;
//  }
//
//  const MCExpr *getImm() const {
//    assert((Kind == k_Immediate) && "Invalid access!");
//    return Imm.Val;
//  }
//
//  unsigned getMemBase() const {
//    assert((Kind == k_MemoryReg || Kind == k_MemoryImm) && "Invalid access!");
//    return Mem.Base;
//  }
//
//  unsigned getMemOffsetReg() const {
//    assert((Kind == k_MemoryReg) && "Invalid access!");
//    return Mem.OffsetReg;
//  }
//
//  const MCExpr *getMemOff() const {
//    assert((Kind == k_MemoryImm) && "Invalid access!");
//    return Mem.Off;
//  }
//
//  /// getStartLoc - Get the location of the first token of this operand.
//  SMLoc getStartLoc() const {
//    return StartLoc;
//  }
//  /// getEndLoc - Get the location of the last token of this operand.
//  SMLoc getEndLoc() const {
//    return EndLoc;
//  }
//
//  virtual void print(raw_ostream &OS) const {
//    switch (Kind) {
//    case k_Token:     OS << "Token: " << getToken() << "\n"; break;
//    case k_Register:  OS << "Reg: #" << getReg() << "\n"; break;
//    case k_Immediate: OS << "Imm: " << getImm() << "\n"; break;
//    case k_MemoryReg: OS << "Mem: " << getMemBase() << "+"
//                         << getMemOffsetReg() << "\n"; break;
//    case k_MemoryImm: assert(getMemOff() != 0);
//      OS << "Mem: " << getMemBase()
//         << "+" << *getMemOff()
//         << "\n"; break;
//    }
//  }
//
//  void addRegOperands(MCInst &Inst, unsigned N) const {
//    assert(N == 1 && "Invalid number of operands!");
//    Inst.addOperand(MCOperand::CreateReg(getReg()));
//  }
//
//  void addImmOperands(MCInst &Inst, unsigned N) const {
//    assert(N == 1 && "Invalid number of operands!");
//    const MCExpr *Expr = getImm();
//    addExpr(Inst, Expr);
//  }
//
//  void addExpr(MCInst &Inst, const MCExpr *Expr) const{
//    // Add as immediate when possible.  Null MCExpr = 0.
//    if (Expr == 0)
//      Inst.addOperand(MCOperand::CreateImm(0));
//    else if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr))
//      Inst.addOperand(MCOperand::CreateImm(CE->getValue()));
//    else
//      Inst.addOperand(MCOperand::CreateExpr(Expr));
//  }
//
//  void addMEMrrOperands(MCInst &Inst, unsigned N) const {
//    assert(N == 2 && "Invalid number of operands!");
//
//    Inst.addOperand(MCOperand::CreateReg(getMemBase()));
//
//    assert(getMemOffsetReg() != 0 && "Invalid offset");
//    Inst.addOperand(MCOperand::CreateReg(getMemOffsetReg()));
//  }
//
//  void addMEMriOperands(MCInst &Inst, unsigned N) const {
//    assert(N == 2 && "Invalid number of operands!");
//
//    Inst.addOperand(MCOperand::CreateReg(getMemBase()));
//
//    const MCExpr *Expr = getMemOff();
//    addExpr(Inst, Expr);
//  }
//
//  static Cse523Operand *CreateToken(StringRef Str, SMLoc S) {
//    Cse523Operand *Op = new Cse523Operand(k_Token);
//    Op->Tok.Data = Str.data();
//    Op->Tok.Length = Str.size();
//    Op->StartLoc = S;
//    Op->EndLoc = S;
//    return Op;
//  }
//
//  static Cse523Operand *CreateReg(unsigned RegNum,
//                                 unsigned Kind,
//                                 SMLoc S, SMLoc E) {
//    Cse523Operand *Op = new Cse523Operand(k_Register);
//    Op->Reg.RegNum = RegNum;
//    Op->Reg.Kind   = (Cse523Operand::RegisterKind)Kind;
//    Op->StartLoc = S;
//    Op->EndLoc = E;
//    return Op;
//  }
//
//  static Cse523Operand *CreateImm(const MCExpr *Val, SMLoc S, SMLoc E) {
//    Cse523Operand *Op = new Cse523Operand(k_Immediate);
//    Op->Imm.Val = Val;
//    Op->StartLoc = S;
//    Op->EndLoc = E;
//    return Op;
//  }
//
//  static Cse523Operand *MorphToDoubleReg(Cse523Operand *Op) {
//    unsigned Reg = Op->getReg();
//    assert(Op->Reg.Kind == rk_FloatReg);
//    unsigned regIdx = Reg - Cse523::F0;
//    if (regIdx % 2 || regIdx > 31)
//      return 0;
//    Op->Reg.RegNum = DoubleRegs[regIdx / 2];
//    Op->Reg.Kind = rk_DoubleReg;
//    return Op;
//  }
//
//  static Cse523Operand *MorphToQuadReg(Cse523Operand *Op) {
//    unsigned Reg = Op->getReg();
//    unsigned regIdx = 0;
//    switch (Op->Reg.Kind) {
//    default: assert(0 && "Unexpected register kind!");
//    case rk_FloatReg:
//      regIdx = Reg - Cse523::F0;
//      if (regIdx % 4 || regIdx > 31)
//        return 0;
//      Reg = QuadFPRegs[regIdx / 4];
//      break;
//    case rk_DoubleReg:
//      regIdx =  Reg - Cse523::D0;
//      if (regIdx % 2 || regIdx > 31)
//        return 0;
//      Reg = QuadFPRegs[regIdx / 2];
//      break;
//    }
//    Op->Reg.RegNum  = Reg;
//    Op->Reg.Kind = rk_QuadReg;
//    return Op;
//  }
//
//  static Cse523Operand *MorphToMEMrr(unsigned Base, Cse523Operand *Op) {
//    unsigned offsetReg = Op->getReg();
//    Op->Kind = k_MemoryReg;
//    Op->Mem.Base = Base;
//    Op->Mem.OffsetReg = offsetReg;
//    Op->Mem.Off = 0;
//    return Op;
//  }
//
//  static Cse523Operand *CreateMEMri(unsigned Base,
//                                 const MCExpr *Off,
//                                 SMLoc S, SMLoc E) {
//    Cse523Operand *Op = new Cse523Operand(k_MemoryImm);
//    Op->Mem.Base = Base;
//    Op->Mem.OffsetReg = 0;
//    Op->Mem.Off = Off;
//    Op->StartLoc = S;
//    Op->EndLoc = E;
//    return Op;
//  }
//
//  static Cse523Operand *MorphToMEMri(unsigned Base, Cse523Operand *Op) {
//    const MCExpr *Imm  = Op->getImm();
//    Op->Kind = k_MemoryImm;
//    Op->Mem.Base = Base;
//    Op->Mem.OffsetReg = 0;
//    Op->Mem.Off = Imm;
//    return Op;
//  }
//};
//


} // end namespace



//bool Cse523AsmParser::
//MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
//                        SmallVectorImpl<MCParsedAsmOperand*> &Operands,
//                        MCStreamer &Out, unsigned &ErrorInfo,
//                        bool MatchingInlineAsm) {
//  MCInst Inst;
//  SmallVector<MCInst, 8> Instructions;
//  unsigned MatchResult = MatchInstructionImpl(Operands, Inst, ErrorInfo,
//                                              MatchingInlineAsm);
//  switch (MatchResult) {
//  default:
//    break;
//
//  case Match_Success: {
//    Inst.setLoc(IDLoc);
//    Out.EmitInstruction(Inst, STI);
//    return false;
//  }
//
//  case Match_MissingFeature:
//    return Error(IDLoc,
//                 "instruction requires a CPU feature not currently enabled");
//
//  case Match_InvalidOperand: {
//    SMLoc ErrorLoc = IDLoc;
//    if (ErrorInfo != ~0U) {
//      if (ErrorInfo >= Operands.size())
//        return Error(IDLoc, "too few operands for instruction");
//
//      ErrorLoc = ((Cse523Operand*) Operands[ErrorInfo])->getStartLoc();
//      if (ErrorLoc == SMLoc())
//        ErrorLoc = IDLoc;
//    }
//
//    return Error(ErrorLoc, "invalid operand for instruction");
//  }
//  case Match_MnemonicFail:
//    return Error(IDLoc, "invalid instruction");
//  }
//  return true;
//}
//
//bool Cse523AsmParser::
//ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc)
//{
//  const AsmToken &Tok = Parser.getTok();
//  StartLoc = Tok.getLoc();
//  EndLoc = Tok.getEndLoc();
//  RegNo = 0;
//  if (getLexer().getKind() != AsmToken::Percent)
//    return false;
//  Parser.Lex();
//  unsigned regKind = Cse523Operand::rk_None;
//  if (matchRegisterName(Tok, RegNo, regKind)) {
//    Parser.Lex();
//    return false;
//  }
//
//  return Error(StartLoc, "invalid register name");
//}
//
//bool Cse523AsmParser::
//ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
//                 SMLoc NameLoc,
//                 SmallVectorImpl<MCParsedAsmOperand*> &Operands)
//{
//  // Check if we have valid mnemonic.
//  if (!mnemonicIsValid(Name, 0)) {
//    Parser.eatToEndOfStatement();
//    return Error(NameLoc, "Unknown instruction");
//  }
//  // First operand in MCInst is instruction mnemonic.
//  Operands.push_back(Cse523Operand::CreateToken(Name, NameLoc));
//
//  if (getLexer().isNot(AsmToken::EndOfStatement)) {
//    // Read the first operand.
//    if (parseOperand(Operands, Name) != MatchOperand_Success) {
//      SMLoc Loc = getLexer().getLoc();
//      Parser.eatToEndOfStatement();
//      return Error(Loc, "unexpected token");
//    }
//
//    while (getLexer().is(AsmToken::Comma)) {
//      Parser.Lex(); // Eat the comma.
//      // Parse and remember the operand.
//      if (parseOperand(Operands, Name) != MatchOperand_Success) {
//        SMLoc Loc = getLexer().getLoc();
//        Parser.eatToEndOfStatement();
//        return Error(Loc, "unexpected token");
//      }
//    }
//  }
//  if (getLexer().isNot(AsmToken::EndOfStatement)) {
//    SMLoc Loc = getLexer().getLoc();
//    Parser.eatToEndOfStatement();
//    return Error(Loc, "unexpected token");
//  }
//  Parser.Lex(); // Consume the EndOfStatement.
//  return false;
//}
//
//bool Cse523AsmParser::
//ParseDirective(AsmToken DirectiveID)
//{
//  // Ignore all directives for now.
//  Parser.eatToEndOfStatement();
//  return false;
//}
//
//Cse523AsmParser::OperandMatchResultTy Cse523AsmParser::
//parseMEMOperand(SmallVectorImpl<MCParsedAsmOperand*> &Operands)
//{
//
//  SMLoc S, E;
//  unsigned BaseReg = 0;
//
//  if (ParseRegister(BaseReg, S, E)) {
//    return MatchOperand_NoMatch;
//  }
//
//  switch (getLexer().getKind()) {
//  default: return MatchOperand_NoMatch;
//
//  case AsmToken::Comma:
//  case AsmToken::RBrac:
//  case AsmToken::EndOfStatement:
//    Operands.push_back(Cse523Operand::CreateMEMri(BaseReg, 0, S, E));
//    return MatchOperand_Success;
//
//  case AsmToken:: Plus:
//    Parser.Lex(); // Eat the '+'
//    break;
//  case AsmToken::Minus:
//    break;
//  }
//
//  Cse523Operand *Offset = 0;
//  OperandMatchResultTy ResTy = parseCse523AsmOperand(Offset);
//  if (ResTy != MatchOperand_Success || !Offset)
//    return MatchOperand_NoMatch;
//
//  Offset = (Offset->isImm()
//            ? Cse523Operand::MorphToMEMri(BaseReg, Offset)
//            : Cse523Operand::MorphToMEMrr(BaseReg, Offset));
//
//  Operands.push_back(Offset);
//  return MatchOperand_Success;
//}
//
//Cse523AsmParser::OperandMatchResultTy Cse523AsmParser::
//parseOperand(SmallVectorImpl<MCParsedAsmOperand*> &Operands,
//             StringRef Mnemonic)
//{
//
//  OperandMatchResultTy ResTy = MatchOperandParserImpl(Operands, Mnemonic);
//
//  // If there wasn't a custom match, try the generic matcher below. Otherwise,
//  // there was a match, but an error occurred, in which case, just return that
//  // the operand parsing failed.
//  if (ResTy == MatchOperand_Success || ResTy == MatchOperand_ParseFail)
//    return ResTy;
//
//  if (getLexer().is(AsmToken::LBrac)) {
//    // Memory operand
//    Operands.push_back(Cse523Operand::CreateToken("[",
//                                                 Parser.getTok().getLoc()));
//    Parser.Lex(); // Eat the [
//
//    if (Mnemonic == "cas" || Mnemonic == "casx") {
//      SMLoc S = Parser.getTok().getLoc();
//      if (getLexer().getKind() != AsmToken::Percent)
//        return MatchOperand_NoMatch;
//      Parser.Lex(); // eat %
//
//      unsigned RegNo, RegKind;
//      if (!matchRegisterName(Parser.getTok(), RegNo, RegKind))
//        return MatchOperand_NoMatch;
//
//      Parser.Lex(); // Eat the identifier token.
//      SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer()-1);
//      Operands.push_back(Cse523Operand::CreateReg(RegNo, RegKind, S, E));
//      ResTy = MatchOperand_Success;
//    } else {
//      ResTy = parseMEMOperand(Operands);
//    }
//
//    if (ResTy != MatchOperand_Success)
//      return ResTy;
//
//    if (!getLexer().is(AsmToken::RBrac))
//      return MatchOperand_ParseFail;
//
//    Operands.push_back(Cse523Operand::CreateToken("]",
//                                                 Parser.getTok().getLoc()));
//    Parser.Lex(); // Eat the ]
//    return MatchOperand_Success;
//  }
//
//  Cse523Operand *Op = 0;
//  ResTy = parseCse523AsmOperand(Op);
//  if (ResTy != MatchOperand_Success || !Op)
//    return MatchOperand_ParseFail;
//
//  // Push the parsed operand into the list of operands
//  Operands.push_back(Op);
//
//  return MatchOperand_Success;
//}
//
//Cse523AsmParser::OperandMatchResultTy
//Cse523AsmParser::parseCse523AsmOperand(Cse523Operand *&Op)
//{
//
//  SMLoc S = Parser.getTok().getLoc();
//  SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
//  const MCExpr *EVal;
//
//  Op = 0;
//  switch (getLexer().getKind()) {
//  default:  break;
//
//  case AsmToken::Percent:
//    Parser.Lex(); // Eat the '%'.
//    unsigned RegNo;
//    unsigned RegKind;
//    if (matchRegisterName(Parser.getTok(), RegNo, RegKind)) {
//      StringRef name = Parser.getTok().getString();
//      Parser.Lex(); // Eat the identifier token.
//      E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
//      switch (RegNo) {
//      default:
//        Op = Cse523Operand::CreateReg(RegNo, RegKind, S, E);
//        break;
//      case Cse523::Y:
//        Op = Cse523Operand::CreateToken("%y", S);
//        break;
//
//      case Cse523::ICC:
//        if (name == "xcc")
//          Op = Cse523Operand::CreateToken("%xcc", S);
//        else
//          Op = Cse523Operand::CreateToken("%icc", S);
//        break;
//
//      case Cse523::FCC:
//        assert(name == "fcc0" && "Cannot handle %fcc other than %fcc0 yet");
//        Op = Cse523Operand::CreateToken("%fcc0", S);
//        break;
//      }
//      break;
//    }
//    if (matchCse523AsmModifiers(EVal, E)) {
//      E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
//      Op = Cse523Operand::CreateImm(EVal, S, E);
//    }
//    break;
//
//  case AsmToken::Minus:
//  case AsmToken::Integer:
//    if (!getParser().parseExpression(EVal, E))
//      Op = Cse523Operand::CreateImm(EVal, S, E);
//    break;
//
//  case AsmToken::Identifier: {
//    StringRef Identifier;
//    if (!getParser().parseIdentifier(Identifier)) {
//      E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
//      MCSymbol *Sym = getContext().GetOrCreateSymbol(Identifier);
//
//      const MCExpr *Res = MCSymbolRefExpr::Create(Sym, MCSymbolRefExpr::VK_None,
//                                                  getContext());
//      Op = Cse523Operand::CreateImm(Res, S, E);
//    }
//    break;
//  }
//  }
//  return (Op) ? MatchOperand_Success : MatchOperand_ParseFail;
//}
//
//bool Cse523AsmParser::matchRegisterName(const AsmToken &Tok,
//                                       unsigned &RegNo,
//                                       unsigned &RegKind)
//{
//  int64_t intVal = 0;
//  RegNo = 0;
//  RegKind = Cse523Operand::rk_None;
//  if (Tok.is(AsmToken::Identifier)) {
//    StringRef name = Tok.getString();
//
//    // %fp
//    if (name.equals("fp")) {
//      RegNo = Cse523::I6;
//      RegKind = Cse523Operand::rk_IntReg;
//      return true;
//    }
//    // %sp
//    if (name.equals("sp")) {
//      RegNo = Cse523::O6;
//      RegKind = Cse523Operand::rk_IntReg;
//      return true;
//    }
//
//    if (name.equals("y")) {
//      RegNo = Cse523::Y;
//      RegKind = Cse523Operand::rk_Y;
//      return true;
//    }
//
//    if (name.equals("icc")) {
//      RegNo = Cse523::ICC;
//      RegKind = Cse523Operand::rk_CCReg;
//      return true;
//    }
//
//    if (name.equals("xcc")) {
//      // FIXME:: check 64bit.
//      RegNo = Cse523::ICC;
//      RegKind = Cse523Operand::rk_CCReg;
//      return true;
//    }
//
//    // %fcc0 - %fcc3
//    if (name.substr(0, 3).equals_lower("fcc")
//        && !name.substr(3).getAsInteger(10, intVal)
//        && intVal < 4) {
//      // FIXME: check 64bit and  handle %fcc1 - %fcc3
//      RegNo = Cse523::FCC;
//      RegKind = Cse523Operand::rk_CCReg;
//      return true;
//    }
//
//    // %g0 - %g7
//    if (name.substr(0, 1).equals_lower("g")
//        && !name.substr(1).getAsInteger(10, intVal)
//        && intVal < 8) {
//      RegNo = IntRegs[intVal];
//      RegKind = Cse523Operand::rk_IntReg;
//      return true;
//    }
//    // %o0 - %o7
//    if (name.substr(0, 1).equals_lower("o")
//        && !name.substr(1).getAsInteger(10, intVal)
//        && intVal < 8) {
//      RegNo = IntRegs[8 + intVal];
//      RegKind = Cse523Operand::rk_IntReg;
//      return true;
//    }
//    if (name.substr(0, 1).equals_lower("l")
//        && !name.substr(1).getAsInteger(10, intVal)
//        && intVal < 8) {
//      RegNo = IntRegs[16 + intVal];
//      RegKind = Cse523Operand::rk_IntReg;
//      return true;
//    }
//    if (name.substr(0, 1).equals_lower("i")
//        && !name.substr(1).getAsInteger(10, intVal)
//        && intVal < 8) {
//      RegNo = IntRegs[24 + intVal];
//      RegKind = Cse523Operand::rk_IntReg;
//      return true;
//    }
//    // %f0 - %f31
//    if (name.substr(0, 1).equals_lower("f")
//        && !name.substr(1, 2).getAsInteger(10, intVal) && intVal < 32) {
//      RegNo = FloatRegs[intVal];
//      RegKind = Cse523Operand::rk_FloatReg;
//      return true;
//    }
//    // %f32 - %f62
//    if (name.substr(0, 1).equals_lower("f")
//        && !name.substr(1, 2).getAsInteger(10, intVal)
//        && intVal >= 32 && intVal <= 62 && (intVal % 2 == 0)) {
//      // FIXME: Check V9
//      RegNo = DoubleRegs[intVal/2];
//      RegKind = Cse523Operand::rk_DoubleReg;
//      return true;
//    }
//
//    // %r0 - %r31
//    if (name.substr(0, 1).equals_lower("r")
//        && !name.substr(1, 2).getAsInteger(10, intVal) && intVal < 31) {
//      RegNo = IntRegs[intVal];
//      RegKind = Cse523Operand::rk_IntReg;
//      return true;
//    }
//  }
//  return false;
//}
//
//
//bool Cse523AsmParser::matchCse523AsmModifiers(const MCExpr *&EVal,
//                                            SMLoc &EndLoc)
//{
//  AsmToken Tok = Parser.getTok();
//  if (!Tok.is(AsmToken::Identifier))
//    return false;
//
//  StringRef name = Tok.getString();
//
//  Cse523MCExpr::VariantKind VK = Cse523MCExpr::parseVariantKind(name);
//
//  if (VK == Cse523MCExpr::VK_Cse523_None)
//    return false;
//
//  Parser.Lex(); // Eat the identifier.
//  if (Parser.getTok().getKind() != AsmToken::LParen)
//    return false;
//
//  Parser.Lex(); // Eat the LParen token.
//  const MCExpr *subExpr;
//  if (Parser.parseParenExpression(subExpr, EndLoc))
//    return false;
//  EVal = Cse523MCExpr::Create(VK, subExpr, getContext());
//  return true;
//}
//
//
//extern "C" void LLVMInitializeCse523AsmParser() {
//  RegisterMCAsmParser<Cse523AsmParser> A(TheCse523Target);
//}
//
//#define GET_REGISTER_MATCHER
//#define GET_MATCHER_IMPLEMENTATION
////#include "Cse523GenAsmMatcher.inc"
//
//
//
//unsigned Cse523AsmParser::
//validateTargetOperandClass(MCParsedAsmOperand *GOp,
//                           unsigned Kind)
//{
//  Cse523Operand *Op = (Cse523Operand*)GOp;
//  if (Op->isFloatOrDoubleReg()) {
//    switch (Kind) {
//    default: break;
//    case MCK_DFPRegs:
//      if (!Op->isFloatReg() || Cse523Operand::MorphToDoubleReg(Op))
//        return MCTargetAsmParser::Match_Success;
//      break;
//    case MCK_QFPRegs:
//      if (Cse523Operand::MorphToQuadReg(Op))
//        return MCTargetAsmParser::Match_Success;
//      break;
//    }
//  }
//  return Match_InvalidOperand;
//}
