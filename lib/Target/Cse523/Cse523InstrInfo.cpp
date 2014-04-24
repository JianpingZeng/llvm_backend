//===-- Cse523InstrInfo.cpp - Cse523 Instruction Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Cse523 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "Cse523InstrInfo.h"
#include "Cse523.h"
#include "Cse523InstrBuilder.h"
#include "Cse523MachineFunctionInfo.h"
#include "Cse523Subtarget.h"
#include "Cse523TargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/LiveVariables.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/StackMaps.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetOptions.h"
#include <limits>

#define GET_INSTRINFO_CTOR_DTOR
#include "Cse523GenInstrInfo.inc"

using namespace llvm;

static cl::opt<bool>
NoFusing("disable-spill-fusing",
        cl::desc("Disable fusing of spill code into instructions"));
static cl::opt<bool>
PrintFailedFusing("print-failed-fuse-candidates",
        cl::desc("Print instructions that the allocator wants to"
            " fuse, but the Cse523 backend currently can't"),
        cl::Hidden);
static cl::opt<bool>
ReMatPICStubLoad("remat-pic-stub-load",
        cl::desc("Re-materialize load from stub in PIC mode"),
        cl::init(false), cl::Hidden);

enum {
    // Select which memory operand is being unfolded.
    // (stored in bits 0 - 3)
    TB_INDEX_0    = 0,
    TB_INDEX_1    = 1,
    TB_INDEX_2    = 2,
    TB_INDEX_3    = 3,
    TB_INDEX_MASK = 0xf,

    // Do not insert the reverse map (MemOp -> RegOp) into the table.
    // This may be needed because there is a many -> one mapping.
    TB_NO_REVERSE   = 1 << 4,

    // Do not insert the forward map (RegOp -> MemOp) into the table.
    // This is needed for Native Client, which prohibits branch
    // instructions from using a memory operand.
    TB_NO_FORWARD   = 1 << 5,

    TB_FOLDED_LOAD  = 1 << 6,
    TB_FOLDED_STORE = 1 << 7,

    // Minimum alignment required for load/store.
    // Used for RegOp->MemOp conversion.
    // (stored in bits 8 - 15)
    TB_ALIGN_SHIFT = 8,
    TB_ALIGN_NONE  =    0 << TB_ALIGN_SHIFT,
    TB_ALIGN_16    =   16 << TB_ALIGN_SHIFT,
    TB_ALIGN_32    =   32 << TB_ALIGN_SHIFT,
    TB_ALIGN_64    =   64 << TB_ALIGN_SHIFT,
    TB_ALIGN_MASK  = 0xff << TB_ALIGN_SHIFT
};

struct Cse523OpTblEntry {
    uint16_t RegOp;
    uint16_t MemOp;
    uint16_t Flags;
};

// Pin the vtable to this file.
void Cse523InstrInfo::anchor() {}

Cse523InstrInfo::Cse523InstrInfo(Cse523TargetMachine &tm)
    : Cse523GenInstrInfo (
        Cse523::ADJCALLSTACKDOWN64, Cse523::ADJCALLSTACKUP64
    ),
    TM(tm), RI(tm)
{

    static const Cse523OpTblEntry OpTbl2Addr[] = {
        //{ Cse523::ADC64ri32,   Cse523::ADC64mi32,  0 },
        //{ Cse523::ADC64rr,     Cse523::ADC64mr,    0 },
        { Cse523::ADD64ri32,   Cse523::ADD64mi32,  0 },
        //{ Cse523::ADD64ri32_DB,Cse523::ADD64mi32,  TB_NO_REVERSE },
        { Cse523::ADD64rr,     Cse523::ADD64mr,    0 },
        //{ Cse523::ADD64rr_DB,  Cse523::ADD64mr,    TB_NO_REVERSE },
        { Cse523::AND64ri32,   Cse523::AND64mi32,  0 },
        { Cse523::AND64rr,     Cse523::AND64mr,    0 },
        //{ Cse523::DEC64_32r,   Cse523::DEC64_32m,  0 },
        { Cse523::DEC64r,      Cse523::DEC64m,     0 },
        //{ Cse523::INC64_32r,   Cse523::INC64_32m,  0 },
        { Cse523::INC64r,      Cse523::INC64m,     0 },
        { Cse523::NEG64r,      Cse523::NEG64m,     0 },
        { Cse523::NOT64r,      Cse523::NOT64m,     0 },
        { Cse523::OR64ri32,    Cse523::OR64mi32,   0 },
        { Cse523::OR64rr,      Cse523::OR64mr,     0 },
        //{ Cse523::ROL64r1,     Cse523::ROL64m1,    0 },
        //{ Cse523::ROL64rCL,    Cse523::ROL64mCL,   0 },
        //{ Cse523::ROL64ri,     Cse523::ROL64mi,    0 },
        //{ Cse523::ROR64r1,     Cse523::ROR64m1,    0 },
        //{ Cse523::ROR64rCL,    Cse523::ROR64mCL,   0 },
        //{ Cse523::ROR64ri,     Cse523::ROR64mi,    0 },
        //{ Cse523::SAR64r1,     Cse523::SAR64m1,    0 },
        //{ Cse523::SAR64rCL,    Cse523::SAR64mCL,   0 },
        //{ Cse523::SAR64ri,     Cse523::SAR64mi,    0 },
        //{ Cse523::SBB64ri32,   Cse523::SBB64mi32,  0 },
        //{ Cse523::SBB64rr,     Cse523::SBB64mr,    0 },
        //{ Cse523::SHL64rCL,    Cse523::SHL64mCL,   0 },
        //{ Cse523::SHL64ri,     Cse523::SHL64mi,    0 },
        //{ Cse523::SHLD64rrCL,  Cse523::SHLD64mrCL, 0 },
        //{ Cse523::SHR64r1,     Cse523::SHR64m1,    0 },
        //{ Cse523::SHR64rCL,    Cse523::SHR64mCL,   0 },
        //{ Cse523::SHR64ri,     Cse523::SHR64mi,    0 },
        //{ Cse523::SHRD64rrCL,  Cse523::SHRD64mrCL, 0 },
        { Cse523::SUB64ri32,   Cse523::SUB64mi32,  0 },
        { Cse523::SUB64rr,     Cse523::SUB64mr,    0 },
        { Cse523::XOR64ri32,   Cse523::XOR64mi32,  0 },
        { Cse523::XOR64rr,     Cse523::XOR64mr,    0 }
    };

    for (unsigned i = 0, e = array_lengthof(OpTbl2Addr); i != e; ++i) {
        unsigned RegOp = OpTbl2Addr[i].RegOp;
        unsigned MemOp = OpTbl2Addr[i].MemOp;
        unsigned Flags = OpTbl2Addr[i].Flags;
        AddTableEntry(RegOp2MemOpTable2Addr, MemOp2RegOpTable,
                RegOp, MemOp,
                // Index 0, folded load and store, no alignment requirement.
                Flags | TB_INDEX_0 | TB_FOLDED_LOAD | TB_FOLDED_STORE);
    }

    static const Cse523OpTblEntry OpTbl0[] = {
        //{ Cse523::BT16ri8,     Cse523::BT16mi8,       TB_FOLDED_LOAD },
        //{ Cse523::BT32ri8,     Cse523::BT32mi8,       TB_FOLDED_LOAD },
        //{ Cse523::BT64ri8,     Cse523::BT64mi8,       TB_FOLDED_LOAD },
        //{ Cse523::CALL32r,     Cse523::CALL32m,       TB_FOLDED_LOAD },
        { Cse523::CALL64r,     Cse523::CALL64m,       TB_FOLDED_LOAD },
        //{ Cse523::CMP16ri,     Cse523::CMP16mi,       TB_FOLDED_LOAD },
        //{ Cse523::CMP16ri8,    Cse523::CMP16mi8,      TB_FOLDED_LOAD },
        //{ Cse523::CMP16rr,     Cse523::CMP16mr,       TB_FOLDED_LOAD },
        //{ Cse523::CMP32ri,     Cse523::CMP32mi,       TB_FOLDED_LOAD },
        //{ Cse523::CMP32ri8,    Cse523::CMP32mi8,      TB_FOLDED_LOAD },
        //{ Cse523::CMP32rr,     Cse523::CMP32mr,       TB_FOLDED_LOAD },
        { Cse523::CMP64ri32,   Cse523::CMP64mi32,     TB_FOLDED_LOAD },
        //{ Cse523::CMP64ri8,    Cse523::CMP64mi8,      TB_FOLDED_LOAD },
        { Cse523::CMP64rr,     Cse523::CMP64mr,       TB_FOLDED_LOAD },
        //{ Cse523::CMP8ri,      Cse523::CMP8mi,        TB_FOLDED_LOAD },
        //{ Cse523::CMP8rr,      Cse523::CMP8mr,        TB_FOLDED_LOAD },
        //{ Cse523::DIV16r,      Cse523::DIV16m,        TB_FOLDED_LOAD },
        //{ Cse523::DIV32r,      Cse523::DIV32m,        TB_FOLDED_LOAD },
        { Cse523::DIV64r,      Cse523::DIV64m,        TB_FOLDED_LOAD },
        //{ Cse523::DIV8r,       Cse523::DIV8m,         TB_FOLDED_LOAD },
        //{ Cse523::EXTRACTPSrr, Cse523::EXTRACTPSmr,   TB_FOLDED_STORE },
        //{ Cse523::IDIV16r,     Cse523::IDIV16m,       TB_FOLDED_LOAD },
        //{ Cse523::IDIV32r,     Cse523::IDIV32m,       TB_FOLDED_LOAD },
        //{ Cse523::IDIV64r,     Cse523::IDIV64m,       TB_FOLDED_LOAD },
        //{ Cse523::IDIV8r,      Cse523::IDIV8m,        TB_FOLDED_LOAD },
        //{ Cse523::IMUL16r,     Cse523::IMUL16m,       TB_FOLDED_LOAD },
        //{ Cse523::IMUL32r,     Cse523::IMUL32m,       TB_FOLDED_LOAD },
        { Cse523::IMUL64r,     Cse523::IMUL64m,       TB_FOLDED_LOAD },
        //{ Cse523::IMUL8r,      Cse523::IMUL8m,        TB_FOLDED_LOAD },
        //{ Cse523::JMP32r,      Cse523::JMP32m,        TB_FOLDED_LOAD },
        { Cse523::JMP64r,      Cse523::JMP64m,        TB_FOLDED_LOAD },
        //{ Cse523::MOV16ri,     Cse523::MOV16mi,       TB_FOLDED_STORE },
        //{ Cse523::MOV16rr,     Cse523::MOV16mr,       TB_FOLDED_STORE },
        //{ Cse523::MOV32ri,     Cse523::MOV32mi,       TB_FOLDED_STORE },
        //{ Cse523::MOV32rr,     Cse523::MOV32mr,       TB_FOLDED_STORE },
        { Cse523::MOV64ri32,   Cse523::MOV64mi32,     TB_FOLDED_STORE },
        { Cse523::MOV64rr,     Cse523::MOV64mr,       TB_FOLDED_STORE },
        //{ Cse523::MOV8ri,      Cse523::MOV8mi,        TB_FOLDED_STORE },
        //{ Cse523::MOV8rr,      Cse523::MOV8mr,        TB_FOLDED_STORE },
        //{ Cse523::MOV8rr_NOREX, Cse523::MOV8mr_NOREX, TB_FOLDED_STORE },
        //{ Cse523::MOVAPDrr,    Cse523::MOVAPDmr,      TB_FOLDED_STORE | TB_ALIGN_16 },
        //{ Cse523::MOVAPSrr,    Cse523::MOVAPSmr,      TB_FOLDED_STORE | TB_ALIGN_16 },
        //{ Cse523::MOVDQArr,    Cse523::MOVDQAmr,      TB_FOLDED_STORE | TB_ALIGN_16 },
        //{ Cse523::MOVPDI2DIrr, Cse523::MOVPDI2DImr,   TB_FOLDED_STORE },
        //{ Cse523::MOVPQIto64rr,Cse523::MOVPQI2QImr,   TB_FOLDED_STORE },
        //{ Cse523::MOVSDto64rr, Cse523::MOVSDto64mr,   TB_FOLDED_STORE },
        //{ Cse523::MOVSS2DIrr,  Cse523::MOVSS2DImr,    TB_FOLDED_STORE },
        //{ Cse523::MOVUPDrr,    Cse523::MOVUPDmr,      TB_FOLDED_STORE },
        //{ Cse523::MOVUPSrr,    Cse523::MOVUPSmr,      TB_FOLDED_STORE },
        //{ Cse523::MUL16r,      Cse523::MUL16m,        TB_FOLDED_LOAD },
        //{ Cse523::MUL32r,      Cse523::MUL32m,        TB_FOLDED_LOAD },
        //{ Cse523::MUL64r,      Cse523::MUL64m,        TB_FOLDED_LOAD },
        //{ Cse523::MUL8r,       Cse523::MUL8m,         TB_FOLDED_LOAD },
        //{ Cse523::SETAEr,      Cse523::SETAEm,        TB_FOLDED_STORE },
        //{ Cse523::SETAr,       Cse523::SETAm,         TB_FOLDED_STORE },
        //{ Cse523::SETBEr,      Cse523::SETBEm,        TB_FOLDED_STORE },
        //{ Cse523::SETBr,       Cse523::SETBm,         TB_FOLDED_STORE },
        //{ Cse523::SETEr,       Cse523::SETEm,         TB_FOLDED_STORE },
        //{ Cse523::SETGEr,      Cse523::SETGEm,        TB_FOLDED_STORE },
        //{ Cse523::SETGr,       Cse523::SETGm,         TB_FOLDED_STORE },
        //{ Cse523::SETLEr,      Cse523::SETLEm,        TB_FOLDED_STORE },
        //{ Cse523::SETLr,       Cse523::SETLm,         TB_FOLDED_STORE },
        //{ Cse523::SETNEr,      Cse523::SETNEm,        TB_FOLDED_STORE },
        //{ Cse523::SETNOr,      Cse523::SETNOm,        TB_FOLDED_STORE },
        //{ Cse523::SETNPr,      Cse523::SETNPm,        TB_FOLDED_STORE },
        //{ Cse523::SETNSr,      Cse523::SETNSm,        TB_FOLDED_STORE },
        //{ Cse523::SETOr,       Cse523::SETOm,         TB_FOLDED_STORE },
        //{ Cse523::SETPr,       Cse523::SETPm,         TB_FOLDED_STORE },
        //{ Cse523::SETSr,       Cse523::SETSm,         TB_FOLDED_STORE },
        //{ Cse523::TAILJMPr,    Cse523::TAILJMPm,      TB_FOLDED_LOAD },
        { Cse523::TAILJMPr64,  Cse523::TAILJMPm64,    TB_FOLDED_LOAD },
        //{ Cse523::TEST16ri,    Cse523::TEST16mi,      TB_FOLDED_LOAD },
        //{ Cse523::TEST32ri,    Cse523::TEST32mi,      TB_FOLDED_LOAD },
        { Cse523::TEST64ri32,  Cse523::TEST64mi32,    TB_FOLDED_LOAD }
        //{ Cse523::TEST8ri,     Cse523::TEST8mi,       TB_FOLDED_LOAD }
    };

    for (unsigned i = 0, e = array_lengthof(OpTbl0); i != e; ++i) {
        unsigned RegOp      = OpTbl0[i].RegOp;
        unsigned MemOp      = OpTbl0[i].MemOp;
        unsigned Flags      = OpTbl0[i].Flags;
        AddTableEntry(RegOp2MemOpTable0, MemOp2RegOpTable,
                RegOp, MemOp, TB_INDEX_0 | Flags);
    }

    static const Cse523OpTblEntry OpTbl1[] = {
        //{ Cse523::CMP16rr,         Cse523::CMP16rm,             0 },
        //{ Cse523::CMP32rr,         Cse523::CMP32rm,             0 },
        { Cse523::CMP64rr,         Cse523::CMP64rm,             0 },
        //{ Cse523::CMP8rr,          Cse523::CMP8rm,              0 },
        //{ Cse523::CVTSD2SSrr,      Cse523::CVTSD2SSrm,          0 },
        //{ Cse523::CVTSI2SD64rr,    Cse523::CVTSI2SD64rm,        0 },
        //{ Cse523::CVTSI2SDrr,      Cse523::CVTSI2SDrm,          0 },
        //{ Cse523::CVTSI2SS64rr,    Cse523::CVTSI2SS64rm,        0 },
        //{ Cse523::CVTSI2SSrr,      Cse523::CVTSI2SSrm,          0 },
        //{ Cse523::CVTSS2SDrr,      Cse523::CVTSS2SDrm,          0 },
        //{ Cse523::CVTTSD2SI64rr,   Cse523::CVTTSD2SI64rm,       0 },
        //{ Cse523::CVTTSD2SIrr,     Cse523::CVTTSD2SIrm,         0 },
        //{ Cse523::CVTTSS2SI64rr,   Cse523::CVTTSS2SI64rm,       0 },
        //{ Cse523::CVTTSS2SIrr,     Cse523::CVTTSS2SIrm,         0 },
        //{ Cse523::IMUL16rri,       Cse523::IMUL16rmi,           0 },
        //{ Cse523::IMUL16rri8,      Cse523::IMUL16rmi8,          0 },
        //{ Cse523::IMUL32rri,       Cse523::IMUL32rmi,           0 },
        //{ Cse523::IMUL32rri8,      Cse523::IMUL32rmi8,          0 },
        { Cse523::IMUL64rri32,     Cse523::IMUL64rmi32,         0 },
        //{ Cse523::IMUL64rri8,      Cse523::IMUL64rmi8,          0 },
        //{ Cse523::Int_COMISDrr,    Cse523::Int_COMISDrm,        0 },
        //{ Cse523::Int_COMISSrr,    Cse523::Int_COMISSrm,        0 },
        //{ Cse523::CVTSD2SI64rr,    Cse523::CVTSD2SI64rm,        0 },
        //{ Cse523::CVTSD2SIrr,      Cse523::CVTSD2SIrm,          0 },
        //{ Cse523::CVTSS2SI64rr,    Cse523::CVTSS2SI64rm,        0 },
        //{ Cse523::CVTSS2SIrr,      Cse523::CVTSS2SIrm,          0 },
        //{ Cse523::CVTTPD2DQrr,     Cse523::CVTTPD2DQrm,         TB_ALIGN_16 },
        //{ Cse523::CVTTPS2DQrr,     Cse523::CVTTPS2DQrm,         TB_ALIGN_16 },
        //{ Cse523::Int_CVTTSD2SI64rr,Cse523::Int_CVTTSD2SI64rm,  0 },
        //{ Cse523::Int_CVTTSD2SIrr, Cse523::Int_CVTTSD2SIrm,     0 },
        //{ Cse523::Int_CVTTSS2SI64rr,Cse523::Int_CVTTSS2SI64rm,  0 },
        //{ Cse523::Int_CVTTSS2SIrr, Cse523::Int_CVTTSS2SIrm,     0 },
        //{ Cse523::Int_UCOMISDrr,   Cse523::Int_UCOMISDrm,       0 },
        //{ Cse523::Int_UCOMISSrr,   Cse523::Int_UCOMISSrm,       0 },
        //{ Cse523::MOV16rr,         Cse523::MOV16rm,             0 },
        //{ Cse523::MOV32rr,         Cse523::MOV32rm,             0 },
        { Cse523::MOV64rr,         Cse523::MOV64rm,             0 },
        //{ Cse523::MOV64toPQIrr,    Cse523::MOVQI2PQIrm,         0 },
        //{ Cse523::MOV64toSDrr,     Cse523::MOV64toSDrm,         0 },
        //{ Cse523::MOV8rr,          Cse523::MOV8rm,              0 },
        //{ Cse523::MOVAPDrr,        Cse523::MOVAPDrm,            TB_ALIGN_16 },
        //{ Cse523::MOVAPSrr,        Cse523::MOVAPSrm,            TB_ALIGN_16 },
        //{ Cse523::MOVDDUPrr,       Cse523::MOVDDUPrm,           0 },
        //{ Cse523::MOVDI2PDIrr,     Cse523::MOVDI2PDIrm,         0 },
        //{ Cse523::MOVDI2SSrr,      Cse523::MOVDI2SSrm,          0 },
        //{ Cse523::MOVDQArr,        Cse523::MOVDQArm,            TB_ALIGN_16 },
        //{ Cse523::MOVSHDUPrr,      Cse523::MOVSHDUPrm,          TB_ALIGN_16 },
        //{ Cse523::MOVSLDUPrr,      Cse523::MOVSLDUPrm,          TB_ALIGN_16 },
        //{ Cse523::MOVSX16rr8,      Cse523::MOVSX16rm8,          0 },
        //{ Cse523::MOVSX32rr16,     Cse523::MOVSX32rm16,         0 },
        //{ Cse523::MOVSX32rr8,      Cse523::MOVSX32rm8,          0 },
        //{ Cse523::MOVSX64rr16,     Cse523::MOVSX64rm16,         0 },
        //{ Cse523::MOVSX64rr32,     Cse523::MOVSX64rm32,         0 },
        //{ Cse523::MOVSX64rr8,      Cse523::MOVSX64rm8,          0 },
        //{ Cse523::MOVUPDrr,        Cse523::MOVUPDrm,            TB_ALIGN_16 },
        //{ Cse523::MOVUPSrr,        Cse523::MOVUPSrm,            0 },
        //{ Cse523::MOVZQI2PQIrr,    Cse523::MOVZQI2PQIrm,        0 },
        //{ Cse523::MOVZPQILo2PQIrr, Cse523::MOVZPQILo2PQIrm,     TB_ALIGN_16 },
        //{ Cse523::MOVZX16rr8,      Cse523::MOVZX16rm8,          0 },
        //{ Cse523::MOVZX32rr16,     Cse523::MOVZX32rm16,         0 },
        //{ Cse523::MOVZX32_NOREXrr8, Cse523::MOVZX32_NOREXrm8,   0 },
        //{ Cse523::MOVZX32rr8,      Cse523::MOVZX32rm8,          0 },
        //{ Cse523::PABSBrr128,      Cse523::PABSBrm128,          TB_ALIGN_16 },
        //{ Cse523::PABSDrr128,      Cse523::PABSDrm128,          TB_ALIGN_16 },
        //{ Cse523::PABSWrr128,      Cse523::PABSWrm128,          TB_ALIGN_16 },
        //{ Cse523::PSHUFDri,        Cse523::PSHUFDmi,            TB_ALIGN_16 },
        //{ Cse523::PSHUFHWri,       Cse523::PSHUFHWmi,           TB_ALIGN_16 },
        //{ Cse523::PSHUFLWri,       Cse523::PSHUFLWmi,           TB_ALIGN_16 },
        //{ Cse523::RCPPSr,          Cse523::RCPPSm,              TB_ALIGN_16 },
        //{ Cse523::RCPPSr_Int,      Cse523::RCPPSm_Int,          TB_ALIGN_16 },
        //{ Cse523::RSQRTPSr,        Cse523::RSQRTPSm,            TB_ALIGN_16 },
        //{ Cse523::RSQRTPSr_Int,    Cse523::RSQRTPSm_Int,        TB_ALIGN_16 },
        //{ Cse523::RSQRTSSr,        Cse523::RSQRTSSm,            0 },
        //{ Cse523::RSQRTSSr_Int,    Cse523::RSQRTSSm_Int,        0 },
        //{ Cse523::SQRTPDr,         Cse523::SQRTPDm,             TB_ALIGN_16 },
        //{ Cse523::SQRTPSr,         Cse523::SQRTPSm,             TB_ALIGN_16 },
        //{ Cse523::SQRTSDr,         Cse523::SQRTSDm,             0 },
        //{ Cse523::SQRTSDr_Int,     Cse523::SQRTSDm_Int,         0 },
        //{ Cse523::SQRTSSr,         Cse523::SQRTSSm,             0 },
        //{ Cse523::SQRTSSr_Int,     Cse523::SQRTSSm_Int,         0 },
        //{ Cse523::TEST16rr,        Cse523::TEST16rm,            0 },
        //{ Cse523::TEST32rr,        Cse523::TEST32rm,            0 },
        { Cse523::TEST64rr,        Cse523::TEST64rm,            0 }
        //{ Cse523::TEST8rr,         Cse523::TEST8rm,             0 },
        //// FIXME: TEST*rr EAX,EAX ---> CMP [mem], 0
        //{ Cse523::UCOMISDrr,       Cse523::UCOMISDrm,           0 },
        //{ Cse523::UCOMISSrr,       Cse523::UCOMISSrm,           0 }
    };

    for (unsigned i = 0, e = array_lengthof(OpTbl1); i != e; ++i) {
        unsigned RegOp = OpTbl1[i].RegOp;
        unsigned MemOp = OpTbl1[i].MemOp;
        unsigned Flags = OpTbl1[i].Flags;
        AddTableEntry(RegOp2MemOpTable1, MemOp2RegOpTable,
                RegOp, MemOp,
                // Index 1, folded load
                Flags | TB_INDEX_1 | TB_FOLDED_LOAD);
    }

    static const Cse523OpTblEntry OpTbl2[] = {
        //{ Cse523::ADC32rr,         Cse523::ADC32rm,       0 },
        { Cse523::ADC64rr,         Cse523::ADC64rm,       0 },
        //{ Cse523::ADD16rr,         Cse523::ADD16rm,       0 },
        //{ Cse523::ADD16rr_DB,      Cse523::ADD16rm,       TB_NO_REVERSE },
        //{ Cse523::ADD32rr,         Cse523::ADD32rm,       0 },
        //{ Cse523::ADD32rr_DB,      Cse523::ADD32rm,       TB_NO_REVERSE },
        { Cse523::ADD64rr,         Cse523::ADD64rm,       0 },
        //{ Cse523::ADD64rr_DB,      Cse523::ADD64rm,       TB_NO_REVERSE },
        //{ Cse523::ADD8rr,          Cse523::ADD8rm,        0 },
        //{ Cse523::ADDPDrr,         Cse523::ADDPDrm,       TB_ALIGN_16 },
        //{ Cse523::ADDPSrr,         Cse523::ADDPSrm,       TB_ALIGN_16 },
        //{ Cse523::ADDSDrr,         Cse523::ADDSDrm,       0 },
        //{ Cse523::ADDSSrr,         Cse523::ADDSSrm,       0 },
        //{ Cse523::ADDSUBPDrr,      Cse523::ADDSUBPDrm,    TB_ALIGN_16 },
        //{ Cse523::ADDSUBPSrr,      Cse523::ADDSUBPSrm,    TB_ALIGN_16 },
        //{ Cse523::AND16rr,         Cse523::AND16rm,       0 },
        //{ Cse523::AND32rr,         Cse523::AND32rm,       0 },
        { Cse523::AND64rr,         Cse523::AND64rm,       0 },
        //{ Cse523::AND8rr,          Cse523::AND8rm,        0 },
        //{ Cse523::ANDNPDrr,        Cse523::ANDNPDrm,      TB_ALIGN_16 },
        //{ Cse523::ANDNPSrr,        Cse523::ANDNPSrm,      TB_ALIGN_16 },
        //{ Cse523::ANDPDrr,         Cse523::ANDPDrm,       TB_ALIGN_16 },
        //{ Cse523::ANDPSrr,         Cse523::ANDPSrm,       TB_ALIGN_16 },
        //{ Cse523::BLENDPDrri,      Cse523::BLENDPDrmi,    TB_ALIGN_16 },
        //{ Cse523::BLENDPSrri,      Cse523::BLENDPSrmi,    TB_ALIGN_16 },
        //{ Cse523::BLENDVPDrr0,     Cse523::BLENDVPDrm0,   TB_ALIGN_16 },
        //{ Cse523::BLENDVPSrr0,     Cse523::BLENDVPSrm0,   TB_ALIGN_16 },
        //{ Cse523::CMOVA16rr,       Cse523::CMOVA16rm,     0 },
        //{ Cse523::CMOVA32rr,       Cse523::CMOVA32rm,     0 },
        //{ Cse523::CMOVA64rr,       Cse523::CMOVA64rm,     0 },
        //{ Cse523::CMOVAE16rr,      Cse523::CMOVAE16rm,    0 },
        //{ Cse523::CMOVAE32rr,      Cse523::CMOVAE32rm,    0 },
        //{ Cse523::CMOVAE64rr,      Cse523::CMOVAE64rm,    0 },
        //{ Cse523::CMOVB16rr,       Cse523::CMOVB16rm,     0 },
        //{ Cse523::CMOVB32rr,       Cse523::CMOVB32rm,     0 },
        //{ Cse523::CMOVB64rr,       Cse523::CMOVB64rm,     0 },
        //{ Cse523::CMOVBE16rr,      Cse523::CMOVBE16rm,    0 },
        //{ Cse523::CMOVBE32rr,      Cse523::CMOVBE32rm,    0 },
        //{ Cse523::CMOVBE64rr,      Cse523::CMOVBE64rm,    0 },
        //{ Cse523::CMOVE16rr,       Cse523::CMOVE16rm,     0 },
        //{ Cse523::CMOVE32rr,       Cse523::CMOVE32rm,     0 },
        //{ Cse523::CMOVE64rr,       Cse523::CMOVE64rm,     0 },
        //{ Cse523::CMOVG16rr,       Cse523::CMOVG16rm,     0 },
        //{ Cse523::CMOVG32rr,       Cse523::CMOVG32rm,     0 },
        //{ Cse523::CMOVG64rr,       Cse523::CMOVG64rm,     0 },
        //{ Cse523::CMOVGE16rr,      Cse523::CMOVGE16rm,    0 },
        //{ Cse523::CMOVGE32rr,      Cse523::CMOVGE32rm,    0 },
        //{ Cse523::CMOVGE64rr,      Cse523::CMOVGE64rm,    0 },
        //{ Cse523::CMOVL16rr,       Cse523::CMOVL16rm,     0 },
        //{ Cse523::CMOVL32rr,       Cse523::CMOVL32rm,     0 },
        //{ Cse523::CMOVL64rr,       Cse523::CMOVL64rm,     0 },
        //{ Cse523::CMOVLE16rr,      Cse523::CMOVLE16rm,    0 },
        //{ Cse523::CMOVLE32rr,      Cse523::CMOVLE32rm,    0 },
        //{ Cse523::CMOVLE64rr,      Cse523::CMOVLE64rm,    0 },
        //{ Cse523::CMOVNE16rr,      Cse523::CMOVNE16rm,    0 },
        //{ Cse523::CMOVNE32rr,      Cse523::CMOVNE32rm,    0 },
        //{ Cse523::CMOVNE64rr,      Cse523::CMOVNE64rm,    0 },
        //{ Cse523::CMOVNO16rr,      Cse523::CMOVNO16rm,    0 },
        //{ Cse523::CMOVNO32rr,      Cse523::CMOVNO32rm,    0 },
        //{ Cse523::CMOVNO64rr,      Cse523::CMOVNO64rm,    0 },
        //{ Cse523::CMOVNP16rr,      Cse523::CMOVNP16rm,    0 },
        //{ Cse523::CMOVNP32rr,      Cse523::CMOVNP32rm,    0 },
        //{ Cse523::CMOVNP64rr,      Cse523::CMOVNP64rm,    0 },
        //{ Cse523::CMOVNS16rr,      Cse523::CMOVNS16rm,    0 },
        //{ Cse523::CMOVNS32rr,      Cse523::CMOVNS32rm,    0 },
        //{ Cse523::CMOVNS64rr,      Cse523::CMOVNS64rm,    0 },
        //{ Cse523::CMOVO16rr,       Cse523::CMOVO16rm,     0 },
        //{ Cse523::CMOVO32rr,       Cse523::CMOVO32rm,     0 },
        //{ Cse523::CMOVO64rr,       Cse523::CMOVO64rm,     0 },
        //{ Cse523::CMOVP16rr,       Cse523::CMOVP16rm,     0 },
        //{ Cse523::CMOVP32rr,       Cse523::CMOVP32rm,     0 },
        //{ Cse523::CMOVP64rr,       Cse523::CMOVP64rm,     0 },
        //{ Cse523::CMOVS16rr,       Cse523::CMOVS16rm,     0 },
        //{ Cse523::CMOVS32rr,       Cse523::CMOVS32rm,     0 },
        //{ Cse523::CMOVS64rr,       Cse523::CMOVS64rm,     0 },
        //{ Cse523::CMPPDrri,        Cse523::CMPPDrmi,      TB_ALIGN_16 },
        //{ Cse523::CMPPSrri,        Cse523::CMPPSrmi,      TB_ALIGN_16 },
        //{ Cse523::CMPSDrr,         Cse523::CMPSDrm,       0 },
        //{ Cse523::CMPSSrr,         Cse523::CMPSSrm,       0 },
        //{ Cse523::DIVPDrr,         Cse523::DIVPDrm,       TB_ALIGN_16 },
        //{ Cse523::DIVPSrr,         Cse523::DIVPSrm,       TB_ALIGN_16 },
        //{ Cse523::DIVSDrr,         Cse523::DIVSDrm,       0 },
        //{ Cse523::DIVSSrr,         Cse523::DIVSSrm,       0 },
        //{ Cse523::FsANDNPDrr,      Cse523::FsANDNPDrm,    TB_ALIGN_16 },
        //{ Cse523::FsANDNPSrr,      Cse523::FsANDNPSrm,    TB_ALIGN_16 },
        //{ Cse523::FsANDPDrr,       Cse523::FsANDPDrm,     TB_ALIGN_16 },
        //{ Cse523::FsANDPSrr,       Cse523::FsANDPSrm,     TB_ALIGN_16 },
        //{ Cse523::FsORPDrr,        Cse523::FsORPDrm,      TB_ALIGN_16 },
        //{ Cse523::FsORPSrr,        Cse523::FsORPSrm,      TB_ALIGN_16 },
        //{ Cse523::FsXORPDrr,       Cse523::FsXORPDrm,     TB_ALIGN_16 },
        //{ Cse523::FsXORPSrr,       Cse523::FsXORPSrm,     TB_ALIGN_16 },
        //{ Cse523::HADDPDrr,        Cse523::HADDPDrm,      TB_ALIGN_16 },
        //{ Cse523::HADDPSrr,        Cse523::HADDPSrm,      TB_ALIGN_16 },
        //{ Cse523::HSUBPDrr,        Cse523::HSUBPDrm,      TB_ALIGN_16 },
        //{ Cse523::HSUBPSrr,        Cse523::HSUBPSrm,      TB_ALIGN_16 },
        //{ Cse523::IMUL16rr,        Cse523::IMUL16rm,      0 },
        //{ Cse523::IMUL32rr,        Cse523::IMUL32rm,      0 },
        //{ Cse523::IMUL64rr,        Cse523::IMUL64rm,      0 },
        //{ Cse523::Int_CMPSDrr,     Cse523::Int_CMPSDrm,   0 },
        //{ Cse523::Int_CMPSSrr,     Cse523::Int_CMPSSrm,   0 },
        //{ Cse523::Int_CVTSD2SSrr,  Cse523::Int_CVTSD2SSrm,      0 },
        //{ Cse523::Int_CVTSI2SD64rr,Cse523::Int_CVTSI2SD64rm,    0 },
        //{ Cse523::Int_CVTSI2SDrr,  Cse523::Int_CVTSI2SDrm,      0 },
        //{ Cse523::Int_CVTSI2SS64rr,Cse523::Int_CVTSI2SS64rm,    0 },
        //{ Cse523::Int_CVTSI2SSrr,  Cse523::Int_CVTSI2SSrm,      0 },
        //{ Cse523::Int_CVTSS2SDrr,  Cse523::Int_CVTSS2SDrm,      0 },
        //{ Cse523::MAXPDrr,         Cse523::MAXPDrm,       TB_ALIGN_16 },
        //{ Cse523::MAXPSrr,         Cse523::MAXPSrm,       TB_ALIGN_16 },
        //{ Cse523::MAXSDrr,         Cse523::MAXSDrm,       0 },
        //{ Cse523::MAXSSrr,         Cse523::MAXSSrm,       0 },
        //{ Cse523::MINPDrr,         Cse523::MINPDrm,       TB_ALIGN_16 },
        //{ Cse523::MINPSrr,         Cse523::MINPSrm,       TB_ALIGN_16 },
        //{ Cse523::MINSDrr,         Cse523::MINSDrm,       0 },
        //{ Cse523::MINSSrr,         Cse523::MINSSrm,       0 },
        //{ Cse523::MPSADBWrri,      Cse523::MPSADBWrmi,    TB_ALIGN_16 },
        //{ Cse523::MULPDrr,         Cse523::MULPDrm,       TB_ALIGN_16 },
        //{ Cse523::MULPSrr,         Cse523::MULPSrm,       TB_ALIGN_16 },
        //{ Cse523::MULSDrr,         Cse523::MULSDrm,       0 },
        //{ Cse523::MULSSrr,         Cse523::MULSSrm,       0 },
        //{ Cse523::OR16rr,          Cse523::OR16rm,        0 },
        //{ Cse523::OR32rr,          Cse523::OR32rm,        0 },
        //{ Cse523::OR64rr,          Cse523::OR64rm,        0 },
        //{ Cse523::OR8rr,           Cse523::OR8rm,         0 },
        //{ Cse523::ORPDrr,          Cse523::ORPDrm,        TB_ALIGN_16 },
        //{ Cse523::ORPSrr,          Cse523::ORPSrm,        TB_ALIGN_16 },
        //{ Cse523::PACKSSDWrr,      Cse523::PACKSSDWrm,    TB_ALIGN_16 },
        //{ Cse523::PACKSSWBrr,      Cse523::PACKSSWBrm,    TB_ALIGN_16 },
        //{ Cse523::PACKUSDWrr,      Cse523::PACKUSDWrm,    TB_ALIGN_16 },
        //{ Cse523::PACKUSWBrr,      Cse523::PACKUSWBrm,    TB_ALIGN_16 },
        //{ Cse523::PADDBrr,         Cse523::PADDBrm,       TB_ALIGN_16 },
        //{ Cse523::PADDDrr,         Cse523::PADDDrm,       TB_ALIGN_16 },
        //{ Cse523::PADDQrr,         Cse523::PADDQrm,       TB_ALIGN_16 },
        //{ Cse523::PADDSBrr,        Cse523::PADDSBrm,      TB_ALIGN_16 },
        //{ Cse523::PADDSWrr,        Cse523::PADDSWrm,      TB_ALIGN_16 },
        //{ Cse523::PADDUSBrr,       Cse523::PADDUSBrm,     TB_ALIGN_16 },
        //{ Cse523::PADDUSWrr,       Cse523::PADDUSWrm,     TB_ALIGN_16 },
        //{ Cse523::PADDWrr,         Cse523::PADDWrm,       TB_ALIGN_16 },
        //{ Cse523::PALIGNR128rr,    Cse523::PALIGNR128rm,  TB_ALIGN_16 },
        //{ Cse523::PANDNrr,         Cse523::PANDNrm,       TB_ALIGN_16 },
        //{ Cse523::PANDrr,          Cse523::PANDrm,        TB_ALIGN_16 },
        //{ Cse523::PAVGBrr,         Cse523::PAVGBrm,       TB_ALIGN_16 },
        //{ Cse523::PAVGWrr,         Cse523::PAVGWrm,       TB_ALIGN_16 },
        //{ Cse523::PBLENDWrri,      Cse523::PBLENDWrmi,    TB_ALIGN_16 },
        //{ Cse523::PCMPEQBrr,       Cse523::PCMPEQBrm,     TB_ALIGN_16 },
        //{ Cse523::PCMPEQDrr,       Cse523::PCMPEQDrm,     TB_ALIGN_16 },
        //{ Cse523::PCMPEQQrr,       Cse523::PCMPEQQrm,     TB_ALIGN_16 },
        //{ Cse523::PCMPEQWrr,       Cse523::PCMPEQWrm,     TB_ALIGN_16 },
        //{ Cse523::PCMPGTBrr,       Cse523::PCMPGTBrm,     TB_ALIGN_16 },
        //{ Cse523::PCMPGTDrr,       Cse523::PCMPGTDrm,     TB_ALIGN_16 },
        //{ Cse523::PCMPGTQrr,       Cse523::PCMPGTQrm,     TB_ALIGN_16 },
        //{ Cse523::PCMPGTWrr,       Cse523::PCMPGTWrm,     TB_ALIGN_16 },
        //{ Cse523::PHADDDrr,        Cse523::PHADDDrm,      TB_ALIGN_16 },
        //{ Cse523::PHADDWrr,        Cse523::PHADDWrm,      TB_ALIGN_16 },
        //{ Cse523::PHADDSWrr128,    Cse523::PHADDSWrm128,  TB_ALIGN_16 },
        //{ Cse523::PHSUBDrr,        Cse523::PHSUBDrm,      TB_ALIGN_16 },
        //{ Cse523::PHSUBSWrr128,    Cse523::PHSUBSWrm128,  TB_ALIGN_16 },
        //{ Cse523::PHSUBWrr,        Cse523::PHSUBWrm,      TB_ALIGN_16 },
        //{ Cse523::PINSRWrri,       Cse523::PINSRWrmi,     TB_ALIGN_16 },
        //{ Cse523::PMADDUBSWrr128,  Cse523::PMADDUBSWrm128, TB_ALIGN_16 },
        //{ Cse523::PMADDWDrr,       Cse523::PMADDWDrm,     TB_ALIGN_16 },
        //{ Cse523::PMAXSWrr,        Cse523::PMAXSWrm,      TB_ALIGN_16 },
        //{ Cse523::PMAXUBrr,        Cse523::PMAXUBrm,      TB_ALIGN_16 },
        //{ Cse523::PMINSWrr,        Cse523::PMINSWrm,      TB_ALIGN_16 },
        //{ Cse523::PMINUBrr,        Cse523::PMINUBrm,      TB_ALIGN_16 },
        //{ Cse523::PMINSBrr,        Cse523::PMINSBrm,      TB_ALIGN_16 },
        //{ Cse523::PMINSDrr,        Cse523::PMINSDrm,      TB_ALIGN_16 },
        //{ Cse523::PMINUDrr,        Cse523::PMINUDrm,      TB_ALIGN_16 },
        //{ Cse523::PMINUWrr,        Cse523::PMINUWrm,      TB_ALIGN_16 },
        //{ Cse523::PMAXSBrr,        Cse523::PMAXSBrm,      TB_ALIGN_16 },
        //{ Cse523::PMAXSDrr,        Cse523::PMAXSDrm,      TB_ALIGN_16 },
        //{ Cse523::PMAXUDrr,        Cse523::PMAXUDrm,      TB_ALIGN_16 },
        //{ Cse523::PMAXUWrr,        Cse523::PMAXUWrm,      TB_ALIGN_16 },
        //{ Cse523::PMULDQrr,        Cse523::PMULDQrm,      TB_ALIGN_16 },
        //{ Cse523::PMULHRSWrr128,   Cse523::PMULHRSWrm128, TB_ALIGN_16 },
        //{ Cse523::PMULHUWrr,       Cse523::PMULHUWrm,     TB_ALIGN_16 },
        //{ Cse523::PMULHWrr,        Cse523::PMULHWrm,      TB_ALIGN_16 },
        //{ Cse523::PMULLDrr,        Cse523::PMULLDrm,      TB_ALIGN_16 },
        //{ Cse523::PMULLWrr,        Cse523::PMULLWrm,      TB_ALIGN_16 },
        //{ Cse523::PMULUDQrr,       Cse523::PMULUDQrm,     TB_ALIGN_16 },
        //{ Cse523::PORrr,           Cse523::PORrm,         TB_ALIGN_16 },
        //{ Cse523::PSADBWrr,        Cse523::PSADBWrm,      TB_ALIGN_16 },
        //{ Cse523::PSHUFBrr,        Cse523::PSHUFBrm,      TB_ALIGN_16 },
        //{ Cse523::PSIGNBrr,        Cse523::PSIGNBrm,      TB_ALIGN_16 },
        //{ Cse523::PSIGNWrr,        Cse523::PSIGNWrm,      TB_ALIGN_16 },
        //{ Cse523::PSIGNDrr,        Cse523::PSIGNDrm,      TB_ALIGN_16 },
        //{ Cse523::PSLLDrr,         Cse523::PSLLDrm,       TB_ALIGN_16 },
        //{ Cse523::PSLLQrr,         Cse523::PSLLQrm,       TB_ALIGN_16 },
        //{ Cse523::PSLLWrr,         Cse523::PSLLWrm,       TB_ALIGN_16 },
        //{ Cse523::PSRADrr,         Cse523::PSRADrm,       TB_ALIGN_16 },
        //{ Cse523::PSRAWrr,         Cse523::PSRAWrm,       TB_ALIGN_16 },
        //{ Cse523::PSRLDrr,         Cse523::PSRLDrm,       TB_ALIGN_16 },
        //{ Cse523::PSRLQrr,         Cse523::PSRLQrm,       TB_ALIGN_16 },
        //{ Cse523::PSRLWrr,         Cse523::PSRLWrm,       TB_ALIGN_16 },
        //{ Cse523::PSUBBrr,         Cse523::PSUBBrm,       TB_ALIGN_16 },
        //{ Cse523::PSUBDrr,         Cse523::PSUBDrm,       TB_ALIGN_16 },
        //{ Cse523::PSUBSBrr,        Cse523::PSUBSBrm,      TB_ALIGN_16 },
        //{ Cse523::PSUBSWrr,        Cse523::PSUBSWrm,      TB_ALIGN_16 },
        //{ Cse523::PSUBWrr,         Cse523::PSUBWrm,       TB_ALIGN_16 },
        //{ Cse523::PUNPCKHBWrr,     Cse523::PUNPCKHBWrm,   TB_ALIGN_16 },
        //{ Cse523::PUNPCKHDQrr,     Cse523::PUNPCKHDQrm,   TB_ALIGN_16 },
        //{ Cse523::PUNPCKHQDQrr,    Cse523::PUNPCKHQDQrm,  TB_ALIGN_16 },
        //{ Cse523::PUNPCKHWDrr,     Cse523::PUNPCKHWDrm,   TB_ALIGN_16 },
        //{ Cse523::PUNPCKLBWrr,     Cse523::PUNPCKLBWrm,   TB_ALIGN_16 },
        //{ Cse523::PUNPCKLDQrr,     Cse523::PUNPCKLDQrm,   TB_ALIGN_16 },
        //{ Cse523::PUNPCKLQDQrr,    Cse523::PUNPCKLQDQrm,  TB_ALIGN_16 },
        //{ Cse523::PUNPCKLWDrr,     Cse523::PUNPCKLWDrm,   TB_ALIGN_16 },
        //{ Cse523::PXORrr,          Cse523::PXORrm,        TB_ALIGN_16 },
        //{ Cse523::SBB32rr,         Cse523::SBB32rm,       0 },
        //{ Cse523::SBB64rr,         Cse523::SBB64rm,       0 },
        //{ Cse523::SHUFPDrri,       Cse523::SHUFPDrmi,     TB_ALIGN_16 },
        //{ Cse523::SHUFPSrri,       Cse523::SHUFPSrmi,     TB_ALIGN_16 },
        //{ Cse523::SUB16rr,         Cse523::SUB16rm,       0 },
        //{ Cse523::SUB32rr,         Cse523::SUB32rm,       0 },
        { Cse523::SUB64rr,         Cse523::SUB64rm,       0 },
        //{ Cse523::SUB8rr,          Cse523::SUB8rm,        0 },
        //{ Cse523::SUBPDrr,         Cse523::SUBPDrm,       TB_ALIGN_16 },
        //{ Cse523::SUBPSrr,         Cse523::SUBPSrm,       TB_ALIGN_16 },
        //{ Cse523::SUBSDrr,         Cse523::SUBSDrm,       0 },
        //{ Cse523::SUBSSrr,         Cse523::SUBSSrm,       0 },
        //// FIXME: TEST*rr -> swapped operand of TEST*mr.
        //{ Cse523::UNPCKHPDrr,      Cse523::UNPCKHPDrm,    TB_ALIGN_16 },
        //{ Cse523::UNPCKHPSrr,      Cse523::UNPCKHPSrm,    TB_ALIGN_16 },
        //{ Cse523::UNPCKLPDrr,      Cse523::UNPCKLPDrm,    TB_ALIGN_16 },
        //{ Cse523::UNPCKLPSrr,      Cse523::UNPCKLPSrm,    TB_ALIGN_16 },
        //{ Cse523::XOR16rr,         Cse523::XOR16rm,       0 },
        //{ Cse523::XOR32rr,         Cse523::XOR32rm,       0 },
        { Cse523::XOR64rr,         Cse523::XOR64rm,       0 }
        //{ Cse523::XOR8rr,          Cse523::XOR8rm,        0 },
        //{ Cse523::XORPDrr,         Cse523::XORPDrm,       TB_ALIGN_16 },
        //{ Cse523::XORPSrr,         Cse523::XORPSrm,       TB_ALIGN_16 }
    };

    for (unsigned i = 0, e = array_lengthof(OpTbl2); i != e; ++i) {
        unsigned RegOp = OpTbl2[i].RegOp;
        unsigned MemOp = OpTbl2[i].MemOp;
        unsigned Flags = OpTbl2[i].Flags;
        AddTableEntry(RegOp2MemOpTable2, MemOp2RegOpTable,
                RegOp, MemOp,
                // Index 2, folded load
                Flags | TB_INDEX_2 | TB_FOLDED_LOAD);
    }
//
//    // TODO: Remove this entry
//    static const Cse523OpTblEntry OpTbl3[1] = {
//    };
//
//    for (unsigned i = 0, e = array_lengthof(OpTbl3); i != e; ++i) {
//        unsigned RegOp = OpTbl3[i].RegOp;
//        unsigned MemOp = OpTbl3[i].MemOp;
//        unsigned Flags = OpTbl3[i].Flags;
//        AddTableEntry(RegOp2MemOpTable3, MemOp2RegOpTable,
//                RegOp, MemOp,
//                // Index 3, folded load
//                Flags | TB_INDEX_3 | TB_FOLDED_LOAD);
//    }

}

void
Cse523InstrInfo::AddTableEntry(RegOp2MemOpTableType &R2MTable,
        MemOp2RegOpTableType &M2RTable,
        unsigned RegOp, unsigned MemOp, unsigned Flags) {
    if ((Flags & TB_NO_FORWARD) == 0) {
        assert(!R2MTable.count(RegOp) && "Duplicate entry!");
        R2MTable[RegOp] = std::make_pair(MemOp, Flags);
    }
    if ((Flags & TB_NO_REVERSE) == 0) {
        assert(!M2RTable.count(MemOp) &&
                "Duplicated entries in unfolding maps?");
        M2RTable[MemOp] = std::make_pair(RegOp, Flags);
    }
}

bool
Cse523InstrInfo::isCoalescableExtInstr(const MachineInstr &MI,
        unsigned &SrcReg, unsigned &DstReg,
        unsigned &SubIdx) const {
//    switch (MI.getOpcode()) {
//        default: break;
//        case Cse523::MOVSX16rr8:
//        case Cse523::MOVZX16rr8:
//        case Cse523::MOVSX32rr8:
//        case Cse523::MOVZX32rr8:
//        case Cse523::MOVSX64rr8:
//             if (!TM.getSubtarget<Cse523Subtarget>().is64Bit())
//                 // It's not always legal to reference the low 8-bit of the larger
//                 // register in 32-bit mode.
//                 return false;
//        case Cse523::MOVSX32rr16:
//        case Cse523::MOVZX32rr16:
//        case Cse523::MOVSX64rr16:
//        case Cse523::MOVSX64rr32: 
//        {
//            if (MI.getOperand(0).getSubReg() || MI.getOperand(1).getSubReg())
//                // Be conservative.
//                return false;
//            SrcReg = MI.getOperand(1).getReg();
//            DstReg = MI.getOperand(0).getReg();
//            switch (MI.getOpcode()) {
//                default: llvm_unreachable("Unreachable!");
//                case Cse523::MOVSX16rr8:
//                case Cse523::MOVZX16rr8:
//                case Cse523::MOVSX32rr8:
//                case Cse523::MOVZX32rr8:
//                case Cse523::MOVSX64rr8:
//                         SubIdx = Cse523::sub_8bit;
//                         break;
//                case Cse523::MOVSX32rr16:
//                case Cse523::MOVZX32rr16:
//                case Cse523::MOVSX64rr16:
//                         SubIdx = Cse523::sub_16bit;
//                         break;
//                case Cse523::MOVSX64rr32:
//                         SubIdx = Cse523::sub_32bit;
//                         break;
//            }
//            return true;
//        }
//    }
    return false;
}

/// isFrameOperand - Return true and the FrameIndex if the specified
/// operand and follow operands form a reference to the stack frame.
bool Cse523InstrInfo::isFrameOperand(const MachineInstr *MI, unsigned int Op,
        int &FrameIndex) const {
    if (MI->getOperand(Op).isFI() && MI->getOperand(Op+1).isImm() &&
            MI->getOperand(Op+2).isReg() && MI->getOperand(Op+3).isImm() &&
            MI->getOperand(Op+1).getImm() == 1 &&
            MI->getOperand(Op+2).getReg() == 0 &&
            MI->getOperand(Op+3).getImm() == 0) {
        FrameIndex = MI->getOperand(Op).getIndex();
        return true;
    }
    return false;
}

static bool isFrameLoadOpcode(int Opcode) {
    switch (Opcode) {
        default: break;
        case Cse523::MOV64rm:
//        case Cse523::LD_Fp64m:
//        case Cse523::MOVSSrm:
//        case Cse523::MOVSDrm:
//        case Cse523::MOVAPSrm:
//        case Cse523::MOVAPDrm:
//        case Cse523::MOVDQArm:
            return true;
    }
    return false;
}

static bool isFrameStoreOpcode(int Opcode) {
    switch (Opcode) {
        default: break;
        case Cse523::MOV64mr:
//        case Cse523::ST_FpP64m:
//        case Cse523::MOVSSmr:
//        case Cse523::MOVSDmr:
//        case Cse523::MOVAPSmr:
//        case Cse523::MOVAPDmr:
//        case Cse523::MOVDQAmr:
             return true;
    }
    return false;
}

unsigned Cse523InstrInfo::isLoadFromStackSlot(const MachineInstr *MI,
        int &FrameIndex) const {
    if (isFrameLoadOpcode(MI->getOpcode()))
        if (MI->getOperand(0).getSubReg() == 0 && isFrameOperand(MI, 1, FrameIndex))
            return MI->getOperand(0).getReg();
    return 0;
}

unsigned Cse523InstrInfo::isLoadFromStackSlotPostFE(const MachineInstr *MI,
        int &FrameIndex) const {
    if (isFrameLoadOpcode(MI->getOpcode())) {
        unsigned Reg;
        if ((Reg = isLoadFromStackSlot(MI, FrameIndex)))
            return Reg;
        // Check for post-frame index elimination operations
        const MachineMemOperand *Dummy;
        return hasLoadFromStackSlot(MI, Dummy, FrameIndex);
    }
    return 0;
}

unsigned Cse523InstrInfo::isStoreToStackSlot(const MachineInstr *MI,
        int &FrameIndex) const {
    if (isFrameStoreOpcode(MI->getOpcode()))
        if (MI->getOperand(Cse523::AddrNumOperands).getSubReg() == 0 &&
                isFrameOperand(MI, 0, FrameIndex))
            return MI->getOperand(Cse523::AddrNumOperands).getReg();
    return 0;
}

unsigned Cse523InstrInfo::isStoreToStackSlotPostFE(const MachineInstr *MI,
        int &FrameIndex) const {
    if (isFrameStoreOpcode(MI->getOpcode())) {
        unsigned Reg;
        if ((Reg = isStoreToStackSlot(MI, FrameIndex)))
            return Reg;
        // Check for post-frame index elimination operations
        const MachineMemOperand *Dummy;
        return hasStoreToStackSlot(MI, Dummy, FrameIndex);
    }
    return 0;
}

/// regIsPICBase - Return true if register is PIC base (i.e.g defined by
/// Cse523::MOVPC32r.
static bool regIsPICBase(unsigned BaseReg, const MachineRegisterInfo &MRI) {
    // Don't waste compile time scanning use-def chains of physregs.
    if (!TargetRegisterInfo::isVirtualRegister(BaseReg))
        return false;
    bool isPICBase = false;
    for (MachineRegisterInfo::def_iterator I = MRI.def_begin(BaseReg),
            E = MRI.def_end(); I != E; ++I) {
        MachineInstr *DefMI = I.getOperand().getParent();
        assert(0);
//        if (DefMI->getOpcode() != Cse523::MOVPC32r)
//            return false;
        assert(!isPICBase && "More than one PIC base?");
        isPICBase = true;
    }
    return isPICBase;
}

bool
Cse523InstrInfo::isReallyTriviallyReMaterializable(const MachineInstr *MI,
        AliasAnalysis *AA) const {
    switch (MI->getOpcode()) {
        default: break;
        case Cse523::MOV64rm:
//        case Cse523::LD_Fp64m:
//        case Cse523::MOVSSrm:
//        case Cse523::MOVSDrm:
//        case Cse523::MOVAPSrm:
//        case Cse523::MOVUPSrm:
//        case Cse523::MOVAPDrm:
//        case Cse523::MOVDQArm:
//        case Cse523::MOVDQUrm:
        {
            // Loads from constant pools are trivially rematerializable.
            if (MI->getOperand(1).isReg() &&
                    MI->getOperand(2).isImm() &&
                    MI->getOperand(3).isReg() && MI->getOperand(3).getReg() == 0 &&
                    MI->isInvariantLoad(AA)) {
                unsigned BaseReg = MI->getOperand(1).getReg();
                if (BaseReg == 0 || BaseReg == Cse523::RIP)
                    return true;
                // Allow re-materialization of PIC load.
                if (!ReMatPICStubLoad && MI->getOperand(4).isGlobal())
                    return false;
                const MachineFunction &MF = *MI->getParent()->getParent();
                const MachineRegisterInfo &MRI = MF.getRegInfo();
                return regIsPICBase(BaseReg, MRI);
            }
            return false;
        }

        case Cse523::LEA64r: 
        {
            if (MI->getOperand(2).isImm() &&
                    MI->getOperand(3).isReg() && MI->getOperand(3).getReg() == 0 &&
                    !MI->getOperand(4).isReg()) {
                // lea fi#, lea GV, etc. are all rematerializable.
                if (!MI->getOperand(1).isReg())
                    return true;
                unsigned BaseReg = MI->getOperand(1).getReg();
                if (BaseReg == 0)
                    return true;
                // Allow re-materialization of lea PICBase + x.
                const MachineFunction &MF = *MI->getParent()->getParent();
                const MachineRegisterInfo &MRI = MF.getRegInfo();
                return regIsPICBase(BaseReg, MRI);
            }
            return false;
        }
    }

    // All other instructions marked M_REMATERIALIZABLE are always trivially
    // rematerializable.
    return true;
}

/// isSafeToClobberEFLAGS - Return true if it's safe insert an instruction that
/// would clobber the EFLAGS condition register. Note the result may be
/// conservative. If it cannot definitely determine the safety after visiting
/// a few instructions in each direction it assumes it's not safe.
static bool isSafeToClobberEFLAGS(MachineBasicBlock &MBB,
        MachineBasicBlock::iterator I) {
    MachineBasicBlock::iterator E = MBB.end();

    // For compile time consideration, if we are not able to determine the
    // safety after visiting 4 instructions in each direction, we will assume
    // it's not safe.
    MachineBasicBlock::iterator Iter = I;
    for (unsigned i = 0; Iter != E && i < 4; ++i) {
        bool SeenDef = false;
        for (unsigned j = 0, e = Iter->getNumOperands(); j != e; ++j) {
            MachineOperand &MO = Iter->getOperand(j);
            if (MO.isRegMask() && MO.clobbersPhysReg(Cse523::EFLAGS))
                SeenDef = true;
            if (!MO.isReg())
                continue;
            if (MO.getReg() == Cse523::EFLAGS) {
                if (MO.isUse())
                    return false;
                SeenDef = true;
            }
        }

        if (SeenDef)
            // This instruction defines EFLAGS, no need to look any further.
            return true;
        ++Iter;
        // Skip over DBG_VALUE.
        while (Iter != E && Iter->isDebugValue())
            ++Iter;
    }

    // It is safe to clobber EFLAGS at the end of a block of no successor has it
    // live in.
    if (Iter == E) {
        for (MachineBasicBlock::succ_iterator SI = MBB.succ_begin(),
                SE = MBB.succ_end(); SI != SE; ++SI)
            if ((*SI)->isLiveIn(Cse523::EFLAGS))
                return false;
        return true;
    }

    MachineBasicBlock::iterator B = MBB.begin();
    Iter = I;
    for (unsigned i = 0; i < 4; ++i) {
        // If we make it to the beginning of the block, it's safe to clobber
        // EFLAGS iff EFLAGS is not live-in.
        if (Iter == B)
            return !MBB.isLiveIn(Cse523::EFLAGS);

        --Iter;
        // Skip over DBG_VALUE.
        while (Iter != B && Iter->isDebugValue())
            --Iter;

        bool SawKill = false;
        for (unsigned j = 0, e = Iter->getNumOperands(); j != e; ++j) {
            MachineOperand &MO = Iter->getOperand(j);
            // A register mask may clobber EFLAGS, but we should still look for a
            // live EFLAGS def.
            if (MO.isRegMask() && MO.clobbersPhysReg(Cse523::EFLAGS))
                SawKill = true;
            if (MO.isReg() && MO.getReg() == Cse523::EFLAGS) {
                if (MO.isDef()) return MO.isDead();
                if (MO.isKill()) SawKill = true;
            }
        }

        if (SawKill)
            // This instruction kills EFLAGS and doesn't redefine it, so
            // there's no need to look further.
            return true;
    }

    // Conservative answer.
    return false;
}

void Cse523InstrInfo::reMaterialize(MachineBasicBlock &MBB,
        MachineBasicBlock::iterator I,
        unsigned DestReg, unsigned SubIdx,
        const MachineInstr *Orig,
        const TargetRegisterInfo &TRI) const {
    // MOV64r0 is implemented with a xor which clobbers condition code.
    // Re-materialize it as movri instructions to avoid side effects.
    unsigned Opc = Orig->getOpcode();
    if (Opc == Cse523::MOV64r0 && !isSafeToClobberEFLAGS(MBB, I)) {
        DebugLoc DL = Orig->getDebugLoc();
        BuildMI(MBB, I, DL, get(Cse523::MOV64ri)).addOperand(Orig->getOperand(0))
            .addImm(0);
    } else {
        MachineInstr *MI = MBB.getParent()->CloneMachineInstr(Orig);
        MBB.insert(I, MI);
    }

    MachineInstr *NewMI = prior(I);
    NewMI->substituteRegister(Orig->getOperand(0).getReg(), DestReg, SubIdx, TRI);
}

/// hasLiveCondCodeDef - True if MI has a condition code def, e.g. EFLAGS, that
/// is not marked dead.
static bool hasLiveCondCodeDef(MachineInstr *MI) {
    for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
        MachineOperand &MO = MI->getOperand(i);
        if (MO.isReg() && MO.isDef() &&
                MO.getReg() == Cse523::EFLAGS && !MO.isDead()) {
            return true;
        }
    }
    return false;
}

/// getTruncatedShiftCount - check whether the shift count for a machine operand
/// is non-zero.
inline static unsigned getTruncatedShiftCount(MachineInstr *MI,
        unsigned ShiftAmtOperandIdx) {
    // The shift count is six bits with the REX.W prefix and five bits without.
    unsigned ShiftCountMask = (MI->getDesc().TSFlags & Cse523II::REX_W) ? 63 : 31;
    unsigned Imm = MI->getOperand(ShiftAmtOperandIdx).getImm();
    return Imm & ShiftCountMask;
}

/// isTruncatedShiftCountForLEA - check whether the given shift count is appropriate
/// can be represented by a LEA instruction.
inline static bool isTruncatedShiftCountForLEA(unsigned ShAmt) {
    // Left shift instructions can be transformed into load-effective-address
    // instructions if we can encode them appropriately.
    // A LEA instruction utilizes a SIB byte to encode it's scale factor.
    // The SIB.scale field is two bits wide which means that we can encode any
    // shift amount less than 4.
    return ShAmt < 4 && ShAmt > 0;
}

bool Cse523InstrInfo::classifyLEAReg(MachineInstr *MI, const MachineOperand &Src,
        unsigned Opc, bool AllowSP,
        unsigned &NewSrc, bool &isKill, bool &isUndef,
        MachineOperand &ImplicitOp) const {
    MachineFunction &MF = *MI->getParent()->getParent();
    const TargetRegisterClass *RC;
    if (AllowSP) {
        RC = &Cse523::GR64RegClass;
    } else {
        RC = &Cse523::GR64_NOSPRegClass;
    }
    unsigned SrcReg = Src.getReg();

    assert(0);

    // For both LEA64 and LEA32 the register already has essentially the right
    // type (32-bit or 64-bit) we may just need to forbid SP.
//    if (Opc != Cse523::LEA64_32r) {
//        NewSrc = SrcReg;
//        isKill = Src.isKill();
//        isUndef = Src.isUndef();
//
//        if (TargetRegisterInfo::isVirtualRegister(NewSrc) &&
//                !MF.getRegInfo().constrainRegClass(NewSrc, RC))
//            return false;
//
//        return true;
//    }
//
//    // This is for an LEA64_32r and incoming registers are 32-bit. One way or
//    // another we need to add 64-bit registers to the final MI.
//    if (TargetRegisterInfo::isPhysicalRegister(SrcReg)) {
//        ImplicitOp = Src;
//        ImplicitOp.setImplicit();
//
//        NewSrc = getCse523SubSuperRegister(Src.getReg(), MVT::i64);
//        MachineBasicBlock::LivenessQueryResult LQR =
//            MI->getParent()->computeRegisterLiveness(&getRegisterInfo(), NewSrc, MI);
//
//        switch (LQR) {
//            case MachineBasicBlock::LQR_Unknown:
//                // We can't give sane liveness flags to the instruction, abandon LEA
//                // formation.
//                return false;
//            case MachineBasicBlock::LQR_Live:
//                isKill = MI->killsRegister(SrcReg);
//                isUndef = false;
//                break;
//            default:
//                // The physreg itself is dead, so we have to use it as an <undef>.
//                isKill = false;
//                isUndef = true;
//                break;
//        }
//    } else {
//        // Virtual register of the wrong class, we have to create a temporary 64-bit
//        // vreg to feed into the LEA.
//        NewSrc = MF.getRegInfo().createVirtualRegister(RC);
//        BuildMI(*MI->getParent(), MI, MI->getDebugLoc(),
//                get(TargetOpcode::COPY))
//            .addReg(NewSrc, RegState::Define | RegState::Undef, Cse523::sub_32bit)
//            .addOperand(Src);
//
//        // Which is obviously going to be dead after we're done with it.
//        isKill = true;
//        isUndef = false;
//    }

    // We've set all the parameters without issue.
    return true;
}

/// convertToThreeAddressWithLEA - Helper for convertToThreeAddress when
/// 16-bit LEA is disabled, use 32-bit LEA to form 3-address code by promoting
/// to a 32-bit superregister and then truncating back down to a 16-bit
/// subregister.
MachineInstr *
Cse523InstrInfo::convertToThreeAddressWithLEA(unsigned MIOpc,
        MachineFunction::iterator &MFI,
        MachineBasicBlock::iterator &MBBI,
        LiveVariables *LV) const {

    assert(0);
//    MachineInstr *MI = MBBI;
//    unsigned Dest = MI->getOperand(0).getReg();
//    unsigned Src = MI->getOperand(1).getReg();
//    bool isDead = MI->getOperand(0).isDead();
//    bool isKill = MI->getOperand(1).isKill();
//
//    MachineRegisterInfo &RegInfo = MFI->getParent()->getRegInfo();
//    unsigned leaOutReg = 10;//RegInfo.createVirtualRegister(&Cse523::GR32RegClass); //TODO
//    unsigned Opc, leaInReg;
//
//    Opc = Cse523::LEA64_32r;
//    leaInReg = RegInfo.createVirtualRegister(&Cse523::GR64_NOSPRegClass);
//
//    // Build and insert into an implicit UNDEF value. This is OK because
//    // well be shifting and then extracting the lower 16-bits.
//    // This has the potential to cause partial register stall. e.g.
//    //   movw    (%rbp,%rcx,2), %dx
//    //   leal    -65(%rdx), %esi
//    // But testing has shown this *does* help performance in 64-bit mode (at
//    // least on modern cse523 machines).
//    BuildMI(*MFI, MBBI, MI->getDebugLoc(), get(Cse523::IMPLICIT_DEF), leaInReg);
//    MachineInstr *InsMI =
//        BuildMI(*MFI, MBBI, MI->getDebugLoc(), get(TargetOpcode::COPY))
//        .addReg(leaInReg, RegState::Define, Cse523::sub_16bit)
//        .addReg(Src, getKillRegState(isKill));
//
//    MachineInstrBuilder MIB = BuildMI(*MFI, MBBI, MI->getDebugLoc(),
//            get(Opc), leaOutReg);
//    switch (MIOpc) {
//        default: llvm_unreachable("Unreachable!");
//        case Cse523::SHL16ri: {
//                                  unsigned ShAmt = MI->getOperand(2).getImm();
//                                  MIB.addReg(0).addImm(1 << ShAmt)
//                                      .addReg(leaInReg, RegState::Kill).addImm(0).addReg(0);
//                                  break;
//                              }
//        case Cse523::INC16r:
//        case Cse523::INC64_16r:
//                 addRegOffset(MIB, leaInReg, true, 1);
//                 break;
//        case Cse523::DEC16r:
//        case Cse523::DEC64_16r:
//                 addRegOffset(MIB, leaInReg, true, -1);
//                 break;
//        case Cse523::ADD16ri:
//        case Cse523::ADD16ri8:
//        case Cse523::ADD16ri_DB:
//        case Cse523::ADD16ri8_DB:
//                 addRegOffset(MIB, leaInReg, true, MI->getOperand(2).getImm());
//                 break;
//        case Cse523::ADD16rr:
//        case Cse523::ADD16rr_DB: 
//                 {
//                     unsigned Src2 = MI->getOperand(2).getReg();
//                     bool isKill2 = MI->getOperand(2).isKill();
//                     unsigned leaInReg2 = 0;
//                     MachineInstr *InsMI2 = 0;
//                     if (Src == Src2) {
//                         // ADD16rr %reg1028<kill>, %reg1028
//                         // just a single insert_subreg.
//                         addRegReg(MIB, leaInReg, true, leaInReg, false);
//                     } else {
//                         leaInReg2 = RegInfo.createVirtualRegister(&Cse523::GR64_NOSPRegClass);
//
//                         // Build and insert into an implicit UNDEF value. This is OK because
//                         // well be shifting and then extracting the lower 16-bits.
//                         BuildMI(*MFI, &*MIB, MI->getDebugLoc(), get(Cse523::IMPLICIT_DEF),leaInReg2);
//                         InsMI2 =
//                             BuildMI(*MFI, &*MIB, MI->getDebugLoc(), get(TargetOpcode::COPY))
//                             .addReg(leaInReg2, RegState::Define, Cse523::sub_16bit)
//                             .addReg(Src2, getKillRegState(isKill2));
//                         addRegReg(MIB, leaInReg, true, leaInReg2, true);
//                     }
//                     if (LV && isKill2 && InsMI2)
//                         LV->replaceKillInstruction(Src2, MI, InsMI2);
//                     break;
//                 }
//    }
//
//    MachineInstr *NewMI = MIB;
    MachineInstr *ExtMI = NULL;
//  ExtMI = BuildMI(*MFI, MBBI, MI->getDebugLoc(), get(TargetOpcode::COPY))
//        .addReg(Dest, RegState::Define | getDeadRegState(isDead))
//        .addReg(leaOutReg, RegState::Kill, Cse523::sub_16bit);
//
//    if (LV) {
//        // Update live variables
//        LV->getVarInfo(leaInReg).Kills.push_back(NewMI);
//        LV->getVarInfo(leaOutReg).Kills.push_back(ExtMI);
//        if (isKill)
//            LV->replaceKillInstruction(Src, MI, InsMI);
//        if (isDead)
//            LV->replaceKillInstruction(Dest, MI, ExtMI);
//    }
//
    return ExtMI;
}

/// convertToThreeAddress - This method must be implemented by targets that
/// set the M_CONVERTIBLE_TO_3_ADDR flag.  When this flag is set, the target
/// may be able to convert a two-address instruction into a true
/// three-address instruction on demand.  This allows the Cse523 target (for
/// example) to convert ADD and SHL instructions into LEA instructions if they
/// would require register copies due to two-addressness.
///
/// This method returns a null pointer if the transformation cannot be
/// performed, otherwise it returns the new instruction.
///
MachineInstr *
Cse523InstrInfo::convertToThreeAddress(MachineFunction::iterator &MFI,
        MachineBasicBlock::iterator &MBBI,
        LiveVariables *LV) const {
    MachineInstr *MI = MBBI;
    assert(0);

    // The following opcodes also sets the condition code register(s). Only
    // convert them to equivalent lea if the condition code register def's
    // are dead!
    if (hasLiveCondCodeDef(MI))
        return 0;

    MachineInstr *NewMI = NULL;
//    MachineFunction &MF = *MI->getParent()->getParent();
//    // All instructions input are two-addr instructions.  Get the known operands.
//    const MachineOperand &Dest = MI->getOperand(0);
//    const MachineOperand &Src = MI->getOperand(1);
//
//    // FIXME: 16-bit LEA's are really slow on Athlons, but not bad on P4's.  When
//    // we have better subtarget support, enable the 16-bit LEA generation here.
//    // 16-bit LEA is also slow on Core2.
//    bool DisableLEA16 = true;
//    bool is64Bit = TM.getSubtarget<Cse523Subtarget>().is64Bit();
//
//    unsigned MIOpc = MI->getOpcode();
//    switch (MIOpc) {
//        case Cse523::SHUFPSrri: {
//                                    assert(MI->getNumOperands() == 4 && "Unknown shufps instruction!");
//                                    if (!TM.getSubtarget<Cse523Subtarget>().hasSSE2()) return 0;
//
//                                    unsigned B = MI->getOperand(1).getReg();
//                                    unsigned C = MI->getOperand(2).getReg();
//                                    if (B != C) return 0;
//                                    unsigned M = MI->getOperand(3).getImm();
//                                    NewMI = BuildMI(MF, MI->getDebugLoc(), get(Cse523::PSHUFDri))
//                                        .addOperand(Dest).addOperand(Src).addImm(M);
//                                    break;
//                                }
//        case Cse523::SHUFPDrri: {
//                                    assert(MI->getNumOperands() == 4 && "Unknown shufpd instruction!");
//                                    if (!TM.getSubtarget<Cse523Subtarget>().hasSSE2()) return 0;
//
//                                    unsigned B = MI->getOperand(1).getReg();
//                                    unsigned C = MI->getOperand(2).getReg();
//                                    if (B != C) return 0;
//                                    unsigned M = MI->getOperand(3).getImm();
//
//                                    // Convert to PSHUFD mask.
//                                    M = ((M & 1) << 1) | ((M & 1) << 3) | ((M & 2) << 4) | ((M & 2) << 6)| 0x44;
//
//                                    NewMI = BuildMI(MF, MI->getDebugLoc(), get(Cse523::PSHUFDri))
//                                        .addOperand(Dest).addOperand(Src).addImm(M);
//                                    break;
//                                }
//        case Cse523::SHL64ri: {
//                                  assert(MI->getNumOperands() >= 3 && "Unknown shift instruction!");
//                                  unsigned ShAmt = getTruncatedShiftCount(MI, 2);
//                                  if (!isTruncatedShiftCountForLEA(ShAmt)) return 0;
//
//                                  // LEA can't handle RSP.
//                                  if (TargetRegisterInfo::isVirtualRegister(Src.getReg()) &&
//                                          !MF.getRegInfo().constrainRegClass(Src.getReg(),
//                                              &Cse523::GR64_NOSPRegClass))
//                                      return 0;
//
//                                  NewMI = BuildMI(MF, MI->getDebugLoc(), get(Cse523::LEA64r))
//                                      .addOperand(Dest)
//                                      .addReg(0).addImm(1 << ShAmt).addOperand(Src).addImm(0).addReg(0);
//                                  break;
//                              }
//        case Cse523::SHL32ri: {
//                                  assert(MI->getNumOperands() >= 3 && "Unknown shift instruction!");
//                                  unsigned ShAmt = getTruncatedShiftCount(MI, 2);
//                                  if (!isTruncatedShiftCountForLEA(ShAmt)) return 0;
//
//                                  unsigned Opc = Cse523::LEA64_32r;
//
//                                  // LEA can't handle ESP.
//                                  bool isKill, isUndef;
//                                  unsigned SrcReg;
//                                  MachineOperand ImplicitOp = MachineOperand::CreateReg(0, false);
//                                  if (!classifyLEAReg(MI, Src, Opc, /*AllowSP=*/ false,
//                                              SrcReg, isKill, isUndef, ImplicitOp))
//                                      return 0;
//
//                                  MachineInstrBuilder MIB = BuildMI(MF, MI->getDebugLoc(), get(Opc))
//                                      .addOperand(Dest)
//                                      .addReg(0).addImm(1 << ShAmt)
//                                      .addReg(SrcReg, getKillRegState(isKill) | getUndefRegState(isUndef))
//                                      .addImm(0).addReg(0);
//                                  if (ImplicitOp.getReg() != 0)
//                                      MIB.addOperand(ImplicitOp);
//                                  NewMI = MIB;
//
//                                  break;
//                              }
//        case Cse523::SHL16ri: {
//                                  assert(MI->getNumOperands() >= 3 && "Unknown shift instruction!");
//                                  unsigned ShAmt = getTruncatedShiftCount(MI, 2);
//                                  if (!isTruncatedShiftCountForLEA(ShAmt)) return 0;
//
//                                  if (DisableLEA16)
//                                      return convertToThreeAddressWithLEA(MIOpc, MFI, MBBI, LV);
//                                  NewMI = BuildMI(MF, MI->getDebugLoc(), get(Cse523::LEA16r))
//                                      .addOperand(Dest)
//                                      .addReg(0).addImm(1 << ShAmt).addOperand(Src).addImm(0).addReg(0);
//                                  break;
//                              }
//        default: {
//
//                     switch (MIOpc) {
//                         default: return 0;
//                         case Cse523::INC64r:
//                         case Cse523::INC32r:
//                         case Cse523::INC64_32r: {
//                                                     assert(MI->getNumOperands() >= 2 && "Unknown inc instruction!");
//                                                     unsigned Opc = MIOpc == Cse523::INC64r ? Cse523::LEA64r
//                                                         : Cse523::LEA64_32r;
//                                                     bool isKill, isUndef;
//                                                     unsigned SrcReg;
//                                                     MachineOperand ImplicitOp = MachineOperand::CreateReg(0, false);
//                                                     if (!classifyLEAReg(MI, Src, Opc, /*AllowSP=*/ false,
//                                                                 SrcReg, isKill, isUndef, ImplicitOp))
//                                                         return 0;
//
//                                                     MachineInstrBuilder MIB = BuildMI(MF, MI->getDebugLoc(), get(Opc))
//                                                         .addOperand(Dest)
//                                                         .addReg(SrcReg, getKillRegState(isKill) | getUndefRegState(isUndef));
//                                                     if (ImplicitOp.getReg() != 0)
//                                                         MIB.addOperand(ImplicitOp);
//
//                                                     NewMI = addOffset(MIB, 1);
//                                                     break;
//                                                 }
//                         case Cse523::INC16r:
//                         case Cse523::INC64_16r:
//                                                 if (DisableLEA16)
//                                                     return convertToThreeAddressWithLEA(MIOpc, MFI, MBBI, LV);
//                                                 assert(MI->getNumOperands() >= 2 && "Unknown inc instruction!");
//                                                 NewMI = addOffset(BuildMI(MF, MI->getDebugLoc(), get(Cse523::LEA16r))
//                                                         .addOperand(Dest).addOperand(Src), 1);
//                                                 break;
//                         case Cse523::DEC64r:
//                         case Cse523::DEC32r:
//                         case Cse523::DEC64_32r: {
//                                                     assert(MI->getNumOperands() >= 2 && "Unknown dec instruction!");
//                                                     unsigned Opc = MIOpc == Cse523::DEC64r ? Cse523::LEA64r
//                                                         : Cse523::LEA64_32r;
//
//                                                     bool isKill, isUndef;
//                                                     unsigned SrcReg;
//                                                     MachineOperand ImplicitOp = MachineOperand::CreateReg(0, false);
//                                                     if (!classifyLEAReg(MI, Src, Opc, /*AllowSP=*/ false,
//                                                                 SrcReg, isKill, isUndef, ImplicitOp))
//                                                         return 0;
//
//                                                     MachineInstrBuilder MIB = BuildMI(MF, MI->getDebugLoc(), get(Opc))
//                                                         .addOperand(Dest)
//                                                         .addReg(SrcReg, getUndefRegState(isUndef) | getKillRegState(isKill));
//                                                     if (ImplicitOp.getReg() != 0)
//                                                         MIB.addOperand(ImplicitOp);
//
//                                                     NewMI = addOffset(MIB, -1);
//
//                                                     break;
//                                                 }
//                         case Cse523::DEC16r:
//                         case Cse523::DEC64_16r:
//                                                 if (DisableLEA16)
//                                                     return convertToThreeAddressWithLEA(MIOpc, MFI, MBBI, LV);
//                                                 assert(MI->getNumOperands() >= 2 && "Unknown dec instruction!");
//                                                 NewMI = addOffset(BuildMI(MF, MI->getDebugLoc(), get(Cse523::LEA16r))
//                                                         .addOperand(Dest).addOperand(Src), -1);
//                                                 break;
//                         case Cse523::ADD64rr:
//                         case Cse523::ADD64rr_DB:
//                         case Cse523::ADD32rr:
//                         case Cse523::ADD32rr_DB: {
//                                                      assert(MI->getNumOperands() >= 3 && "Unknown add instruction!");
//                                                      unsigned Opc;
//                                                      if (MIOpc == Cse523::ADD64rr || MIOpc == Cse523::ADD64rr_DB)
//                                                          Opc = Cse523::LEA64r;
//                                                      else
//                                                          Opc = Cse523::LEA64_32r;
//
//                                                      bool isKill, isUndef;
//                                                      unsigned SrcReg;
//                                                      MachineOperand ImplicitOp = MachineOperand::CreateReg(0, false);
//                                                      if (!classifyLEAReg(MI, Src, Opc, /*AllowSP=*/ true,
//                                                                  SrcReg, isKill, isUndef, ImplicitOp))
//                                                          return 0;
//
//                                                      const MachineOperand &Src2 = MI->getOperand(2);
//                                                      bool isKill2, isUndef2;
//                                                      unsigned SrcReg2;
//                                                      MachineOperand ImplicitOp2 = MachineOperand::CreateReg(0, false);
//                                                      if (!classifyLEAReg(MI, Src2, Opc, /*AllowSP=*/ false,
//                                                                  SrcReg2, isKill2, isUndef2, ImplicitOp2))
//                                                          return 0;
//
//                                                      MachineInstrBuilder MIB = BuildMI(MF, MI->getDebugLoc(), get(Opc))
//                                                          .addOperand(Dest);
//                                                      if (ImplicitOp.getReg() != 0)
//                                                          MIB.addOperand(ImplicitOp);
//                                                      if (ImplicitOp2.getReg() != 0)
//                                                          MIB.addOperand(ImplicitOp2);
//
//                                                      NewMI = addRegReg(MIB, SrcReg, isKill, SrcReg2, isKill2);
//
//                                                      // Preserve undefness of the operands.
//                                                      NewMI->getOperand(1).setIsUndef(isUndef);
//                                                      NewMI->getOperand(3).setIsUndef(isUndef2);
//
//                                                      if (LV && Src2.isKill())
//                                                          LV->replaceKillInstruction(SrcReg2, MI, NewMI);
//                                                      break;
//                                                  }
//                         case Cse523::ADD16rr:
//                         case Cse523::ADD16rr_DB: {
//                                                      if (DisableLEA16)
//                                                          return convertToThreeAddressWithLEA(MIOpc, MFI, MBBI, LV);
//                                                      assert(MI->getNumOperands() >= 3 && "Unknown add instruction!");
//                                                      unsigned Src2 = MI->getOperand(2).getReg();
//                                                      bool isKill2 = MI->getOperand(2).isKill();
//                                                      NewMI = addRegReg(BuildMI(MF, MI->getDebugLoc(), get(Cse523::LEA16r))
//                                                              .addOperand(Dest),
//                                                              Src.getReg(), Src.isKill(), Src2, isKill2);
//
//                                                      // Preserve undefness of the operands.
//                                                      bool isUndef = MI->getOperand(1).isUndef();
//                                                      bool isUndef2 = MI->getOperand(2).isUndef();
//                                                      NewMI->getOperand(1).setIsUndef(isUndef);
//                                                      NewMI->getOperand(3).setIsUndef(isUndef2);
//
//                                                      if (LV && isKill2)
//                                                          LV->replaceKillInstruction(Src2, MI, NewMI);
//                                                      break;
//                                                  }
//                         case Cse523::ADD64ri32:
//                         case Cse523::ADD64ri8:
//                         case Cse523::ADD64ri32_DB:
//                         case Cse523::ADD64ri8_DB:
//                                                  assert(MI->getNumOperands() >= 3 && "Unknown add instruction!");
//                                                  NewMI = addOffset(BuildMI(MF, MI->getDebugLoc(), get(Cse523::LEA64r))
//                                                          .addOperand(Dest).addOperand(Src),
//                                                          MI->getOperand(2).getImm());
//                                                  break;
//                         case Cse523::ADD32ri:
//                         case Cse523::ADD32ri8:
//                         case Cse523::ADD32ri_DB:
//                         case Cse523::ADD32ri8_DB: {
//                                                       assert(MI->getNumOperands() >= 3 && "Unknown add instruction!");
//                                                       unsigned Opc = Cse523::LEA64_32r;
//
//                                                       bool isKill, isUndef;
//                                                       unsigned SrcReg;
//                                                       MachineOperand ImplicitOp = MachineOperand::CreateReg(0, false);
//                                                       if (!classifyLEAReg(MI, Src, Opc, /*AllowSP=*/ true,
//                                                                   SrcReg, isKill, isUndef, ImplicitOp))
//                                                           return 0;
//
//                                                       MachineInstrBuilder MIB = BuildMI(MF, MI->getDebugLoc(), get(Opc))
//                                                           .addOperand(Dest)
//                                                           .addReg(SrcReg, getUndefRegState(isUndef) | getKillRegState(isKill));
//                                                       if (ImplicitOp.getReg() != 0)
//                                                           MIB.addOperand(ImplicitOp);
//
//                                                       NewMI = addOffset(MIB, MI->getOperand(2).getImm());
//                                                       break;
//                                                   }
//                         case Cse523::ADD16ri:
//                         case Cse523::ADD16ri8:
//                         case Cse523::ADD16ri_DB:
//                         case Cse523::ADD16ri8_DB:
//                                                   if (DisableLEA16)
//                                                       return convertToThreeAddressWithLEA(MIOpc, MFI, MBBI, LV);
//                                                   assert(MI->getNumOperands() >= 3 && "Unknown add instruction!");
//                                                   NewMI = addOffset(BuildMI(MF, MI->getDebugLoc(), get(Cse523::LEA16r))
//                                                           .addOperand(Dest).addOperand(Src),
//                                                           MI->getOperand(2).getImm());
//                                                   break;
//                     }
//                 }
//    }
//
//    if (!NewMI) return 0;
//
//    if (LV) {  // Update live variables
//        if (Src.isKill())
//            LV->replaceKillInstruction(Src.getReg(), MI, NewMI);
//        if (Dest.isDead())
//            LV->replaceKillInstruction(Dest.getReg(), MI, NewMI);
//    }
//
//    MFI->insert(MBBI, NewMI);          // Insert the new inst
    return NewMI;
}

/// commuteInstruction - We have a few instructions that must be hacked on to
/// commute them.
///
MachineInstr *
Cse523InstrInfo::commuteInstruction(MachineInstr *MI, bool NewMI) const {

    assert(0);
    switch (MI->getOpcode()) {
//        case Cse523::SHRD16rri8: // A = SHRD16rri8 B, C, I -> A = SHLD16rri8 C, B, (16-I)
//        case Cse523::SHLD16rri8: // A = SHLD16rri8 B, C, I -> A = SHRD16rri8 C, B, (16-I)
//        case Cse523::SHRD32rri8: // A = SHRD32rri8 B, C, I -> A = SHLD32rri8 C, B, (32-I)
//        case Cse523::SHLD32rri8: // A = SHLD32rri8 B, C, I -> A = SHRD32rri8 C, B, (32-I)
//        case Cse523::SHRD64rri8: // A = SHRD64rri8 B, C, I -> A = SHLD64rri8 C, B, (64-I)
//        case Cse523::SHLD64rri8:{// A = SHLD64rri8 B, C, I -> A = SHRD64rri8 C, B, (64-I)
//                                    unsigned Opc;
//                                    unsigned Size;
//                                    switch (MI->getOpcode()) {
//                                        default: llvm_unreachable("Unreachable!");
//                                        case Cse523::SHRD16rri8: Size = 16; Opc = Cse523::SHLD16rri8; break;
//                                        case Cse523::SHLD16rri8: Size = 16; Opc = Cse523::SHRD16rri8; break;
//                                        case Cse523::SHRD32rri8: Size = 32; Opc = Cse523::SHLD32rri8; break;
//                                        case Cse523::SHLD32rri8: Size = 32; Opc = Cse523::SHRD32rri8; break;
//                                        case Cse523::SHRD64rri8: Size = 64; Opc = Cse523::SHLD64rri8; break;
//                                        case Cse523::SHLD64rri8: Size = 64; Opc = Cse523::SHRD64rri8; break;
//                                    }
//                                    unsigned Amt = MI->getOperand(3).getImm();
//                                    if (NewMI) {
//                                        MachineFunction &MF = *MI->getParent()->getParent();
//                                        MI = MF.CloneMachineInstr(MI);
//                                        NewMI = false;
//                                    }
//                                    MI->setDesc(get(Opc));
//                                    MI->getOperand(3).setImm(Size-Amt);
//                                    return TargetInstrInfo::commuteInstruction(MI, NewMI);
//                                }
//        case Cse523::CMOVB16rr:  case Cse523::CMOVB32rr:  case Cse523::CMOVB64rr:
//        case Cse523::CMOVAE16rr: case Cse523::CMOVAE32rr: case Cse523::CMOVAE64rr:
//        case Cse523::CMOVE16rr:  case Cse523::CMOVE32rr:  case Cse523::CMOVE64rr:
//        case Cse523::CMOVNE16rr: case Cse523::CMOVNE32rr: case Cse523::CMOVNE64rr:
//        case Cse523::CMOVBE16rr: case Cse523::CMOVBE32rr: case Cse523::CMOVBE64rr:
//        case Cse523::CMOVA16rr:  case Cse523::CMOVA32rr:  case Cse523::CMOVA64rr:
//        case Cse523::CMOVL16rr:  case Cse523::CMOVL32rr:  case Cse523::CMOVL64rr:
//        case Cse523::CMOVGE16rr: case Cse523::CMOVGE32rr: case Cse523::CMOVGE64rr:
//        case Cse523::CMOVLE16rr: case Cse523::CMOVLE32rr: case Cse523::CMOVLE64rr:
//        case Cse523::CMOVG16rr:  case Cse523::CMOVG32rr:  case Cse523::CMOVG64rr:
//        case Cse523::CMOVS16rr:  case Cse523::CMOVS32rr:  case Cse523::CMOVS64rr:
//        case Cse523::CMOVNS16rr: case Cse523::CMOVNS32rr: case Cse523::CMOVNS64rr:
//        case Cse523::CMOVP16rr:  case Cse523::CMOVP32rr:  case Cse523::CMOVP64rr:
//        case Cse523::CMOVNP16rr: case Cse523::CMOVNP32rr: case Cse523::CMOVNP64rr:
//        case Cse523::CMOVO16rr:  case Cse523::CMOVO32rr:  case Cse523::CMOVO64rr:
//        case Cse523::CMOVNO16rr: case Cse523::CMOVNO32rr: case Cse523::CMOVNO64rr: 
//        {
//            unsigned Opc;
//            switch (MI->getOpcode()) {
//                default: llvm_unreachable("Unreachable!");
//                case Cse523::CMOVB16rr:  Opc = Cse523::CMOVAE16rr; break;
//                case Cse523::CMOVB32rr:  Opc = Cse523::CMOVAE32rr; break;
//                case Cse523::CMOVB64rr:  Opc = Cse523::CMOVAE64rr; break;
//                case Cse523::CMOVAE16rr: Opc = Cse523::CMOVB16rr; break;
//                case Cse523::CMOVAE32rr: Opc = Cse523::CMOVB32rr; break;
//                case Cse523::CMOVAE64rr: Opc = Cse523::CMOVB64rr; break;
//                case Cse523::CMOVE16rr:  Opc = Cse523::CMOVNE16rr; break;
//                case Cse523::CMOVE32rr:  Opc = Cse523::CMOVNE32rr; break;
//                case Cse523::CMOVE64rr:  Opc = Cse523::CMOVNE64rr; break;
//                case Cse523::CMOVNE16rr: Opc = Cse523::CMOVE16rr; break;
//                case Cse523::CMOVNE32rr: Opc = Cse523::CMOVE32rr; break;
//                case Cse523::CMOVNE64rr: Opc = Cse523::CMOVE64rr; break;
//                case Cse523::CMOVBE16rr: Opc = Cse523::CMOVA16rr; break;
//                case Cse523::CMOVBE32rr: Opc = Cse523::CMOVA32rr; break;
//                case Cse523::CMOVBE64rr: Opc = Cse523::CMOVA64rr; break;
//                case Cse523::CMOVA16rr:  Opc = Cse523::CMOVBE16rr; break;
//                case Cse523::CMOVA32rr:  Opc = Cse523::CMOVBE32rr; break;
//                case Cse523::CMOVA64rr:  Opc = Cse523::CMOVBE64rr; break;
//                case Cse523::CMOVL16rr:  Opc = Cse523::CMOVGE16rr; break;
//                case Cse523::CMOVL32rr:  Opc = Cse523::CMOVGE32rr; break;
//                case Cse523::CMOVL64rr:  Opc = Cse523::CMOVGE64rr; break;
//                case Cse523::CMOVGE16rr: Opc = Cse523::CMOVL16rr; break;
//                case Cse523::CMOVGE32rr: Opc = Cse523::CMOVL32rr; break;
//                case Cse523::CMOVGE64rr: Opc = Cse523::CMOVL64rr; break;
//                case Cse523::CMOVLE16rr: Opc = Cse523::CMOVG16rr; break;
//                case Cse523::CMOVLE32rr: Opc = Cse523::CMOVG32rr; break;
//                case Cse523::CMOVLE64rr: Opc = Cse523::CMOVG64rr; break;
//                case Cse523::CMOVG16rr:  Opc = Cse523::CMOVLE16rr; break;
//                case Cse523::CMOVG32rr:  Opc = Cse523::CMOVLE32rr; break;
//                case Cse523::CMOVG64rr:  Opc = Cse523::CMOVLE64rr; break;
//                case Cse523::CMOVS16rr:  Opc = Cse523::CMOVNS16rr; break;
//                case Cse523::CMOVS32rr:  Opc = Cse523::CMOVNS32rr; break;
//                case Cse523::CMOVS64rr:  Opc = Cse523::CMOVNS64rr; break;
//                case Cse523::CMOVNS16rr: Opc = Cse523::CMOVS16rr; break;
//                case Cse523::CMOVNS32rr: Opc = Cse523::CMOVS32rr; break;
//                case Cse523::CMOVNS64rr: Opc = Cse523::CMOVS64rr; break;
//                case Cse523::CMOVP16rr:  Opc = Cse523::CMOVNP16rr; break;
//                case Cse523::CMOVP32rr:  Opc = Cse523::CMOVNP32rr; break;
//                case Cse523::CMOVP64rr:  Opc = Cse523::CMOVNP64rr; break;
//                case Cse523::CMOVNP16rr: Opc = Cse523::CMOVP16rr; break;
//                case Cse523::CMOVNP32rr: Opc = Cse523::CMOVP32rr; break;
//                case Cse523::CMOVNP64rr: Opc = Cse523::CMOVP64rr; break;
//                case Cse523::CMOVO16rr:  Opc = Cse523::CMOVNO16rr; break;
//                case Cse523::CMOVO32rr:  Opc = Cse523::CMOVNO32rr; break;
//                case Cse523::CMOVO64rr:  Opc = Cse523::CMOVNO64rr; break;
//                case Cse523::CMOVNO16rr: Opc = Cse523::CMOVO16rr; break;
//                case Cse523::CMOVNO32rr: Opc = Cse523::CMOVO32rr; break;
//                case Cse523::CMOVNO64rr: Opc = Cse523::CMOVO64rr; break;
//            }
//            if (NewMI) {
//                MachineFunction &MF = *MI->getParent()->getParent();
//                MI = MF.CloneMachineInstr(MI);
//                NewMI = false;
//            }
//            MI->setDesc(get(Opc));
//            // Fallthrough intended.
//        }
        default:
            return TargetInstrInfo::commuteInstruction(MI, NewMI);
    }
}

static Cse523::CondCode getCondFromBranchOpc(unsigned BrOpc) {
    assert(0);
    switch (BrOpc) {
        default: return Cse523::COND_INVALID;
        //case Cse523::JE_4:  return Cse523::COND_E;
        //case Cse523::JNE_4: return Cse523::COND_NE;
        //case Cse523::JL_4:  return Cse523::COND_L;
        //case Cse523::JLE_4: return Cse523::COND_LE;
        //case Cse523::JG_4:  return Cse523::COND_G;
        //case Cse523::JGE_4: return Cse523::COND_GE;
        //case Cse523::JB_4:  return Cse523::COND_B;
        //case Cse523::JBE_4: return Cse523::COND_BE;
        //case Cse523::JA_4:  return Cse523::COND_A;
        //case Cse523::JAE_4: return Cse523::COND_AE;
        //case Cse523::JS_4:  return Cse523::COND_S;
        //case Cse523::JNS_4: return Cse523::COND_NS;
        //case Cse523::JP_4:  return Cse523::COND_P;
        //case Cse523::JNP_4: return Cse523::COND_NP;
        //case Cse523::JO_4:  return Cse523::COND_O;
        //case Cse523::JNO_4: return Cse523::COND_NO;
    }
}

/// getCondFromSETOpc - return condition code of a SET opcode.
static Cse523::CondCode getCondFromSETOpc(unsigned Opc) {
    assert(0);
    switch (Opc) {
        default: return Cse523::COND_INVALID;
        //case Cse523::SETAr:  case Cse523::SETAm:  return Cse523::COND_A;
        //case Cse523::SETAEr: case Cse523::SETAEm: return Cse523::COND_AE;
        //case Cse523::SETBr:  case Cse523::SETBm:  return Cse523::COND_B;
        //case Cse523::SETBEr: case Cse523::SETBEm: return Cse523::COND_BE;
        //case Cse523::SETEr:  case Cse523::SETEm:  return Cse523::COND_E;
        //case Cse523::SETGr:  case Cse523::SETGm:  return Cse523::COND_G;
        //case Cse523::SETGEr: case Cse523::SETGEm: return Cse523::COND_GE;
        //case Cse523::SETLr:  case Cse523::SETLm:  return Cse523::COND_L;
        //case Cse523::SETLEr: case Cse523::SETLEm: return Cse523::COND_LE;
        //case Cse523::SETNEr: case Cse523::SETNEm: return Cse523::COND_NE;
        //case Cse523::SETNOr: case Cse523::SETNOm: return Cse523::COND_NO;
        //case Cse523::SETNPr: case Cse523::SETNPm: return Cse523::COND_NP;
        //case Cse523::SETNSr: case Cse523::SETNSm: return Cse523::COND_NS;
        //case Cse523::SETOr:  case Cse523::SETOm:  return Cse523::COND_O;
        //case Cse523::SETPr:  case Cse523::SETPm:  return Cse523::COND_P;
        //case Cse523::SETSr:  case Cse523::SETSm:  return Cse523::COND_S;
    }
}

/// getCondFromCmovOpc - return condition code of a CMov opcode.
Cse523::CondCode Cse523::getCondFromCMovOpc(unsigned Opc) {
    assert(0);
    switch (Opc) {
        default: return Cse523::COND_INVALID;
        //case Cse523::CMOVA16rm:  case Cse523::CMOVA16rr:  case Cse523::CMOVA32rm:
        //case Cse523::CMOVA32rr:  case Cse523::CMOVA64rm:  case Cse523::CMOVA64rr:
        //         return Cse523::COND_A;
        //case Cse523::CMOVAE16rm: case Cse523::CMOVAE16rr: case Cse523::CMOVAE32rm:
        //case Cse523::CMOVAE32rr: case Cse523::CMOVAE64rm: case Cse523::CMOVAE64rr:
        //         return Cse523::COND_AE;
        //case Cse523::CMOVB16rm:  case Cse523::CMOVB16rr:  case Cse523::CMOVB32rm:
        //case Cse523::CMOVB32rr:  case Cse523::CMOVB64rm:  case Cse523::CMOVB64rr:
        //         return Cse523::COND_B;
        //case Cse523::CMOVBE16rm: case Cse523::CMOVBE16rr: case Cse523::CMOVBE32rm:
        //case Cse523::CMOVBE32rr: case Cse523::CMOVBE64rm: case Cse523::CMOVBE64rr:
        //         return Cse523::COND_BE;
        //case Cse523::CMOVE16rm:  case Cse523::CMOVE16rr:  case Cse523::CMOVE32rm:
        //case Cse523::CMOVE32rr:  case Cse523::CMOVE64rm:  case Cse523::CMOVE64rr:
        //         return Cse523::COND_E;
        //case Cse523::CMOVG16rm:  case Cse523::CMOVG16rr:  case Cse523::CMOVG32rm:
        //case Cse523::CMOVG32rr:  case Cse523::CMOVG64rm:  case Cse523::CMOVG64rr:
        //         return Cse523::COND_G;
        //case Cse523::CMOVGE16rm: case Cse523::CMOVGE16rr: case Cse523::CMOVGE32rm:
        //case Cse523::CMOVGE32rr: case Cse523::CMOVGE64rm: case Cse523::CMOVGE64rr:
        //         return Cse523::COND_GE;
        //case Cse523::CMOVL16rm:  case Cse523::CMOVL16rr:  case Cse523::CMOVL32rm:
        //case Cse523::CMOVL32rr:  case Cse523::CMOVL64rm:  case Cse523::CMOVL64rr:
        //         return Cse523::COND_L;
        //case Cse523::CMOVLE16rm: case Cse523::CMOVLE16rr: case Cse523::CMOVLE32rm:
        //case Cse523::CMOVLE32rr: case Cse523::CMOVLE64rm: case Cse523::CMOVLE64rr:
        //         return Cse523::COND_LE;
        //case Cse523::CMOVNE16rm: case Cse523::CMOVNE16rr: case Cse523::CMOVNE32rm:
        //case Cse523::CMOVNE32rr: case Cse523::CMOVNE64rm: case Cse523::CMOVNE64rr:
        //         return Cse523::COND_NE;
        //case Cse523::CMOVNO16rm: case Cse523::CMOVNO16rr: case Cse523::CMOVNO32rm:
        //case Cse523::CMOVNO32rr: case Cse523::CMOVNO64rm: case Cse523::CMOVNO64rr:
        //         return Cse523::COND_NO;
        //case Cse523::CMOVNP16rm: case Cse523::CMOVNP16rr: case Cse523::CMOVNP32rm:
        //case Cse523::CMOVNP32rr: case Cse523::CMOVNP64rm: case Cse523::CMOVNP64rr:
        //         return Cse523::COND_NP;
        //case Cse523::CMOVNS16rm: case Cse523::CMOVNS16rr: case Cse523::CMOVNS32rm:
        //case Cse523::CMOVNS32rr: case Cse523::CMOVNS64rm: case Cse523::CMOVNS64rr:
        //         return Cse523::COND_NS;
        //case Cse523::CMOVO16rm:  case Cse523::CMOVO16rr:  case Cse523::CMOVO32rm:
        //case Cse523::CMOVO32rr:  case Cse523::CMOVO64rm:  case Cse523::CMOVO64rr:
        //         return Cse523::COND_O;
        //case Cse523::CMOVP16rm:  case Cse523::CMOVP16rr:  case Cse523::CMOVP32rm:
        //case Cse523::CMOVP32rr:  case Cse523::CMOVP64rm:  case Cse523::CMOVP64rr:
        //         return Cse523::COND_P;
        //case Cse523::CMOVS16rm:  case Cse523::CMOVS16rr:  case Cse523::CMOVS32rm:
        //case Cse523::CMOVS32rr:  case Cse523::CMOVS64rm:  case Cse523::CMOVS64rr:
        //         return Cse523::COND_S;
    }
}

unsigned Cse523::GetCondBranchFromCond(Cse523::CondCode CC) {
    assert(0);
    switch (CC) {
        default: llvm_unreachable("Illegal condition code!");
        //case Cse523::COND_E:  return Cse523::JE_4;
        //case Cse523::COND_NE: return Cse523::JNE_4;
        //case Cse523::COND_L:  return Cse523::JL_4;
        //case Cse523::COND_LE: return Cse523::JLE_4;
        //case Cse523::COND_G:  return Cse523::JG_4;
        //case Cse523::COND_GE: return Cse523::JGE_4;
        //case Cse523::COND_B:  return Cse523::JB_4;
        //case Cse523::COND_BE: return Cse523::JBE_4;
        //case Cse523::COND_A:  return Cse523::JA_4;
        //case Cse523::COND_AE: return Cse523::JAE_4;
        //case Cse523::COND_S:  return Cse523::JS_4;
        //case Cse523::COND_NS: return Cse523::JNS_4;
        //case Cse523::COND_P:  return Cse523::JP_4;
        //case Cse523::COND_NP: return Cse523::JNP_4;
        //case Cse523::COND_O:  return Cse523::JO_4;
        //case Cse523::COND_NO: return Cse523::JNO_4;
    }
}

/// GetOppositeBranchCondition - Return the inverse of the specified condition,
/// e.g. turning COND_E to COND_NE.
Cse523::CondCode Cse523::GetOppositeBranchCondition(Cse523::CondCode CC) {
    assert(0);
    switch (CC) {
        default: llvm_unreachable("Illegal condition code!");
        //case Cse523::COND_E:  return Cse523::COND_NE;
        //case Cse523::COND_NE: return Cse523::COND_E;
        //case Cse523::COND_L:  return Cse523::COND_GE;
        //case Cse523::COND_LE: return Cse523::COND_G;
        //case Cse523::COND_G:  return Cse523::COND_LE;
        //case Cse523::COND_GE: return Cse523::COND_L;
        //case Cse523::COND_B:  return Cse523::COND_AE;
        //case Cse523::COND_BE: return Cse523::COND_A;
        //case Cse523::COND_A:  return Cse523::COND_BE;
        //case Cse523::COND_AE: return Cse523::COND_B;
        //case Cse523::COND_S:  return Cse523::COND_NS;
        //case Cse523::COND_NS: return Cse523::COND_S;
        //case Cse523::COND_P:  return Cse523::COND_NP;
        //case Cse523::COND_NP: return Cse523::COND_P;
        //case Cse523::COND_O:  return Cse523::COND_NO;
        //case Cse523::COND_NO: return Cse523::COND_O;
    }
}

/// getSwappedCondition - assume the flags are set by MI(a,b), return
/// the condition code if we modify the instructions such that flags are
/// set by MI(b,a).
static Cse523::CondCode getSwappedCondition(Cse523::CondCode CC) {
    switch (CC) {
        default: return Cse523::COND_INVALID;
        case Cse523::COND_E:  return Cse523::COND_E;
        case Cse523::COND_NE: return Cse523::COND_NE;
        case Cse523::COND_L:  return Cse523::COND_G;
        case Cse523::COND_LE: return Cse523::COND_GE;
        case Cse523::COND_G:  return Cse523::COND_L;
        case Cse523::COND_GE: return Cse523::COND_LE;
        case Cse523::COND_B:  return Cse523::COND_A;
        case Cse523::COND_BE: return Cse523::COND_AE;
        case Cse523::COND_A:  return Cse523::COND_B;
        case Cse523::COND_AE: return Cse523::COND_BE;
    }
}

/// getSETFromCond - Return a set opcode for the given condition and
/// whether it has memory operand.
static unsigned getSETFromCond(Cse523::CondCode CC,
        bool HasMemoryOperand) {
    assert(0);
    static const uint16_t Opc[16][2] = {
//        { Cse523::SETAr,  Cse523::SETAm  },
//        { Cse523::SETAEr, Cse523::SETAEm },
//        { Cse523::SETBr,  Cse523::SETBm  },
//        { Cse523::SETBEr, Cse523::SETBEm },
//        { Cse523::SETEr,  Cse523::SETEm  },
//        { Cse523::SETGr,  Cse523::SETGm  },
//        { Cse523::SETGEr, Cse523::SETGEm },
//        { Cse523::SETLr,  Cse523::SETLm  },
//        { Cse523::SETLEr, Cse523::SETLEm },
//        { Cse523::SETNEr, Cse523::SETNEm },
//        { Cse523::SETNOr, Cse523::SETNOm },
//        { Cse523::SETNPr, Cse523::SETNPm },
//        { Cse523::SETNSr, Cse523::SETNSm },
//        { Cse523::SETOr,  Cse523::SETOm  },
//        { Cse523::SETPr,  Cse523::SETPm  },
//        { Cse523::SETSr,  Cse523::SETSm  }
    };

    assert(CC < 16 && "Can only handle standard cond codes");
    return Opc[CC][HasMemoryOperand ? 1 : 0];
}

/// getCMovFromCond - Return a cmov opcode for the given condition,
/// register size in bytes, and operand type.
static unsigned getCMovFromCond(Cse523::CondCode CC, unsigned RegBytes,
        bool HasMemoryOperand) {
    assert(0);
    static const uint16_t Opc[32][3] = {
//        { Cse523::CMOVA16rr,  Cse523::CMOVA32rr,  Cse523::CMOVA64rr  },
//        { Cse523::CMOVAE16rr, Cse523::CMOVAE32rr, Cse523::CMOVAE64rr },
//        { Cse523::CMOVB16rr,  Cse523::CMOVB32rr,  Cse523::CMOVB64rr  },
//        { Cse523::CMOVBE16rr, Cse523::CMOVBE32rr, Cse523::CMOVBE64rr },
//        { Cse523::CMOVE16rr,  Cse523::CMOVE32rr,  Cse523::CMOVE64rr  },
//        { Cse523::CMOVG16rr,  Cse523::CMOVG32rr,  Cse523::CMOVG64rr  },
//        { Cse523::CMOVGE16rr, Cse523::CMOVGE32rr, Cse523::CMOVGE64rr },
//        { Cse523::CMOVL16rr,  Cse523::CMOVL32rr,  Cse523::CMOVL64rr  },
//        { Cse523::CMOVLE16rr, Cse523::CMOVLE32rr, Cse523::CMOVLE64rr },
//        { Cse523::CMOVNE16rr, Cse523::CMOVNE32rr, Cse523::CMOVNE64rr },
//        { Cse523::CMOVNO16rr, Cse523::CMOVNO32rr, Cse523::CMOVNO64rr },
//        { Cse523::CMOVNP16rr, Cse523::CMOVNP32rr, Cse523::CMOVNP64rr },
//        { Cse523::CMOVNS16rr, Cse523::CMOVNS32rr, Cse523::CMOVNS64rr },
//        { Cse523::CMOVO16rr,  Cse523::CMOVO32rr,  Cse523::CMOVO64rr  },
//        { Cse523::CMOVP16rr,  Cse523::CMOVP32rr,  Cse523::CMOVP64rr  },
//        { Cse523::CMOVS16rr,  Cse523::CMOVS32rr,  Cse523::CMOVS64rr  },
//        { Cse523::CMOVA16rm,  Cse523::CMOVA32rm,  Cse523::CMOVA64rm  },
//        { Cse523::CMOVAE16rm, Cse523::CMOVAE32rm, Cse523::CMOVAE64rm },
//        { Cse523::CMOVB16rm,  Cse523::CMOVB32rm,  Cse523::CMOVB64rm  },
//        { Cse523::CMOVBE16rm, Cse523::CMOVBE32rm, Cse523::CMOVBE64rm },
//        { Cse523::CMOVE16rm,  Cse523::CMOVE32rm,  Cse523::CMOVE64rm  },
//        { Cse523::CMOVG16rm,  Cse523::CMOVG32rm,  Cse523::CMOVG64rm  },
//        { Cse523::CMOVGE16rm, Cse523::CMOVGE32rm, Cse523::CMOVGE64rm },
//        { Cse523::CMOVL16rm,  Cse523::CMOVL32rm,  Cse523::CMOVL64rm  },
//        { Cse523::CMOVLE16rm, Cse523::CMOVLE32rm, Cse523::CMOVLE64rm },
//        { Cse523::CMOVNE16rm, Cse523::CMOVNE32rm, Cse523::CMOVNE64rm },
//        { Cse523::CMOVNO16rm, Cse523::CMOVNO32rm, Cse523::CMOVNO64rm },
//        { Cse523::CMOVNP16rm, Cse523::CMOVNP32rm, Cse523::CMOVNP64rm },
//        { Cse523::CMOVNS16rm, Cse523::CMOVNS32rm, Cse523::CMOVNS64rm },
//        { Cse523::CMOVO16rm,  Cse523::CMOVO32rm,  Cse523::CMOVO64rm  },
//        { Cse523::CMOVP16rm,  Cse523::CMOVP32rm,  Cse523::CMOVP64rm  },
//        { Cse523::CMOVS16rm,  Cse523::CMOVS32rm,  Cse523::CMOVS64rm  }
    };

    assert(CC < 16 && "Can only handle standard cond codes");
    unsigned Idx = HasMemoryOperand ? 16+CC : CC;
    switch(RegBytes) {
        default: llvm_unreachable("Illegal register size!");
        case 2: return Opc[Idx][0];
        case 4: return Opc[Idx][1];
        case 8: return Opc[Idx][2];
    }
}

bool Cse523InstrInfo::isUnpredicatedTerminator(const MachineInstr *MI) const {
    if (!MI->isTerminator()) return false;

    // Conditional branch is a special case.
    if (MI->isBranch() && !MI->isBarrier())
        return true;
    if (!MI->isPredicable())
        return true;
    return !isPredicated(MI);
}

bool Cse523InstrInfo::AnalyzeBranch(MachineBasicBlock &MBB,
        MachineBasicBlock *&TBB,
        MachineBasicBlock *&FBB,
        SmallVectorImpl<MachineOperand> &Cond,
        bool AllowModify) const {
    // Start from the bottom of the block and work up, examining the
    // terminator instructions.
    MachineBasicBlock::iterator I = MBB.end();
    MachineBasicBlock::iterator UnCondBrIter = MBB.end();
    while (I != MBB.begin()) {
        --I;
        if (I->isDebugValue())
            continue;

        // Working from the bottom, when we see a non-terminator instruction, we're
        // done.
        if (!isUnpredicatedTerminator(I))
            break;

        // A terminator that isn't a branch can't easily be handled by this
        // analysis.
        if (!I->isBranch())
            return true;

        // Handle unconditional branches.
        if (I->getOpcode() == Cse523::JMP_4) {
            UnCondBrIter = I;

            if (!AllowModify) {
                TBB = I->getOperand(0).getMBB();
                continue;
            }

            // If the block has any instructions after a JMP, delete them.
            while (llvm::next(I) != MBB.end())
                llvm::next(I)->eraseFromParent();

            Cond.clear();
            FBB = 0;

            // Delete the JMP if it's equivalent to a fall-through.
            if (MBB.isLayoutSuccessor(I->getOperand(0).getMBB())) {
                TBB = 0;
                I->eraseFromParent();
                I = MBB.end();
                UnCondBrIter = MBB.end();
                continue;
            }

            // TBB is used to indicate the unconditional destination.
            TBB = I->getOperand(0).getMBB();
            continue;
        }

        // Handle conditional branches.
        Cse523::CondCode BranchCode = getCondFromBranchOpc(I->getOpcode());
        if (BranchCode == Cse523::COND_INVALID)
            return true;  // Can't handle indirect branch.

        // Working from the bottom, handle the first conditional branch.
        if (Cond.empty()) {
            MachineBasicBlock *TargetBB = I->getOperand(0).getMBB();
            if (AllowModify && UnCondBrIter != MBB.end() &&
                    MBB.isLayoutSuccessor(TargetBB)) {
                // If we can modify the code and it ends in something like:
                //
                //     jCC L1
                //     jmp L2
                //   L1:
                //     ...
                //   L2:
                //
                // Then we can change this to:
                //
                //     jnCC L2
                //   L1:
                //     ...
                //   L2:
                //
                // Which is a bit more efficient.
                // We conditionally jump to the fall-through block.
                BranchCode = GetOppositeBranchCondition(BranchCode);
                unsigned JNCC = GetCondBranchFromCond(BranchCode);
                MachineBasicBlock::iterator OldInst = I;

                BuildMI(MBB, UnCondBrIter, MBB.findDebugLoc(I), get(JNCC))
                    .addMBB(UnCondBrIter->getOperand(0).getMBB());
                BuildMI(MBB, UnCondBrIter, MBB.findDebugLoc(I), get(Cse523::JMP_4))
                    .addMBB(TargetBB);

                OldInst->eraseFromParent();
                UnCondBrIter->eraseFromParent();

                // Restart the analysis.
                UnCondBrIter = MBB.end();
                I = MBB.end();
                continue;
            }

            FBB = TBB;
            TBB = I->getOperand(0).getMBB();
            Cond.push_back(MachineOperand::CreateImm(BranchCode));
            continue;
        }

        // Handle subsequent conditional branches. Only handle the case where all
        // conditional branches branch to the same destination and their condition
        // opcodes fit one of the special multi-branch idioms.
        assert(Cond.size() == 1);
        assert(TBB);

        // Only handle the case where all conditional branches branch to the same
        // destination.
        if (TBB != I->getOperand(0).getMBB())
            return true;

        // If the conditions are the same, we can leave them alone.
        Cse523::CondCode OldBranchCode = (Cse523::CondCode)Cond[0].getImm();
        if (OldBranchCode == BranchCode)
            continue;

        // If they differ, see if they fit one of the known patterns. Theoretically,
        // we could handle more patterns here, but we shouldn't expect to see them
        // if instruction selection has done a reasonable job.
        if ((OldBranchCode == Cse523::COND_NP &&
                    BranchCode == Cse523::COND_E) ||
                (OldBranchCode == Cse523::COND_E &&
                 BranchCode == Cse523::COND_NP))
            BranchCode = Cse523::COND_NP_OR_E;
        else if ((OldBranchCode == Cse523::COND_P &&
                    BranchCode == Cse523::COND_NE) ||
                (OldBranchCode == Cse523::COND_NE &&
                 BranchCode == Cse523::COND_P))
            BranchCode = Cse523::COND_NE_OR_P;
        else
            return true;

        // Update the MachineOperand.
        Cond[0].setImm(BranchCode);
    }

    return false;
}

unsigned Cse523InstrInfo::RemoveBranch(MachineBasicBlock &MBB) const {
    MachineBasicBlock::iterator I = MBB.end();
    unsigned Count = 0;

    while (I != MBB.begin()) {
        --I;
        if (I->isDebugValue())
            continue;
        if (I->getOpcode() != Cse523::JMP_4 &&
                getCondFromBranchOpc(I->getOpcode()) == Cse523::COND_INVALID)
            break;
        // Remove the branch.
        I->eraseFromParent();
        I = MBB.end();
        ++Count;
    }

    return Count;
}

unsigned
Cse523InstrInfo::InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
        MachineBasicBlock *FBB,
        const SmallVectorImpl<MachineOperand> &Cond,
        DebugLoc DL) const {
    // Shouldn't be a fall through.
    assert(TBB && "InsertBranch must not be told to insert a fallthrough");
    assert((Cond.size() == 1 || Cond.size() == 0) &&
            "Cse523 branch conditions have one component!");

    if (Cond.empty()) {
        // Unconditional branch?
        assert(!FBB && "Unconditional branch with multiple successors!");
        BuildMI(&MBB, DL, get(Cse523::JMP_4)).addMBB(TBB);
        return 1;
    }

    // Conditional branch.
    unsigned Count = 0;
    assert(0);
    //Cse523::CondCode CC = (Cse523::CondCode)Cond[0].getImm();
    //switch (CC) {
    //    case Cse523::COND_NP_OR_E:
    //        // Synthesize NP_OR_E with two branches.
    //        BuildMI(&MBB, DL, get(Cse523::JNP_4)).addMBB(TBB);
    //        ++Count;
    //        BuildMI(&MBB, DL, get(Cse523::JE_4)).addMBB(TBB);
    //        ++Count;
    //        break;
    //    case Cse523::COND_NE_OR_P:
    //        // Synthesize NE_OR_P with two branches.
    //        BuildMI(&MBB, DL, get(Cse523::JNE_4)).addMBB(TBB);
    //        ++Count;
    //        BuildMI(&MBB, DL, get(Cse523::JP_4)).addMBB(TBB);
    //        ++Count;
    //        break;
    //    default: {
    //                 unsigned Opc = GetCondBranchFromCond(CC);
    //                 BuildMI(&MBB, DL, get(Opc)).addMBB(TBB);
    //                 ++Count;
    //             }
    //}
    if (FBB) {
        // Two-way Conditional branch. Insert the second branch.
        BuildMI(&MBB, DL, get(Cse523::JMP_4)).addMBB(FBB);
        ++Count;
    }
    return Count;
}

bool Cse523InstrInfo::
canInsertSelect(const MachineBasicBlock &MBB,
        const SmallVectorImpl<MachineOperand> &Cond,
        unsigned TrueReg, unsigned FalseReg,
        int &CondCycles, int &TrueCycles, int &FalseCycles) const {
    // Not all subtargets have cmov instructions.
    if (!TM.getSubtarget<Cse523Subtarget>().hasCMov())
        return false;
    if (Cond.size() != 1)
        return false;
    // We cannot do the composite conditions, at least not in SSA form.
    if ((Cse523::CondCode)Cond[0].getImm() > Cse523::COND_S)
        return false;

    // Check register classes.
    const MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    const TargetRegisterClass *RC =
        RI.getCommonSubClass(MRI.getRegClass(TrueReg), MRI.getRegClass(FalseReg));
    if (!RC)
        return false;

    // We have cmov instructions for 64 bit general purpose registers.
    if (Cse523::GR64RegClass.hasSubClassEq(RC)) {
        // This latency applies to Pentium M, Merom, Wolfdale, Nehalem, and Sandy
        // Bridge. Probably Ivy Bridge as well.
        CondCycles = 2;
        TrueCycles = 2;
        FalseCycles = 2;
        return true;
    }

    // Can't do vectors.
    return false;
}

void Cse523InstrInfo::insertSelect(MachineBasicBlock &MBB,
        MachineBasicBlock::iterator I, DebugLoc DL,
        unsigned DstReg,
        const SmallVectorImpl<MachineOperand> &Cond,
        unsigned TrueReg, unsigned FalseReg) const {
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    assert(Cond.size() == 1 && "Invalid Cond array");
    unsigned Opc = getCMovFromCond((Cse523::CondCode)Cond[0].getImm(),
            MRI.getRegClass(DstReg)->getSize(),
            false/*HasMemoryOperand*/);
    BuildMI(MBB, I, DL, get(Opc), DstReg).addReg(FalseReg).addReg(TrueReg);
}

/// isHReg - Test if the given register is a physical h register.
static bool isHReg(unsigned Reg) {
    return false;
}

inline static bool MaskRegClassContains(unsigned Reg) {
    return false;
}
static
unsigned copyPhysRegOpcode_AVX512(unsigned& DestReg, unsigned& SrcReg) {
    llvm_unreachable("Invalid Register copyPhysReg()");
    return 0;
}

void Cse523InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
        MachineBasicBlock::iterator MI, DebugLoc DL,
        unsigned DestReg, unsigned SrcReg,
        bool KillSrc) const {
    // First deal with the normal symmetric copies.
    unsigned Opc = 0;
    if (Cse523::GR64RegClass.contains(DestReg, SrcReg))
        Opc = Cse523::MOV64rr;
    else
        llvm_unreachable("Invalid Register copyPhysReg()");

    if (Opc) {
        BuildMI(MBB, MI, DL, get(Opc), DestReg)
            .addReg(SrcReg, getKillRegState(KillSrc));
        return;
    }
    // TODO EFLAFS -> EFLAGS code

    DEBUG(dbgs() << "Cannot copy " << RI.getName(SrcReg)
            << " to " << RI.getName(DestReg) << '\n');
    llvm_unreachable("Cannot emit physreg copy instruction");
}

static unsigned getLoadStoreRegOpcode(unsigned Reg,
        const TargetRegisterClass *RC,
        bool isStackAligned,
        const TargetMachine &TM,
        bool load) 
{
    switch (RC->getSize()) {
        default:
            llvm_unreachable("Unknown spill size");
        case 1:
            llvm_unreachable("Unknown 1-byte regclass");
        case 2:
            llvm_unreachable("Unknown 2-byte regclass");
        case 4:
            llvm_unreachable("Unknown 4-byte regclass");
        case 8:
            if (Cse523::GR64RegClass.hasSubClassEq(RC)) {
                return load ? Cse523::MOV64rm : Cse523::MOV64mr;
            } else {
                llvm_unreachable("Unknown 8-byte regclass");
            }
        case 10:
        case 16:
        case 32:
        case 64:
            llvm_unreachable("Unknown spill size >8");
    }
    return 0;
}

static unsigned getStoreRegOpcode(unsigned SrcReg,
        const TargetRegisterClass *RC,
        bool isStackAligned,
        TargetMachine &TM) {
    return getLoadStoreRegOpcode(SrcReg, RC, isStackAligned, TM, false);
}


static unsigned getLoadRegOpcode(unsigned DestReg,
        const TargetRegisterClass *RC,
        bool isStackAligned,
        const TargetMachine &TM) {
    return getLoadStoreRegOpcode(DestReg, RC, isStackAligned, TM, true);
}

void Cse523InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
        MachineBasicBlock::iterator MI,
        unsigned SrcReg, bool isKill, int FrameIdx,
        const TargetRegisterClass *RC,
        const TargetRegisterInfo *TRI) const {
    const MachineFunction &MF = *MBB.getParent();
    assert(MF.getFrameInfo()->getObjectSize(FrameIdx) >= RC->getSize() &&
            "Stack slot too small for store");
    unsigned Alignment = std::max<uint32_t>(RC->getSize(), 16);
    bool isAligned = (TM.getFrameLowering()->getStackAlignment() >= Alignment) ||
        RI.canRealignStack(MF);
    unsigned Opc = getStoreRegOpcode(SrcReg, RC, isAligned, TM);
    DebugLoc DL = MBB.findDebugLoc(MI);
    addFrameReference(BuildMI(MBB, MI, DL, get(Opc)), FrameIdx)
        .addReg(SrcReg, getKillRegState(isKill));
}

void Cse523InstrInfo::storeRegToAddr(MachineFunction &MF, unsigned SrcReg,
        bool isKill,
        SmallVectorImpl<MachineOperand> &Addr,
        const TargetRegisterClass *RC,
        MachineInstr::mmo_iterator MMOBegin,
        MachineInstr::mmo_iterator MMOEnd,
        SmallVectorImpl<MachineInstr*> &NewMIs) const {
    unsigned Alignment = std::max<uint32_t>(RC->getSize(), 16);
    bool isAligned = MMOBegin != MMOEnd &&
        (*MMOBegin)->getAlignment() >= Alignment;
    unsigned Opc = getStoreRegOpcode(SrcReg, RC, isAligned, TM);
    DebugLoc DL;
    MachineInstrBuilder MIB = BuildMI(MF, DL, get(Opc));
    for (unsigned i = 0, e = Addr.size(); i != e; ++i)
        MIB.addOperand(Addr[i]);
    MIB.addReg(SrcReg, getKillRegState(isKill));
    (*MIB).setMemRefs(MMOBegin, MMOEnd);
    NewMIs.push_back(MIB);
}


void Cse523InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
        MachineBasicBlock::iterator MI,
        unsigned DestReg, int FrameIdx,
        const TargetRegisterClass *RC,
        const TargetRegisterInfo *TRI) const {
    const MachineFunction &MF = *MBB.getParent();
    unsigned Alignment = std::max<uint32_t>(RC->getSize(), 16);
    bool isAligned = (TM.getFrameLowering()->getStackAlignment() >= Alignment) ||
        RI.canRealignStack(MF);
    unsigned Opc = getLoadRegOpcode(DestReg, RC, isAligned, TM);
    DebugLoc DL = MBB.findDebugLoc(MI);
    addFrameReference(BuildMI(MBB, MI, DL, get(Opc), DestReg), FrameIdx);
}

void Cse523InstrInfo::loadRegFromAddr(MachineFunction &MF, unsigned DestReg,
        SmallVectorImpl<MachineOperand> &Addr,
        const TargetRegisterClass *RC,
        MachineInstr::mmo_iterator MMOBegin,
        MachineInstr::mmo_iterator MMOEnd,
        SmallVectorImpl<MachineInstr*> &NewMIs) const {
    unsigned Alignment = std::max<uint32_t>(RC->getSize(), 16);
    bool isAligned = MMOBegin != MMOEnd &&
        (*MMOBegin)->getAlignment() >= Alignment;
    unsigned Opc = getLoadRegOpcode(DestReg, RC, isAligned, TM);
    DebugLoc DL;
    MachineInstrBuilder MIB = BuildMI(MF, DL, get(Opc), DestReg);
    for (unsigned i = 0, e = Addr.size(); i != e; ++i)
        MIB.addOperand(Addr[i]);
    (*MIB).setMemRefs(MMOBegin, MMOEnd);
    NewMIs.push_back(MIB);
}

bool Cse523InstrInfo::
analyzeCompare(const MachineInstr *MI, unsigned &SrcReg, unsigned &SrcReg2,
        int &CmpMask, int &CmpValue) const {
    assert(0);
    switch (MI->getOpcode()) {
        default: break;
//        case Cse523::CMP64ri32:
//        case Cse523::CMP64ri8:
//        case Cse523::CMP32ri:
//        case Cse523::CMP32ri8:
//        case Cse523::CMP16ri:
//        case Cse523::CMP16ri8:
//        case Cse523::CMP8ri:
//                 SrcReg = MI->getOperand(0).getReg();
//                 SrcReg2 = 0;
//                 CmpMask = ~0;
//                 CmpValue = MI->getOperand(1).getImm();
//                 return true;
//                 // A SUB can be used to perform comparison.
//        case Cse523::SUB64rm:
//        case Cse523::SUB32rm:
//        case Cse523::SUB16rm:
//        case Cse523::SUB8rm:
//                 SrcReg = MI->getOperand(1).getReg();
//                 SrcReg2 = 0;
//                 CmpMask = ~0;
//                 CmpValue = 0;
//                 return true;
//        case Cse523::SUB64rr:
//        case Cse523::SUB32rr:
//        case Cse523::SUB16rr:
//        case Cse523::SUB8rr:
//                 SrcReg = MI->getOperand(1).getReg();
//                 SrcReg2 = MI->getOperand(2).getReg();
//                 CmpMask = ~0;
//                 CmpValue = 0;
//                 return true;
//        case Cse523::SUB64ri32:
//        case Cse523::SUB64ri8:
//        case Cse523::SUB32ri:
//        case Cse523::SUB32ri8:
//        case Cse523::SUB16ri:
//        case Cse523::SUB16ri8:
//        case Cse523::SUB8ri:
//                 SrcReg = MI->getOperand(1).getReg();
//                 SrcReg2 = 0;
//                 CmpMask = ~0;
//                 CmpValue = MI->getOperand(2).getImm();
//                 return true;
//        case Cse523::CMP64rr:
//        case Cse523::CMP32rr:
//        case Cse523::CMP16rr:
//        case Cse523::CMP8rr:
//                 SrcReg = MI->getOperand(0).getReg();
//                 SrcReg2 = MI->getOperand(1).getReg();
//                 CmpMask = ~0;
//                 CmpValue = 0;
//                 return true;
//        case Cse523::TEST8rr:
//        case Cse523::TEST16rr:
//        case Cse523::TEST32rr:
//        case Cse523::TEST64rr:
//                 SrcReg = MI->getOperand(0).getReg();
//                 if (MI->getOperand(1).getReg() != SrcReg) return false;
//                 // Compare against zero.
//                 SrcReg2 = 0;
//                 CmpMask = ~0;
//                 CmpValue = 0;
//                 return true;
    }
    return false;
}

/// isRedundantFlagInstr - check whether the first instruction, whose only
/// purpose is to update flags, can be made redundant.
/// CMPrr can be made redundant by SUBrr if the operands are the same.
/// This function can be extended later on.
/// SrcReg, SrcRegs: register operands for FlagI.
/// ImmValue: immediate for FlagI if it takes an immediate.
inline static bool isRedundantFlagInstr(MachineInstr *FlagI, unsigned SrcReg,
        unsigned SrcReg2, int ImmValue,
        MachineInstr *OI) {

    assert(0);
//    if (((FlagI->getOpcode() == Cse523::CMP64rr &&
//                    OI->getOpcode() == Cse523::SUB64rr) ||
//                (FlagI->getOpcode() == Cse523::CMP32rr &&
//                 OI->getOpcode() == Cse523::SUB32rr)||
//                (FlagI->getOpcode() == Cse523::CMP16rr &&
//                 OI->getOpcode() == Cse523::SUB16rr)||
//                (FlagI->getOpcode() == Cse523::CMP8rr &&
//                 OI->getOpcode() == Cse523::SUB8rr)) &&
//            ((OI->getOperand(1).getReg() == SrcReg &&
//              OI->getOperand(2).getReg() == SrcReg2) ||
//             (OI->getOperand(1).getReg() == SrcReg2 &&
//              OI->getOperand(2).getReg() == SrcReg)))
//        return true;
//
//    if (((FlagI->getOpcode() == Cse523::CMP64ri32 &&
//                    OI->getOpcode() == Cse523::SUB64ri32) ||
//                (FlagI->getOpcode() == Cse523::CMP64ri8 &&
//                 OI->getOpcode() == Cse523::SUB64ri8) ||
//                (FlagI->getOpcode() == Cse523::CMP32ri &&
//                 OI->getOpcode() == Cse523::SUB32ri) ||
//                (FlagI->getOpcode() == Cse523::CMP32ri8 &&
//                 OI->getOpcode() == Cse523::SUB32ri8) ||
//                (FlagI->getOpcode() == Cse523::CMP16ri &&
//                 OI->getOpcode() == Cse523::SUB16ri) ||
//                (FlagI->getOpcode() == Cse523::CMP16ri8 &&
//                 OI->getOpcode() == Cse523::SUB16ri8) ||
//                (FlagI->getOpcode() == Cse523::CMP8ri &&
//                 OI->getOpcode() == Cse523::SUB8ri)) &&
//            OI->getOperand(1).getReg() == SrcReg &&
//            OI->getOperand(2).getImm() == ImmValue)
//        return true;
    return false;
}

/// isDefConvertible - check whether the definition can be converted
/// to remove a comparison against zero.
inline static bool isDefConvertible(MachineInstr *MI) {
    assert(0);
    switch (MI->getOpcode()) {
        default: return false;

        // The shift instructions only modify ZF if their shift count is non-zero.
        // N.B.: The processor truncates the shift count depending on the encoding.
//        case Cse523::SAR8ri:    case Cse523::SAR16ri:  case Cse523::SAR32ri:case Cse523::SAR64ri:
//        case Cse523::SHR8ri:    case Cse523::SHR16ri:  case Cse523::SHR32ri:case Cse523::SHR64ri:
//             return getTruncatedShiftCount(MI, 2) != 0;
//
//                 // Some left shift instructions can be turned into LEA instructions but only
//                 // if their flags aren't used. Avoid transforming such instructions.
//        case Cse523::SHL8ri:    case Cse523::SHL16ri:  case Cse523::SHL32ri:case Cse523::SHL64ri:
//        {
//            unsigned ShAmt = getTruncatedShiftCount(MI, 2);
//            if (isTruncatedShiftCountForLEA(ShAmt)) return false;
//            return ShAmt != 0;
//        }
//
//        case Cse523::SHRD16rri8:case Cse523::SHRD32rri8:case Cse523::SHRD64rri8:
//        case Cse523::SHLD16rri8:case Cse523::SHLD32rri8:case Cse523::SHLD64rri8:
//            return getTruncatedShiftCount(MI, 3) != 0;
//
//        case Cse523::SUB64ri32: case Cse523::SUB64ri8: case Cse523::SUB32ri:
//        case Cse523::SUB32ri8:  case Cse523::SUB16ri:  case Cse523::SUB16ri8:
//        case Cse523::SUB8ri:    case Cse523::SUB64rr:  case Cse523::SUB32rr:
//        case Cse523::SUB16rr:   case Cse523::SUB8rr:   case Cse523::SUB64rm:
//        case Cse523::SUB32rm:   case Cse523::SUB16rm:  case Cse523::SUB8rm:
//        case Cse523::DEC64r:    case Cse523::DEC32r:   case Cse523::DEC16r: case Cse523::DEC8r:
//        case Cse523::DEC64_32r: case Cse523::DEC64_16r:
//        case Cse523::ADD64ri32: case Cse523::ADD64ri8: case Cse523::ADD32ri:
//        case Cse523::ADD32ri8:  case Cse523::ADD16ri:  case Cse523::ADD16ri8:
//        case Cse523::ADD8ri:    case Cse523::ADD64rr:  case Cse523::ADD32rr:
//        case Cse523::ADD16rr:   case Cse523::ADD8rr:   case Cse523::ADD64rm:
//        case Cse523::ADD32rm:   case Cse523::ADD16rm:  case Cse523::ADD8rm:
//        case Cse523::INC64r:    case Cse523::INC32r:   case Cse523::INC16r: case Cse523::INC8r:
//        case Cse523::INC64_32r: case Cse523::INC64_16r:
//        case Cse523::AND64ri32: case Cse523::AND64ri8: case Cse523::AND32ri:
//        case Cse523::AND32ri8:  case Cse523::AND16ri:  case Cse523::AND16ri8:
//        case Cse523::AND8ri:    case Cse523::AND64rr:  case Cse523::AND32rr:
//        case Cse523::AND16rr:   case Cse523::AND8rr:   case Cse523::AND64rm:
//        case Cse523::AND32rm:   case Cse523::AND16rm:  case Cse523::AND8rm:
//        case Cse523::XOR64ri32: case Cse523::XOR64ri8: case Cse523::XOR32ri:
//        case Cse523::XOR32ri8:  case Cse523::XOR16ri:  case Cse523::XOR16ri8:
//        case Cse523::XOR8ri:    case Cse523::XOR64rr:  case Cse523::XOR32rr:
//        case Cse523::XOR16rr:   case Cse523::XOR8rr:   case Cse523::XOR64rm:
//        case Cse523::XOR32rm:   case Cse523::XOR16rm:  case Cse523::XOR8rm:
//        case Cse523::OR64ri32:  case Cse523::OR64ri8:  case Cse523::OR32ri:
//        case Cse523::OR32ri8:   case Cse523::OR16ri:   case Cse523::OR16ri8:
//        case Cse523::OR8ri:     case Cse523::OR64rr:   case Cse523::OR32rr:
//        case Cse523::OR16rr:    case Cse523::OR8rr:    case Cse523::OR64rm:
//        case Cse523::OR32rm:    case Cse523::OR16rm:   case Cse523::OR8rm:
//        case Cse523::NEG8r:     case Cse523::NEG16r:   case Cse523::NEG32r: case Cse523::NEG64r:
//        case Cse523::SAR8r1:    case Cse523::SAR16r1:  case Cse523::SAR32r1:case Cse523::SAR64r1:
//        case Cse523::SHR8r1:    case Cse523::SHR16r1:  case Cse523::SHR32r1:case Cse523::SHR64r1:
//        case Cse523::SHL8r1:    case Cse523::SHL16r1:  case Cse523::SHL32r1:case Cse523::SHL64r1:
//        case Cse523::ADC32ri:   case Cse523::ADC32ri8:
//        case Cse523::ADC32rr:   case Cse523::ADC64ri32:
//        case Cse523::ADC64ri8:  case Cse523::ADC64rr:
//        case Cse523::SBB32ri:   case Cse523::SBB32ri8:
//        case Cse523::SBB32rr:   case Cse523::SBB64ri32:
//        case Cse523::SBB64ri8:  case Cse523::SBB64rr:
//        case Cse523::ANDN32rr:  case Cse523::ANDN32rm:
//        case Cse523::ANDN64rr:  case Cse523::ANDN64rm:
//        case Cse523::BEXTR32rr: case Cse523::BEXTR64rr:
//        case Cse523::BEXTR32rm: case Cse523::BEXTR64rm:
//        case Cse523::BLSI32rr:  case Cse523::BLSI32rm:
//        case Cse523::BLSI64rr:  case Cse523::BLSI64rm:
//        case Cse523::BLSMSK32rr:case Cse523::BLSMSK32rm:
//        case Cse523::BLSMSK64rr:case Cse523::BLSMSK64rm:
//        case Cse523::BLSR32rr:  case Cse523::BLSR32rm:
//        case Cse523::BLSR64rr:  case Cse523::BLSR64rm:
//        case Cse523::BZHI32rr:  case Cse523::BZHI32rm:
//        case Cse523::BZHI64rr:  case Cse523::BZHI64rm:
//        case Cse523::LZCNT16rr: case Cse523::LZCNT16rm:
//        case Cse523::LZCNT32rr: case Cse523::LZCNT32rm:
//        case Cse523::LZCNT64rr: case Cse523::LZCNT64rm:
//        case Cse523::POPCNT16rr:case Cse523::POPCNT16rm:
//        case Cse523::POPCNT32rr:case Cse523::POPCNT32rm:
//        case Cse523::POPCNT64rr:case Cse523::POPCNT64rm:
//        case Cse523::TZCNT16rr: case Cse523::TZCNT16rm:
//        case Cse523::TZCNT32rr: case Cse523::TZCNT32rm:
//        case Cse523::TZCNT64rr: case Cse523::TZCNT64rm:
//            return true;
    }
}

/// optimizeCompareInstr - Check if there exists an earlier instruction that
/// operates on the same source operands and sets flags in the same way as
/// Compare; remove Compare if possible.
bool Cse523InstrInfo::
optimizeCompareInstr(MachineInstr *CmpInstr, unsigned SrcReg, unsigned SrcReg2,
        int CmpMask, int CmpValue,
        const MachineRegisterInfo *MRI) const {
    assert(0);
    // Check whether we can replace SUB with CMP.
//    unsigned NewOpcode = 0;
//    switch (CmpInstr->getOpcode()) {
//        default: break;
//        case Cse523::SUB64ri32:
//        case Cse523::SUB64ri8:
//        case Cse523::SUB32ri:
//        case Cse523::SUB32ri8:
//        case Cse523::SUB16ri:
//        case Cse523::SUB16ri8:
//        case Cse523::SUB8ri:
//        case Cse523::SUB64rm:
//        case Cse523::SUB32rm:
//        case Cse523::SUB16rm:
//        case Cse523::SUB8rm:
//        case Cse523::SUB64rr:
//        case Cse523::SUB32rr:
//        case Cse523::SUB16rr:
//        case Cse523::SUB8rr: 
//        {
//            if (!MRI->use_nodbg_empty(CmpInstr->getOperand(0).getReg()))
//                return false;
//            // There is no use of the destination register, we can replace SUB with CMP.
//            switch (CmpInstr->getOpcode()) {
//                default: llvm_unreachable("Unreachable!");
//                case Cse523::SUB64rm:   NewOpcode = Cse523::CMP64rm;   break;
//                case Cse523::SUB32rm:   NewOpcode = Cse523::CMP32rm;   break;
//                case Cse523::SUB16rm:   NewOpcode = Cse523::CMP16rm;   break;
//                case Cse523::SUB8rm:    NewOpcode = Cse523::CMP8rm;    break;
//                case Cse523::SUB64rr:   NewOpcode = Cse523::CMP64rr;   break;
//                case Cse523::SUB32rr:   NewOpcode = Cse523::CMP32rr;   break;
//                case Cse523::SUB16rr:   NewOpcode = Cse523::CMP16rr;   break;
//                case Cse523::SUB8rr:    NewOpcode = Cse523::CMP8rr;    break;
//                case Cse523::SUB64ri32: NewOpcode = Cse523::CMP64ri32; break;
//                case Cse523::SUB64ri8:  NewOpcode = Cse523::CMP64ri8;  break;
//                case Cse523::SUB32ri:   NewOpcode = Cse523::CMP32ri;   break;
//                case Cse523::SUB32ri8:  NewOpcode = Cse523::CMP32ri8;  break;
//                case Cse523::SUB16ri:   NewOpcode = Cse523::CMP16ri;   break;
//                case Cse523::SUB16ri8:  NewOpcode = Cse523::CMP16ri8;  break;
//                case Cse523::SUB8ri:    NewOpcode = Cse523::CMP8ri;    break;
//            }
//            CmpInstr->setDesc(get(NewOpcode));
//            CmpInstr->RemoveOperand(0);
//            // Fall through to optimize Cmp if Cmp is CMPrr or CMPri.
//            if (NewOpcode == Cse523::CMP64rm || NewOpcode == Cse523::CMP32rm ||
//                    NewOpcode == Cse523::CMP16rm || NewOpcode == Cse523::CMP8rm)
//                return false;
//        }
//    }
//
//    // Get the unique definition of SrcReg.
//    machineInstr *MI = MRI->getUniqueVRegDef(SrcReg);
//    if (!MI) return false;
//
//    // CmpInstr is the first instruction of the BB.
//    machineBasicBlock::iterator I = CmpInstr, Def = MI;
//
//    // If we are comparing against zero, check whether we can use MI to update
//    // EFLAGS. If MI is not in the same BB as CmpInstr, do not optimize.
//    bool IsCmpZero = (SrcReg2 == 0 && CmpValue == 0);
//    if (IsCmpZero && (MI->getParent() != CmpInstr->getParent() ||
//                !isDefConvertible(MI)))
//        return false;
//
//    // We are searching for an earlier instruction that can make CmpInstr
//    // redundant and that instruction will be saved in Sub.
//    machineInstr *Sub = NULL;
//    const TargetRegisterInfo *TRI = &getRegisterInfo();
//
//    // We iterate backward, starting from the instruction before CmpInstr and
//    // stop when reaching the definition of a source register or done with the BB.
//    // RI points to the instruction before CmpInstr.
//    // If the definition is in this basic block, RE points to the definition;
//    // otherwise, RE is the rend of the basic block.
//    machineBasicBlock::reverse_iterator
//        RI = MachineBasicBlock::reverse_iterator(I),
//           RE = CmpInstr->getParent() == MI->getParent() ?
//               MachineBasicBlock::reverse_iterator(++Def) /* points to MI */ :
//               CmpInstr->getParent()->rend();
//    machineInstr *Movr0Inst = 0;
//    for (; RI != RE; ++RI) {
//        MachineInstr *Instr = &*RI;
//        // Check whether CmpInstr can be made redundant by the current instruction.
//        if (!IsCmpZero &&
//                isRedundantFlagInstr(CmpInstr, SrcReg, SrcReg2, CmpValue, Instr)) {
//            Sub = Instr;
//            break;
//        }
//
//        if (Instr->modifiesRegister(Cse523::EFLAGS, TRI) ||
//                Instr->readsRegister(Cse523::EFLAGS, TRI)) {
//            // This instruction modifies or uses EFLAGS.
//
//            // MOV32r0 etc. are implemented with xor which clobbers condition code.
//            // They are safe to move up, if the definition to EFLAGS is dead and
//            // earlier instructions do not read or write EFLAGS.
//            if (!Movr0Inst && Instr->getOpcode() == Cse523::MOV32r0 &&
//                    Instr->registerDefIsDead(Cse523::EFLAGS, TRI)) {
//                Movr0Inst = Instr;
//                continue;
//            }
//
//            // We can't remove CmpInstr.
//            return false;
//        }
//    }
//
//    // Return false if no candidates exist.
//    if (!IsCmpZero && !Sub)
//        return false;
//
//    bool IsSwapped = (SrcReg2 != 0 && Sub->getOperand(1).getReg() == SrcReg2 &&
//            Sub->getOperand(2).getReg() == SrcReg);
//
//    // Scan forward from the instruction after CmpInstr for uses of EFLAGS.
//    // It is safe to remove CmpInstr if EFLAGS is redefined or killed.
//    // If we are done with the basic block, we need to check whether EFLAGS is
//    // live-out.
//    bool IsSafe = false;
//    smallVector<std::pair<MachineInstr*, unsigned /*NewOpc*/>, 4> OpsToUpdate;
//    machineBasicBlock::iterator E = CmpInstr->getParent()->end();
//    for (++I; I != E; ++I) {
//        const MachineInstr &Instr = *I;
//        bool ModifyEFLAGS = Instr.modifiesRegister(Cse523::EFLAGS, TRI);
//        bool UseEFLAGS = Instr.readsRegister(Cse523::EFLAGS, TRI);
//        // We should check the usage if this instruction uses and updates EFLAGS.
//        if (!UseEFLAGS && ModifyEFLAGS) {
//            // It is safe to remove CmpInstr if EFLAGS is updated again.
//            IsSafe = true;
//            break;
//        }
//        if (!UseEFLAGS && !ModifyEFLAGS)
//            continue;
//
//        // EFLAGS is used by this instruction.
//        Cse523::CondCode OldCC;
//        bool OpcIsSET = false;
//        if (IsCmpZero || IsSwapped) {
//            // We decode the condition code from opcode.
//            if (Instr.isBranch())
//                OldCC = getCondFromBranchOpc(Instr.getOpcode());
//            else {
//                OldCC = getCondFromSETOpc(Instr.getOpcode());
//                if (OldCC != Cse523::COND_INVALID)
//                    OpcIsSET = true;
//                else
//                    OldCC = Cse523::getCondFromCMovOpc(Instr.getOpcode());
//            }
//            if (OldCC == Cse523::COND_INVALID) return false;
//        }
//        if (IsCmpZero) {
//            switch (OldCC) {
//                default: break;
//                case Cse523::COND_A: case Cse523::COND_AE:
//                case Cse523::COND_B: case Cse523::COND_BE:
//                case Cse523::COND_G: case Cse523::COND_GE:
//                case Cse523::COND_L: case Cse523::COND_LE:
//                case Cse523::COND_O: case Cse523::COND_NO:
//                         // CF and OF are used, we can't perform this optimization.
//                         return false;
//            }
//        } else if (IsSwapped) {
//            // If we have SUB(r1, r2) and CMP(r2, r1), the condition code needs
//            // to be changed from r2 > r1 to r1 < r2, from r2 < r1 to r1 > r2, etc.
//            // We swap the condition code and synthesize the new opcode.
//            Cse523::CondCode NewCC = getSwappedCondition(OldCC);
//            if (NewCC == Cse523::COND_INVALID) return false;
//
//            // Synthesize the new opcode.
//            bool HasMemoryOperand = Instr.hasOneMemOperand();
//            unsigned NewOpc;
//            if (Instr.isBranch())
//                NewOpc = GetCondBranchFromCond(NewCC);
//            else if(OpcIsSET)
//                NewOpc = getSETFromCond(NewCC, HasMemoryOperand);
//            else {
//                unsigned DstReg = Instr.getOperand(0).getReg();
//                NewOpc = getCMovFromCond(NewCC, MRI->getRegClass(DstReg)->getSize(),
//                        HasMemoryOperand);
//            }
//
//            // Push the MachineInstr to OpsToUpdate.
//            // If it is safe to remove CmpInstr, the condition code of these
//            // instructions will be modified.
//            OpsToUpdate.push_back(std::make_pair(&*I, NewOpc));
//        }
//        if (ModifyEFLAGS || Instr.killsRegister(Cse523::EFLAGS, TRI)) {
//            // It is safe to remove CmpInstr if EFLAGS is updated again or killed.
//            IsSafe = true;
//            break;
//        }
//    }
//
//    // If EFLAGS is not killed nor re-defined, we should check whether it is
//    // live-out. If it is live-out, do not optimize.
//    if ((IsCmpZero || IsSwapped) && !IsSafe) {
//        MachineBasicBlock *MBB = CmpInstr->getParent();
//        for (MachineBasicBlock::succ_iterator SI = MBB->succ_begin(),
//                SE = MBB->succ_end(); SI != SE; ++SI)
//            if ((*SI)->isLiveIn(Cse523::EFLAGS))
//                return false;
//    }
//
//    // The instruction to be updated is either Sub or MI.
//    sub = IsCmpZero ? MI : Sub;
//    // Move Movr0Inst to the appropriate place before Sub.
//    if (Movr0Inst) {
//        // Look backwards until we find a def that doesn't use the current EFLAGS.
//        Def = Sub;
//        MachineBasicBlock::reverse_iterator
//            InsertI = MachineBasicBlock::reverse_iterator(++Def),
//                    InsertE = Sub->getParent()->rend();
//        for (; InsertI != InsertE; ++InsertI) {
//            MachineInstr *Instr = &*InsertI;
//            if (!Instr->readsRegister(Cse523::EFLAGS, TRI) &&
//                    Instr->modifiesRegister(Cse523::EFLAGS, TRI)) {
//                Sub->getParent()->remove(Movr0Inst);
//                Instr->getParent()->insert(MachineBasicBlock::iterator(Instr),
//                        Movr0Inst);
//                break;
//            }
//        }
//        if (InsertI == InsertE)
//            return false;
//    }
//
//    // Make sure Sub instruction defines EFLAGS and mark the def live.
//    unsigned i = 0, e = Sub->getNumOperands();
//    for (; i != e; ++i) {
//        MachineOperand &MO = Sub->getOperand(i);
//        if (MO.isReg() && MO.isDef() && MO.getReg() == Cse523::EFLAGS) {
//            MO.setIsDead(false);
//            break;
//        }
//    }
//    assert(i != e && "Unable to locate a def EFLAGS operand");
//
//    CmpInstr->eraseFromParent();
//
//    // Modify the condition code of instructions in OpsToUpdate.
//    for (unsigned i = 0, e = OpsToUpdate.size(); i < e; i++)
//        OpsToUpdate[i].first->setDesc(get(OpsToUpdate[i].second));
    return true;
}

/// optimizeLoadInstr - Try to remove the load by folding it to a register
/// operand at the use. We fold the load instructions if load defines a virtual
/// register, the virtual register is used once in the same BB, and the
/// instructions in-between do not load or store, and have no side effects.
MachineInstr* Cse523InstrInfo::
optimizeLoadInstr(MachineInstr *MI, const MachineRegisterInfo *MRI,
        unsigned &FoldAsLoadDefReg,
        MachineInstr *&DefMI) const {
    if (FoldAsLoadDefReg == 0)
        return 0;
    // To be conservative, if there exists another load, clear the load candidate.
    if (MI->mayLoad()) {
        FoldAsLoadDefReg = 0;
        return 0;
    }

    // Check whether we can move DefMI here.
    DefMI = MRI->getVRegDef(FoldAsLoadDefReg);
    assert(DefMI);
    bool SawStore = false;
    if (!DefMI->isSafeToMove(this, 0, SawStore))
        return 0;

    // We try to commute MI if possible.
    unsigned IdxEnd = (MI->isCommutable()) ? 2 : 1;
    for (unsigned Idx = 0; Idx < IdxEnd; Idx++) {
        // Collect information about virtual register operands of MI.
        unsigned SrcOperandId = 0;
        bool FoundSrcOperand = false;
        for (unsigned i = 0, e = MI->getDesc().getNumOperands(); i != e; ++i) {
            MachineOperand &MO = MI->getOperand(i);
            if (!MO.isReg())
                continue;
            unsigned Reg = MO.getReg();
            if (Reg != FoldAsLoadDefReg)
                continue;
            // Do not fold if we have a subreg use or a def or multiple uses.
            if (MO.getSubReg() || MO.isDef() || FoundSrcOperand)
                return 0;

            SrcOperandId = i;
            FoundSrcOperand = true;
        }
        if (!FoundSrcOperand) return 0;

        // Check whether we can fold the def into SrcOperandId.
        SmallVector<unsigned, 8> Ops;
        Ops.push_back(SrcOperandId);
        MachineInstr *FoldMI = foldMemoryOperand(MI, Ops, DefMI);
        if (FoldMI) {
            FoldAsLoadDefReg = 0;
            return FoldMI;
        }

        if (Idx == 1) {
            // MI was changed but it didn't help, commute it back!
            commuteInstruction(MI, false);
            return 0;
        }

        // Check whether we can commute MI and enable folding.
        if (MI->isCommutable()) {
            MachineInstr *NewMI = commuteInstruction(MI, false);
            // Unable to commute.
            if (!NewMI) return 0;
            if (NewMI != MI) {
                // New instruction. It doesn't need to be kept.
                NewMI->eraseFromParent();
                return 0;
            }
        }
    }
    return 0;
}

/// Expand2AddrUndef - Expand a single-def pseudo instruction to a two-addr
/// instruction with two undef reads of the register being defined.  This is
/// used for mapping:
///   %xmm4 = V_SET0
/// to:
///   %xmm4 = PXORrr %xmm4<undef>, %xmm4<undef>
///
static bool Expand2AddrUndef(MachineInstrBuilder &MIB,
        const MCInstrDesc &Desc) {
    assert(Desc.getNumOperands() == 3 && "Expected two-addr instruction.");
    unsigned Reg = MIB->getOperand(0).getReg();
    MIB->setDesc(Desc);

    // MachineInstr::addOperand() will insert explicit operands before any
    // implicit operands.
    MIB.addReg(Reg, RegState::Undef).addReg(Reg, RegState::Undef);
    // But we don't trust that.
    assert(MIB->getOperand(1).getReg() == Reg &&
            MIB->getOperand(2).getReg() == Reg && "Misplaced operand");
    return true;
}

bool Cse523InstrInfo::expandPostRAPseudo(MachineBasicBlock::iterator MI) const {
    MachineInstrBuilder MIB(*MI->getParent()->getParent(), MI);
    switch (MI->getOpcode()) {
        default: break;
        case Cse523::MOV64r0:
            return Expand2AddrUndef(MIB, get(Cse523::XOR64rr));
//        case Cse523::SETB_C8r:
//            return Expand2AddrUndef(MIB, get(Cse523::SBB8rr));
//        case Cse523::SETB_C16r:
//            return Expand2AddrUndef(MIB, get(Cse523::SBB16rr));
//        case Cse523::SETB_C32r:
//            return Expand2AddrUndef(MIB, get(Cse523::SBB32rr));
//        case Cse523::SETB_C64r:
//            return Expand2AddrUndef(MIB, get(Cse523::SBB64rr));
//        case Cse523::TEST8ri_NOREX:
//            MI->setDesc(get(Cse523::TEST8ri));
//            return true;
//        case Cse523::KSET0W: return Expand2AddrUndef(MIB, get(Cse523::KXORWrr));
//        case Cse523::KSET1B:
//        case Cse523::KSET1W: return Expand2AddrUndef(MIB, get(Cse523::KXNORWrr));
    }
    return false;
}

static MachineInstr *FuseTwoAddrInst(MachineFunction &MF, unsigned Opcode,
        const SmallVectorImpl<MachineOperand> &MOs,
        MachineInstr *MI,
        const TargetInstrInfo &TII) {
    // Create the base instruction with the memory operand as the first part.
    // Omit the implicit operands, something BuildMI can't do.
    MachineInstr *NewMI = MF.CreateMachineInstr(TII.get(Opcode),
            MI->getDebugLoc(), true);
    MachineInstrBuilder MIB(MF, NewMI);
    unsigned NumAddrOps = MOs.size();
    for (unsigned i = 0; i != NumAddrOps; ++i)
        MIB.addOperand(MOs[i]);
    if (NumAddrOps < 4)  // FrameIndex only
        addOffset(MIB, 0);

    // Loop over the rest of the ri operands, converting them over.
    unsigned NumOps = MI->getDesc().getNumOperands()-2;
    for (unsigned i = 0; i != NumOps; ++i) {
        MachineOperand &MO = MI->getOperand(i+2);
        MIB.addOperand(MO);
    }
    for (unsigned i = NumOps+2, e = MI->getNumOperands(); i != e; ++i) {
        MachineOperand &MO = MI->getOperand(i);
        MIB.addOperand(MO);
    }
    return MIB;
}

static MachineInstr *FuseInst(MachineFunction &MF,
        unsigned Opcode, unsigned OpNo,
        const SmallVectorImpl<MachineOperand> &MOs,
        MachineInstr *MI, const TargetInstrInfo &TII) {
    // Omit the implicit operands, something BuildMI can't do.
    MachineInstr *NewMI = MF.CreateMachineInstr(TII.get(Opcode),
            MI->getDebugLoc(), true);
    MachineInstrBuilder MIB(MF, NewMI);

    for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
        MachineOperand &MO = MI->getOperand(i);
        if (i == OpNo) {
            assert(MO.isReg() && "Expected to fold into reg operand!");
            unsigned NumAddrOps = MOs.size();
            for (unsigned i = 0; i != NumAddrOps; ++i)
                MIB.addOperand(MOs[i]);
            if (NumAddrOps < 4)  // FrameIndex only
                addOffset(MIB, 0);
        } else {
            MIB.addOperand(MO);
        }
    }
    return MIB;
}

static MachineInstr *MakeM0Inst(const TargetInstrInfo &TII, unsigned Opcode,
        const SmallVectorImpl<MachineOperand> &MOs,
        MachineInstr *MI) {
    MachineFunction &MF = *MI->getParent()->getParent();
    MachineInstrBuilder MIB = BuildMI(MF, MI->getDebugLoc(), TII.get(Opcode));

    unsigned NumAddrOps = MOs.size();
    for (unsigned i = 0; i != NumAddrOps; ++i)
        MIB.addOperand(MOs[i]);
    if (NumAddrOps < 4)  // FrameIndex only
        addOffset(MIB, 0);
    return MIB.addImm(0);
}

MachineInstr*
Cse523InstrInfo::foldMemoryOperandImpl(MachineFunction &MF,
        MachineInstr *MI, unsigned i,
        const SmallVectorImpl<MachineOperand> &MOs,
        unsigned Size, unsigned Align) const {
    const DenseMap<unsigned, std::pair<unsigned,unsigned> > *OpcodeTablePtr = 0;
    bool isCallRegIndirect = TM.getSubtarget<Cse523Subtarget>().callRegIndirect();
    bool isTwoAddrFold = false;

    // Atom favors register form of call. So, we do not fold loads into calls
    // when Cse523Subtarget is Atom.
    if (isCallRegIndirect && (MI->getOpcode() == Cse523::CALL64r)) {
        return NULL;
    }

    unsigned NumOps = MI->getDesc().getNumOperands();
    bool isTwoAddr = NumOps > 1 &&
        MI->getDesc().getOperandConstraint(1, MCOI::TIED_TO) != -1;

    // FIXME: AsmPrinter doesn't know how to handle
    // Cse523II::MO_GOT_ABSOLUTE_ADDRESS after folding.
    if (MI->getOpcode() == Cse523::ADD64ri32 &&
            MI->getOperand(2).getTargetFlags() == Cse523II::MO_GOT_ABSOLUTE_ADDRESS)
        return NULL;

    MachineInstr *NewMI = NULL;
    // Folding a memory location into the two-address part of a two-address
    // instruction is different than folding it other places.  It requires
    // replacing the *two* registers with the memory location.
    if (isTwoAddr && NumOps >= 2 && i < 2 &&
            MI->getOperand(0).isReg() &&
            MI->getOperand(1).isReg() &&
            MI->getOperand(0).getReg() == MI->getOperand(1).getReg()) {
        OpcodeTablePtr = &RegOp2MemOpTable2Addr;
        isTwoAddrFold = true;
    } else if (i == 0) { // If operand 0
        if (MI->getOpcode() == Cse523::MOV64r0) {
            NewMI = MakeM0Inst(*this, Cse523::MOV64mi32, MOs, MI);
            if (NewMI)
                return NewMI;
        }

        OpcodeTablePtr = &RegOp2MemOpTable0;
    } else if (i == 1) {
        OpcodeTablePtr = &RegOp2MemOpTable1;
    } else if (i == 2) {
        OpcodeTablePtr = &RegOp2MemOpTable2;
    } else if (i == 3) {
        OpcodeTablePtr = &RegOp2MemOpTable3;
    }

    // If table selected...
    if (OpcodeTablePtr) {
        // Find the Opcode to fuse
        DenseMap<unsigned, std::pair<unsigned,unsigned> >::const_iterator I =
            OpcodeTablePtr->find(MI->getOpcode());
        if (I != OpcodeTablePtr->end()) {
            unsigned Opcode = I->second.first;
            unsigned MinAlign = (I->second.second & TB_ALIGN_MASK) >> TB_ALIGN_SHIFT;
            if (Align < MinAlign)
                return NULL;
            bool NarrowToMOV32rm = false;
            if (Size) {
                unsigned RCSize = getRegClass(MI->getDesc(), i, &RI, MF)->getSize();
                if (Size < RCSize) {
                    // Check if it's safe to fold the load. If the size of the object is
                    // narrower than the load width, then it's not.
                    if (Opcode != Cse523::MOV64rm || RCSize != 8 || Size != 4)
                        return NULL;
                    // If this is a 64-bit load, but the spill slot is 32, then we can do
                    // a 32-bit load which is implicitly zero-extended. This likely is due
                    // to liveintervalanalysis remat'ing a load from stack slot.
                    if (MI->getOperand(0).getSubReg() || MI->getOperand(1).getSubReg())
                        return NULL;
                    assert(0);
                    //Opcode = Cse523::MOV32rm;
                    //NarrowToMOV32rm = true;
                }
            }

            if (isTwoAddrFold)
                NewMI = FuseTwoAddrInst(MF, Opcode, MOs, MI, *this);
            else
                NewMI = FuseInst(MF, Opcode, i, MOs, MI, *this);

            if (NarrowToMOV32rm) {
                // If this is the special case where we use a MOV32rm to load a 32-bit
                // value and zero-extend the top bits. Change the destination register
                // to a 32-bit one.
                assert(0);
                //unsigned DstReg = NewMI->getOperand(0).getReg();
                //if (TargetRegisterInfo::isPhysicalRegister(DstReg))
                //    NewMI->getOperand(0).setReg(RI.getSubReg(DstReg,
                //                Cse523::sub_32bit));
                //else
                //    NewMI->getOperand(0).setSubReg(Cse523::sub_32bit);
            }
            return NewMI;
        }
    }

    // No fusion
    if (PrintFailedFusing && !MI->isCopy())
        dbgs() << "We failed to fuse operand " << i << " in " << *MI;
    return NULL;
}

/// hasPartialRegUpdate - Return true for all instructions that only update
/// the first 32 or 64-bits of the destination register and leave the rest
/// unmodified. This can be used to avoid folding loads if the instructions
/// only update part of the destination register, and the non-updated part is
/// not needed. e.g. cvtss2sd, sqrtss. Unfolding the load from these
/// instructions breaks the partial register dependency and it can improve
/// performance. e.g.:
///
///   movss (%rdi), %xmm0
///   cvtss2sd %xmm0, %xmm0
///
/// Instead of
///   cvtss2sd (%rdi), %xmm0
///
/// FIXME: This should be turned into a TSFlags.
///
static bool hasPartialRegUpdate(unsigned Opcode) {
    switch (Opcode) {
        default: break;
// TODO:
//        case Cse523::CVTSI2SSrr:
//        case Cse523::CVTSI2SS64rr:
//        case Cse523::CVTSI2SDrr:
//        case Cse523::CVTSI2SD64rr:
//        case Cse523::CVTSD2SSrr:
//        case Cse523::Int_CVTSD2SSrr:
//        case Cse523::CVTSS2SDrr:
//        case Cse523::Int_CVTSS2SDrr:
//        case Cse523::RCPSSr:
//        case Cse523::RCPSSr_Int:
//        case Cse523::ROUNDSDr:
//        case Cse523::ROUNDSDr_Int:
//        case Cse523::ROUNDSSr:
//        case Cse523::ROUNDSSr_Int:
//        case Cse523::RSQRTSSr:
//        case Cse523::RSQRTSSr_Int:
//        case Cse523::SQRTSSr:
//        case Cse523::SQRTSSr_Int:
//            return true;
    }

    return false;
}

/// getPartialRegUpdateClearance - Inform the ExeDepsFix pass how many idle
/// instructions we would like before a partial register update.
unsigned Cse523InstrInfo::
getPartialRegUpdateClearance(const MachineInstr *MI, unsigned OpNum,
        const TargetRegisterInfo *TRI) const {
    if (OpNum != 0 || !hasPartialRegUpdate(MI->getOpcode()))
        return 0;

    // If MI is marked as reading Reg, the partial register update is wanted.
    const MachineOperand &MO = MI->getOperand(0);
    unsigned Reg = MO.getReg();
    if (TargetRegisterInfo::isVirtualRegister(Reg)) {
        if (MO.readsReg() || MI->readsVirtualRegister(Reg))
            return 0;
    } else {
        if (MI->readsRegister(Reg, TRI))
            return 0;
    }

    // If any of the preceding 16 instructions are reading Reg, insert a
    // dependency breaking instruction.  The magic number is based on a few
    // Nehalem experiments.
    return 16;
}

// Return true for any instruction the copies the high bits of the first source
// operand into the unused high bits of the destination operand.
static bool hasUndefRegUpdate(unsigned Opcode) {
    return false;
}

/// Inform the ExeDepsFix pass how many idle instructions we would like before
/// certain undef register reads.
///
/// This catches the VCVTSI2SD family of instructions:
///
/// vcvtsi2sdq %rax, %xmm0<undef>, %xmm14
///
/// We should to be careful *not* to catch VXOR idioms which are presumably
/// handled specially in the pipeline:
///
/// vxorps %xmm1<undef>, %xmm1<undef>, %xmm1
///
/// Like getPartialRegUpdateClearance, this makes a strong assumption that the
/// high bits that are passed-through are not live.
unsigned Cse523InstrInfo::
getUndefRegClearance(const MachineInstr *MI, unsigned &OpNum,
        const TargetRegisterInfo *TRI) const {
    if (!hasUndefRegUpdate(MI->getOpcode()))
        return 0;

    // Set the OpNum parameter to the first source operand.
    OpNum = 1;

    const MachineOperand &MO = MI->getOperand(OpNum);
    if (MO.isUndef() && TargetRegisterInfo::isPhysicalRegister(MO.getReg())) {
        // Use the same magic number as getPartialRegUpdateClearance.
        return 16;
    }
    return 0;
}

void Cse523InstrInfo::
breakPartialRegDependency(MachineBasicBlock::iterator MI, unsigned OpNum,
        const TargetRegisterInfo *TRI) const {
    return;
}

MachineInstr*
Cse523InstrInfo::foldMemoryOperandImpl(MachineFunction &MF, MachineInstr *MI,
        const SmallVectorImpl<unsigned> &Ops,
        int FrameIndex) const {
    // Check switch flag
    if (NoFusing) return NULL;

    // Unless optimizing for size, don't fold to avoid partial
    // register update stalls
    if (!MF.getFunction()->getAttributes().
            hasAttribute(AttributeSet::FunctionIndex, Attribute::OptimizeForSize) &&
            hasPartialRegUpdate(MI->getOpcode()))
        return 0;

    const MachineFrameInfo *MFI = MF.getFrameInfo();
    unsigned Size = MFI->getObjectSize(FrameIndex);
    unsigned Alignment = MFI->getObjectAlignment(FrameIndex);
    // If the function stack isn't realigned we don't want to fold instructions
    // that need increased alignment.
    if (!RI.needsStackRealignment(MF))
        Alignment = std::min(Alignment, TM.getFrameLowering()->getStackAlignment());
    if (Ops.size() == 2 && Ops[0] == 0 && Ops[1] == 1) {
        unsigned NewOpc = 0;
        unsigned RCSize = 0;
        switch (MI->getOpcode()) {
            default: return NULL;
            case Cse523::TEST64rr: NewOpc = Cse523::CMP64ri32; RCSize = 8; break;
        }
        // Check if it's safe to fold the load. If the size of the object is
        // narrower than the load width, then it's not.
        if (Size < RCSize)
            return NULL;
        // Change to CMPXXri r, 0 first.
        MI->setDesc(get(NewOpc));
        MI->getOperand(1).ChangeToImmediate(0);
    } else if (Ops.size() != 1)
        return NULL;

    SmallVector<MachineOperand,4> MOs;
    MOs.push_back(MachineOperand::CreateFI(FrameIndex));
    return foldMemoryOperandImpl(MF, MI, Ops[0], MOs, Size, Alignment);
}

MachineInstr* Cse523InstrInfo::foldMemoryOperandImpl(MachineFunction &MF,
        MachineInstr *MI,
        const SmallVectorImpl<unsigned> &Ops,
        MachineInstr *LoadMI) const {
    // If loading from a FrameIndex, fold directly from the FrameIndex.
    unsigned NumOps = LoadMI->getDesc().getNumOperands();
    int FrameIndex;
    if (isLoadFromStackSlot(LoadMI, FrameIndex))
        return foldMemoryOperandImpl(MF, MI, Ops, FrameIndex);

    // Check switch flag
    if (NoFusing) return NULL;

    // Unless optimizing for size, don't fold to avoid partial
    // register update stalls
    if (!MF.getFunction()->getAttributes().
            hasAttribute(AttributeSet::FunctionIndex, Attribute::OptimizeForSize) &&
            hasPartialRegUpdate(MI->getOpcode()))
        return 0;

    // Determine the alignment of the load.
    unsigned Alignment = 0;
    if (LoadMI->hasOneMemOperand())
        Alignment = (*LoadMI->memoperands_begin())->getAlignment();
    else
        return 0;
    if (Ops.size() == 2 && Ops[0] == 0 && Ops[1] == 1) {
        unsigned NewOpc = 0;
        switch (MI->getOpcode()) {
            default: return NULL;
            case Cse523::TEST64rr: NewOpc = Cse523::CMP64ri32; break;
        }
        // Change to CMPXXri r, 0 first.
        MI->setDesc(get(NewOpc));
        MI->getOperand(1).ChangeToImmediate(0);
    } else if (Ops.size() != 1)
        return NULL;

    // Make sure the subregisters match.
    // Otherwise we risk changing the size of the load.
    if (LoadMI->getOperand(0).getSubReg() != MI->getOperand(Ops[0]).getSubReg())
        return NULL;

    SmallVector<MachineOperand,Cse523::AddrNumOperands> MOs;

    // Folding a normal load. Just copy the load's address operands.
    for (unsigned i = NumOps - Cse523::AddrNumOperands; i != NumOps; ++i)
        MOs.push_back(LoadMI->getOperand(i));

    return foldMemoryOperandImpl(MF, MI, Ops[0], MOs, 0, Alignment);
}


bool Cse523InstrInfo::canFoldMemoryOperand(const MachineInstr *MI,
        const SmallVectorImpl<unsigned> &Ops) const {
    // Check switch flag
    if (NoFusing) return 0;

    if (Ops.size() == 2 && Ops[0] == 0 && Ops[1] == 1) {
        switch (MI->getOpcode()) {
            default: return false;
            case Cse523::TEST64rr:
                     return true;
            case Cse523::ADD64ri32:
                     // FIXME: AsmPrinter doesn't know how to handle
                     // Cse523II::MO_GOT_ABSOLUTE_ADDRESS after folding.
                     if (MI->getOperand(2).getTargetFlags() == Cse523II::MO_GOT_ABSOLUTE_ADDRESS)
                         return false;
                     break;
        }
    }

    if (Ops.size() != 1)
        return false;

    unsigned OpNum = Ops[0];
    unsigned Opc = MI->getOpcode();
    unsigned NumOps = MI->getDesc().getNumOperands();
    bool isTwoAddr = NumOps > 1 &&
        MI->getDesc().getOperandConstraint(1, MCOI::TIED_TO) != -1;

    // Folding a memory location into the two-address part of a two-address
    // instruction is different than folding it other places.  It requires
    // replacing the *two* registers with the memory location.
    const DenseMap<unsigned, std::pair<unsigned,unsigned> > *OpcodeTablePtr = 0;
    if (isTwoAddr && NumOps >= 2 && OpNum < 2) {
        OpcodeTablePtr = &RegOp2MemOpTable2Addr;
    } else if (OpNum == 0) { // If operand 0
        if (Opc == Cse523::MOV64r0)
            return true;
        OpcodeTablePtr = &RegOp2MemOpTable0;
    } else if (OpNum == 1) {
        OpcodeTablePtr = &RegOp2MemOpTable1;
    } else if (OpNum == 2) {
        OpcodeTablePtr = &RegOp2MemOpTable2;
    } else if (OpNum == 3) {
        OpcodeTablePtr = &RegOp2MemOpTable3;
    }

    if (OpcodeTablePtr && OpcodeTablePtr->count(Opc))
        return true;
    return TargetInstrInfo::canFoldMemoryOperand(MI, Ops);
}

bool Cse523InstrInfo::unfoldMemoryOperand(MachineFunction &MF, MachineInstr *MI,
        unsigned Reg, bool UnfoldLoad, bool UnfoldStore,
        SmallVectorImpl<MachineInstr*> &NewMIs) const {
    assert(0);
//    DenseMap<unsigned, std::pair<unsigned,unsigned> >::const_iterator I =
//        MemOp2RegOpTable.find(MI->getOpcode());
//    if (I == MemOp2RegOpTable.end())
//        return false;
//    unsigned Opc = I->second.first;
//    unsigned Index = I->second.second & TB_INDEX_MASK;
//    bool FoldedLoad = I->second.second & TB_FOLDED_LOAD;
//    bool FoldedStore = I->second.second & TB_FOLDED_STORE;
//    if (UnfoldLoad && !FoldedLoad)
//        return false;
//    UnfoldLoad &= FoldedLoad;
//    if (UnfoldStore && !FoldedStore)
//        return false;
//    UnfoldStore &= FoldedStore;
//
//    const MCInstrDesc &MCID = get(Opc);
//    const TargetRegisterClass *RC = getRegClass(MCID, Index, &RI, MF);
//
//    SmallVector<MachineOperand, Cse523::AddrNumOperands> AddrOps;
//    SmallVector<MachineOperand,2> BeforeOps;
//    SmallVector<MachineOperand,2> AfterOps;
//    SmallVector<MachineOperand,4> ImpOps;
//    for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
//        MachineOperand &Op = MI->getOperand(i);
//        if (i >= Index && i < Index + Cse523::AddrNumOperands)
//            AddrOps.push_back(Op);
//        else if (Op.isReg() && Op.isImplicit())
//            ImpOps.push_back(Op);
//        else if (i < Index)
//            BeforeOps.push_back(Op);
//        else if (i > Index)
//            AfterOps.push_back(Op);
//    }
//
//    // Emit the load instruction.
//    if (UnfoldLoad) {
//        std::pair<MachineInstr::mmo_iterator,
//            MachineInstr::mmo_iterator> MMOs =
//                MF.extractLoadMemRefs(MI->memoperands_begin(),
//                        MI->memoperands_end());
//        loadRegFromAddr(MF, Reg, AddrOps, RC, MMOs.first, MMOs.second, NewMIs);
//        if (UnfoldStore) {
//            // Address operands cannot be marked isKill.
//            for (unsigned i = 1; i != 1 + Cse523::AddrNumOperands; ++i) {
//                MachineOperand &MO = NewMIs[0]->getOperand(i);
//                if (MO.isReg())
//                    MO.setIsKill(false);
//            }
//        }
//    }
//
//    // Emit the data processing instruction.
//    MachineInstr *DataMI = MF.CreateMachineInstr(MCID, MI->getDebugLoc(), true);
//    MachineInstrBuilder MIB(MF, DataMI);
//
//    if (FoldedStore)
//        MIB.addReg(Reg, RegState::Define);
//    for (unsigned i = 0, e = BeforeOps.size(); i != e; ++i)
//        MIB.addOperand(BeforeOps[i]);
//    if (FoldedLoad)
//        MIB.addReg(Reg);
//    for (unsigned i = 0, e = AfterOps.size(); i != e; ++i)
//        MIB.addOperand(AfterOps[i]);
//    for (unsigned i = 0, e = ImpOps.size(); i != e; ++i) {
//        MachineOperand &MO = ImpOps[i];
//        MIB.addReg(MO.getReg(),
//                getDefRegState(MO.isDef()) |
//                RegState::Implicit |
//                getKillRegState(MO.isKill()) |
//                getDeadRegState(MO.isDead()) |
//                getUndefRegState(MO.isUndef()));
//    }
//    // Change CMP32ri r, 0 back to TEST32rr r, r, etc.
//    switch (DataMI->getOpcode()) {
//        default: break;
//        case Cse523::CMP64ri32:
//        case Cse523::CMP64ri8:
//        case Cse523::CMP32ri:
//        case Cse523::CMP32ri8:
//        case Cse523::CMP16ri:
//        case Cse523::CMP16ri8:
//        case Cse523::CMP8ri: {
//                                 MachineOperand &MO0 = DataMI->getOperand(0);
//                                 MachineOperand &MO1 = DataMI->getOperand(1);
//                                 if (MO1.getImm() == 0) {
//                                     unsigned NewOpc;
//                                     switch (DataMI->getOpcode()) {
//                                         default: llvm_unreachable("Unreachable!");
//                                         case Cse523::CMP64ri8:
//                                         case Cse523::CMP64ri32: NewOpc = Cse523::TEST64rr; break;
//                                         case Cse523::CMP32ri8:
//                                         case Cse523::CMP32ri:   NewOpc = Cse523::TEST32rr; break;
//                                         case Cse523::CMP16ri8:
//                                         case Cse523::CMP16ri:   NewOpc = Cse523::TEST16rr; break;
//                                         case Cse523::CMP8ri:    NewOpc = Cse523::TEST8rr; break;
//                                     }
//                                     DataMI->setDesc(get(NewOpc));
//                                     MO1.ChangeToRegister(MO0.getReg(), false);
//                                 }
//                             }
//    }
//    NewMIs.push_back(DataMI);
//
//    // Emit the store instruction.
//    if (UnfoldStore) {
//        const TargetRegisterClass *DstRC = getRegClass(MCID, 0, &RI, MF);
//        std::pair<MachineInstr::mmo_iterator,
//            MachineInstr::mmo_iterator> MMOs =
//                MF.extractStoreMemRefs(MI->memoperands_begin(),
//                        MI->memoperands_end());
//        storeRegToAddr(MF, Reg, true, AddrOps, DstRC, MMOs.first, MMOs.second, NewMIs);
//    }

    return true;
}

bool
Cse523InstrInfo::unfoldMemoryOperand(SelectionDAG &DAG, SDNode *N,
        SmallVectorImpl<SDNode*> &NewNodes) const {
    if (!N->isMachineOpcode())
        return false;

    DenseMap<unsigned, std::pair<unsigned,unsigned> >::const_iterator I =
        MemOp2RegOpTable.find(N->getMachineOpcode());
    if (I == MemOp2RegOpTable.end())
        return false;
    unsigned Opc = I->second.first;
    unsigned Index = I->second.second & TB_INDEX_MASK;
    bool FoldedLoad = I->second.second & TB_FOLDED_LOAD;
    bool FoldedStore = I->second.second & TB_FOLDED_STORE;
    const MCInstrDesc &MCID = get(Opc);
    MachineFunction &MF = DAG.getMachineFunction();
    const TargetRegisterClass *RC = getRegClass(MCID, Index, &RI, MF);
    unsigned NumDefs = MCID.NumDefs;
    std::vector<SDValue> AddrOps;
    std::vector<SDValue> BeforeOps;
    std::vector<SDValue> AfterOps;
    SDLoc dl(N);
    unsigned NumOps = N->getNumOperands();
    for (unsigned i = 0; i != NumOps-1; ++i) {
        SDValue Op = N->getOperand(i);
        if (i >= Index-NumDefs && i < Index-NumDefs + Cse523::AddrNumOperands)
            AddrOps.push_back(Op);
        else if (i < Index-NumDefs)
            BeforeOps.push_back(Op);
        else if (i > Index-NumDefs)
            AfterOps.push_back(Op);
    }
    SDValue Chain = N->getOperand(NumOps-1);
    AddrOps.push_back(Chain);

    // Emit the load instruction.
    SDNode *Load = 0;
    if (FoldedLoad) {
        EVT VT = *RC->vt_begin();
        std::pair<MachineInstr::mmo_iterator,
            MachineInstr::mmo_iterator> MMOs =
                MF.extractLoadMemRefs(cast<MachineSDNode>(N)->memoperands_begin(),
                        cast<MachineSDNode>(N)->memoperands_end());

        unsigned Alignment = RC->getSize() == 32 ? 32 : 16;
        bool isAligned = (*MMOs.first) &&
            (*MMOs.first)->getAlignment() >= Alignment;
        Load = DAG.getMachineNode(getLoadRegOpcode(0, RC, isAligned, TM), dl,
                VT, MVT::Other, AddrOps);
        NewNodes.push_back(Load);

        // Preserve memory reference information.
        cast<MachineSDNode>(Load)->setMemRefs(MMOs.first, MMOs.second);
    }

    // Emit the data processing instruction.
    std::vector<EVT> VTs;
    const TargetRegisterClass *DstRC = 0;
    if (MCID.getNumDefs() > 0) {
        DstRC = getRegClass(MCID, 0, &RI, MF);
        VTs.push_back(*DstRC->vt_begin());
    }
    for (unsigned i = 0, e = N->getNumValues(); i != e; ++i) {
        EVT VT = N->getValueType(i);
        if (VT != MVT::Other && i >= (unsigned)MCID.getNumDefs())
            VTs.push_back(VT);
    }
    if (Load)
        BeforeOps.push_back(SDValue(Load, 0));
    std::copy(AfterOps.begin(), AfterOps.end(), std::back_inserter(BeforeOps));
    SDNode *NewNode= DAG.getMachineNode(Opc, dl, VTs, BeforeOps);
    NewNodes.push_back(NewNode);

    // Emit the store instruction.
    if (FoldedStore) {
        AddrOps.pop_back();
        AddrOps.push_back(SDValue(NewNode, 0));
        AddrOps.push_back(Chain);
        std::pair<MachineInstr::mmo_iterator,
            MachineInstr::mmo_iterator> MMOs =
                MF.extractStoreMemRefs(cast<MachineSDNode>(N)->memoperands_begin(),
                        cast<MachineSDNode>(N)->memoperands_end());

        unsigned Alignment = RC->getSize() == 32 ? 32 : 16;
        bool isAligned = (*MMOs.first) &&
            (*MMOs.first)->getAlignment() >= Alignment;
        SDNode *Store = DAG.getMachineNode(getStoreRegOpcode(0, DstRC,
                    isAligned, TM),
                dl, MVT::Other, AddrOps);
        NewNodes.push_back(Store);

        // Preserve memory reference information.
        cast<MachineSDNode>(Load)->setMemRefs(MMOs.first, MMOs.second);
    }

    return true;
}

unsigned Cse523InstrInfo::getOpcodeAfterMemoryUnfold(unsigned Opc,
        bool UnfoldLoad, bool UnfoldStore,
        unsigned *LoadRegIndex) const {
    DenseMap<unsigned, std::pair<unsigned,unsigned> >::const_iterator I =
        MemOp2RegOpTable.find(Opc);
    assert(0);
    if (I == MemOp2RegOpTable.end())
        return 0;
    bool FoldedLoad = I->second.second & TB_FOLDED_LOAD;
    bool FoldedStore = I->second.second & TB_FOLDED_STORE;
    if (UnfoldLoad && !FoldedLoad)
        return 0;
    if (UnfoldStore && !FoldedStore)
        return 0;
    if (LoadRegIndex)
        *LoadRegIndex = I->second.second & TB_INDEX_MASK;
    return I->second.first;
}

bool
Cse523InstrInfo::areLoadsFromSameBasePtr(SDNode *Load1, SDNode *Load2,
        int64_t &Offset1, int64_t &Offset2) const {
    if (!Load1->isMachineOpcode() || !Load2->isMachineOpcode())
        return false;
    unsigned Opc1 = Load1->getMachineOpcode();
    unsigned Opc2 = Load2->getMachineOpcode();
    switch (Opc1) {
        default: return false;
        case Cse523::MOV64rm:
                 break;
    }
    switch (Opc2) {
        default: return false;
        case Cse523::MOV64rm:
                 break;
    }

    // Check if chain operands and base addresses match.
    if (Load1->getOperand(0) != Load2->getOperand(0) ||
            Load1->getOperand(5) != Load2->getOperand(5))
        return false;
    // Segment operands should match as well.
    if (Load1->getOperand(4) != Load2->getOperand(4))
        return false;
    // Scale should be 1, Index should be Reg0.
    if (Load1->getOperand(1) == Load2->getOperand(1) &&
            Load1->getOperand(2) == Load2->getOperand(2)) {
        if (cast<ConstantSDNode>(Load1->getOperand(1))->getZExtValue() != 1)
            return false;

        // Now let's examine the displacements.
        if (isa<ConstantSDNode>(Load1->getOperand(3)) &&
                isa<ConstantSDNode>(Load2->getOperand(3))) {
            Offset1 = cast<ConstantSDNode>(Load1->getOperand(3))->getSExtValue();
            Offset2 = cast<ConstantSDNode>(Load2->getOperand(3))->getSExtValue();
            return true;
        }
    }
    return false;
}

bool Cse523InstrInfo::shouldScheduleLoadsNear(SDNode *Load1, SDNode *Load2,
        int64_t Offset1, int64_t Offset2,
        unsigned NumLoads) const {
    assert(Offset2 > Offset1);
    if ((Offset2 - Offset1) / 8 > 64)
        return false;

    unsigned Opc1 = Load1->getMachineOpcode();
    unsigned Opc2 = Load2->getMachineOpcode();
    if (Opc1 != Opc2)
        return false;  // FIXME: overly conservative?

    switch (Opc1) {
        default: break;
//        case Cse523::LD_Fp32m:
//        case Cse523::LD_Fp64m:
//        case Cse523::LD_Fp80m:
//          return false;
    }

    EVT VT = Load1->getValueType(0);
    switch (VT.getSimpleVT().SimpleTy) {
        default:
            // XMM registers. In 64-bit mode we can be a bit more aggressive since we
            // have 16 of them to play with.
            if (TM.getSubtargetImpl()->is64Bit()) {
                if (NumLoads >= 3)
                    return false;
            } else if (NumLoads) {
                return false;
            }
            break;
        case MVT::i8:
        case MVT::i16:
        case MVT::i32:
        case MVT::i64:
        case MVT::f32:
        case MVT::f64:
            if (NumLoads)
                return false;
            break;
    }

    return true;
}

bool Cse523InstrInfo::shouldScheduleAdjacent(MachineInstr* First,
        MachineInstr *Second) const {
    // Check if this processor supports macro-fusion. Since this is a minor
    // heuristic, we haven't specifically reserved a feature. hasAVX is a decent
    // proxy for SandyBridge+.
    //if (!TM.getSubtarget<Cse523Subtarget>().hasAVX())
    return false;
}

bool Cse523InstrInfo::
ReverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const {
    assert(Cond.size() == 1 && "Invalid Cse523 branch condition!");
    Cse523::CondCode CC = static_cast<Cse523::CondCode>(Cond[0].getImm());
    if (CC == Cse523::COND_NE_OR_P || CC == Cse523::COND_NP_OR_E)
        return true;
    Cond[0].setImm(GetOppositeBranchCondition(CC));
    return false;
}

bool Cse523InstrInfo::
isSafeToMoveRegClassDefs(const TargetRegisterClass *RC) const {
    // FIXME: Return false for x87 stack register classes for now. We can't
    // allow any loads of these registers before FpGet_ST0_80.
    assert(0);
    return false;
//    return !(RC == &Cse523::CCRRegClass || RC == &Cse523::RFP32RegClass ||
//            RC == &Cse523::RFP64RegClass || RC == &Cse523::RFP80RegClass);
}

/// getGlobalBaseReg - Return a virtual register initialized with the
/// the global base register value. Output instructions required to
/// initialize the register in the function entry block, if necessary.
///
/// TODO: Eliminate this and move the code to Cse523MachineFunctionInfo.
///
unsigned Cse523InstrInfo::getGlobalBaseReg(MachineFunction *MF) const {

    Cse523MachineFunctionInfo *Cse523FI = MF->getInfo<Cse523MachineFunctionInfo>();
    unsigned GlobalBaseReg = Cse523FI->getGlobalBaseReg();
    if (GlobalBaseReg != 0)
        return GlobalBaseReg;

    // Create the register. The code to initialize it is inserted
    // later, by the CGBR pass (below).
    MachineRegisterInfo &RegInfo = MF->getRegInfo();
    assert(0);
    GlobalBaseReg = 10;//RegInfo.createVirtualRegister(&Cse523::GR32_NOSPRegClass);
    Cse523FI->setGlobalBaseReg(GlobalBaseReg);
    return GlobalBaseReg;
}

// These are the replaceable SSE instructions. Some of these have Int variants
// that we don't include here. We don't want to replace instructions selected
// by intrinsics.
static const uint16_t ReplaceableInstrs[1][3] = {
    //PackedSingle     PackedDouble    PackedInt
//    { Cse523::MOVAPSmr,   Cse523::MOVAPDmr,  Cse523::MOVDQAmr  },
//    { Cse523::MOVAPSrm,   Cse523::MOVAPDrm,  Cse523::MOVDQArm  },
//    { Cse523::MOVAPSrr,   Cse523::MOVAPDrr,  Cse523::MOVDQArr  },
//    { Cse523::MOVUPSmr,   Cse523::MOVUPDmr,  Cse523::MOVDQUmr  },
//    { Cse523::MOVUPSrm,   Cse523::MOVUPDrm,  Cse523::MOVDQUrm  },
//    { Cse523::MOVNTPSmr,  Cse523::MOVNTPDmr, Cse523::MOVNTDQmr },
//    { Cse523::ANDNPSrm,   Cse523::ANDNPDrm,  Cse523::PANDNrm   },
//    { Cse523::ANDNPSrr,   Cse523::ANDNPDrr,  Cse523::PANDNrr   },
//    { Cse523::ANDPSrm,    Cse523::ANDPDrm,   Cse523::PANDrm    },
//    { Cse523::ANDPSrr,    Cse523::ANDPDrr,   Cse523::PANDrr    },
//    { Cse523::ORPSrm,     Cse523::ORPDrm,    Cse523::PORrm     },
//    { Cse523::ORPSrr,     Cse523::ORPDrr,    Cse523::PORrr     },
//    { Cse523::XORPSrm,    Cse523::XORPDrm,   Cse523::PXORrm    },
//    { Cse523::XORPSrr,    Cse523::XORPDrr,   Cse523::PXORrr    }
};

static const uint16_t ReplaceableInstrsAVX2[1][3] = {
    //PackedSingle       PackedDouble       PackedInt
};

// FIXME: Some shuffle and unpack instructions have equivalents in different
// domains, but they require a bit more work than just switching opcodes.

static const uint16_t *lookup(unsigned opcode, unsigned domain) {
    for (unsigned i = 0, e = array_lengthof(ReplaceableInstrs); i != e; ++i)
        if (ReplaceableInstrs[i][domain-1] == opcode)
            return ReplaceableInstrs[i];
    return 0;
}

static const uint16_t *lookupAVX2(unsigned opcode, unsigned domain) {
    for (unsigned i = 0, e = array_lengthof(ReplaceableInstrsAVX2); i != e; ++i)
        if (ReplaceableInstrsAVX2[i][domain-1] == opcode)
            return ReplaceableInstrsAVX2[i];
    return 0;
}

std::pair<uint16_t, uint16_t>
Cse523InstrInfo::getExecutionDomain(const MachineInstr *MI) const {
    uint16_t domain = (MI->getDesc().TSFlags >> Cse523II::SSEDomainShift) & 3;
    uint16_t validDomains = 0;
    if (domain && lookup(MI->getOpcode(), domain))
        validDomains = 0xe;
    else if (domain && lookupAVX2(MI->getOpcode(), domain))
        validDomains = 0x6;
    return std::make_pair(domain, validDomains);
}

void Cse523InstrInfo::setExecutionDomain(MachineInstr *MI, unsigned Domain) const {
    assert(Domain>0 && Domain<4 && "Invalid execution domain");
    uint16_t dom = (MI->getDesc().TSFlags >> Cse523II::SSEDomainShift) & 3;
    assert(dom && "Not an SSE instruction");
    const uint16_t *table = lookup(MI->getOpcode(), dom);
    assert(table && "Cannot change domain");
    MI->setDesc(get(table[Domain-1]));
}

/// getNoopForMachoTarget - Return the noop instruction to use for a noop.
void Cse523InstrInfo::getNoopForMachoTarget(MCInst &NopInst) const {
    NopInst.setOpcode(Cse523::NOOP);
}

bool Cse523InstrInfo::isHighLatencyDef(int opc) const {
    switch (opc) {
        default: return false;
// TODO:
//        case Cse523::DIVSDrm:
//        case Cse523::DIVSDrm_Int:
//        case Cse523::DIVSDrr:
//        case Cse523::DIVSDrr_Int:
//        case Cse523::DIVSSrm:
//        case Cse523::DIVSSrm_Int:
//        case Cse523::DIVSSrr:
//        case Cse523::DIVSSrr_Int:
//        case Cse523::SQRTPDm:
//        case Cse523::SQRTPDr:
//        case Cse523::SQRTPSm:
//        case Cse523::SQRTPSr:
//        case Cse523::SQRTSDm:
//        case Cse523::SQRTSDm_Int:
//        case Cse523::SQRTSDr:
//        case Cse523::SQRTSDr_Int:
//        case Cse523::SQRTSSm:
//        case Cse523::SQRTSSm_Int:
//        case Cse523::SQRTSSr:
//        case Cse523::SQRTSSr_Int:
                 return true;
    }
}

bool Cse523InstrInfo::
hasHighOperandLatency(const InstrItineraryData *ItinData,
        const MachineRegisterInfo *MRI,
        const MachineInstr *DefMI, unsigned DefIdx,
        const MachineInstr *UseMI, unsigned UseIdx) const {
    return isHighLatencyDef(DefMI->getOpcode());
}

namespace {
    /// CGBR - Create Global Base Reg pass. This initializes the PIC
    /// global base register for cse523-32.
    struct CGBR : public MachineFunctionPass {
        static char ID;
        CGBR() : MachineFunctionPass(ID) {}

        virtual bool runOnMachineFunction(MachineFunction &MF) {
            const Cse523TargetMachine *TM =
                static_cast<const Cse523TargetMachine *>(&MF.getTarget());

            // Only emit a global base reg in PIC mode.
            if (TM->getRelocationModel() != Reloc::PIC_)
                return false;

            Cse523MachineFunctionInfo *Cse523FI = MF.getInfo<Cse523MachineFunctionInfo>();
            unsigned GlobalBaseReg = Cse523FI->getGlobalBaseReg();

            // If we didn't need a GlobalBaseReg, don't insert code.
            if (GlobalBaseReg == 0)
                return false;
            assert(0);

            // Insert the set of GlobalBaseReg into the first MBB of the function
//            MachineBasicBlock &FirstMBB = MF.front();
//            MachineBasicBlock::iterator MBBI = FirstMBB.begin();
//            DebugLoc DL = FirstMBB.findDebugLoc(MBBI);
//            MachineRegisterInfo &RegInfo = MF.getRegInfo();
//            const Cse523InstrInfo *TII = TM->getInstrInfo();
//
//            unsigned PC;
//            if (TM->getSubtarget<Cse523Subtarget>().isPICStyleGOT())
//                PC = 10;
//            else
//                PC = GlobalBaseReg;
//
//            // Operand of MovePCtoStack is completely ignored by asm printer. It's
//            // only used in JIT code emission as displacement to pc.
//            BuildMI(FirstMBB, MBBI, DL, TII->get(Cse523::MOVPC32r), PC).addImm(0);
//
//            // If we're using vanilla 'GOT' PIC style, we should use relative addressing
//            // not to pc, but to _GLOBAL_OFFSET_TABLE_ external.
//            if (TM->getSubtarget<Cse523Subtarget>().isPICStyleGOT()) {
//                // Generate addl $__GLOBAL_OFFSET_TABLE_ + [.-piclabel], %some_register
//                BuildMI(FirstMBB, MBBI, DL, TII->get(Cse523::ADD32ri), GlobalBaseReg)
//                    .addReg(PC).addExternalSymbol("_GLOBAL_OFFSET_TABLE_",
//                            Cse523II::MO_GOT_ABSOLUTE_ADDRESS);
//            }

            return true;
        }

        virtual const char *getPassName() const {
            return "Cse523 PIC Global Base Reg Initialization";
        }

        virtual void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.setPreservesCFG();
            MachineFunctionPass::getAnalysisUsage(AU);
        }
    };
}

char CGBR::ID = 0;
FunctionPass*
llvm::createGlobalBaseRegPass() { return new CGBR(); }

namespace {
    struct LDTLSCleanup : public MachineFunctionPass {
        static char ID;
        LDTLSCleanup() : MachineFunctionPass(ID) {}

        virtual bool runOnMachineFunction(MachineFunction &MF) {
            Cse523MachineFunctionInfo* MFI = MF.getInfo<Cse523MachineFunctionInfo>();
            if (MFI->getNumLocalDynamicTLSAccesses() < 2) {
                // No point folding accesses if there isn't at least two.
                return false;
            }

            MachineDominatorTree *DT = &getAnalysis<MachineDominatorTree>();
            return VisitNode(DT->getRootNode(), 0);
        }

        // Visit the dominator subtree rooted at Node in pre-order.
        // If TLSBaseAddrReg is non-null, then use that to replace any
        // TLS_base_addr instructions. Otherwise, create the register
        // when the first such instruction is seen, and then use it
        // as we encounter more instructions.
        bool VisitNode(MachineDomTreeNode *Node, unsigned TLSBaseAddrReg) {
            MachineBasicBlock *BB = Node->getBlock();
            bool Changed = false;

            // Traverse the current block.
            for (MachineBasicBlock::iterator I = BB->begin(), E = BB->end(); I != E;
                    ++I) {
                switch (I->getOpcode()) {
                    case Cse523::TLS_base_addr64:
                        if (TLSBaseAddrReg)
                            I = ReplaceTLSBaseAddrCall(I, TLSBaseAddrReg);
                        else
                            I = SetRegister(I, &TLSBaseAddrReg);
                        Changed = true;
                        break;
                    default:
                        break;
                }
            }

            // Visit the children of this block in the dominator tree.
            for (MachineDomTreeNode::iterator I = Node->begin(), E = Node->end();
                    I != E; ++I) {
                Changed |= VisitNode(*I, TLSBaseAddrReg);
            }

            return Changed;
        }

        // Replace the TLS_base_addr instruction I with a copy from
        // TLSBaseAddrReg, returning the new instruction.
        MachineInstr *ReplaceTLSBaseAddrCall(MachineInstr *I,
                unsigned TLSBaseAddrReg) {
            MachineFunction *MF = I->getParent()->getParent();
            const Cse523TargetMachine *TM =
                static_cast<const Cse523TargetMachine *>(&MF->getTarget());
            const Cse523InstrInfo *TII = TM->getInstrInfo();

            // Insert a Copy from TLSBaseAddrReg to RAX/EAX.
            MachineInstr *Copy = BuildMI(*I->getParent(), I, I->getDebugLoc(),
                    TII->get(TargetOpcode::COPY), Cse523::RAX)
                .addReg(TLSBaseAddrReg);

            // Erase the TLS_base_addr instruction.
            I->eraseFromParent();

            return Copy;
        }

        // Create a virtal register in *TLSBaseAddrReg, and populate it by
        // inserting a copy instruction after I. Returns the new instruction.
        MachineInstr *SetRegister(MachineInstr *I, unsigned *TLSBaseAddrReg) {
            MachineFunction *MF = I->getParent()->getParent();
            const Cse523TargetMachine *TM =
                static_cast<const Cse523TargetMachine *>(&MF->getTarget());
            const Cse523InstrInfo *TII = TM->getInstrInfo();

            // Create a virtual register for the TLS base address.
            MachineRegisterInfo &RegInfo = MF->getRegInfo();
            *TLSBaseAddrReg = RegInfo.createVirtualRegister(&Cse523::GR64RegClass);

            // Insert a copy from RAX/EAX to TLSBaseAddrReg.
            MachineInstr *Next = I->getNextNode();
            MachineInstr *Copy = BuildMI(*I->getParent(), Next, I->getDebugLoc(),
                    TII->get(TargetOpcode::COPY),
                    *TLSBaseAddrReg)
                .addReg(Cse523::RAX);

            return Copy;
        }

        virtual const char *getPassName() const {
            return "Local Dynamic TLS Access Clean-up";
        }

        virtual void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.setPreservesCFG();
            AU.addRequired<MachineDominatorTree>();
            MachineFunctionPass::getAnalysisUsage(AU);
        }
    };
}

char LDTLSCleanup::ID = 0;
FunctionPass*
llvm::createCleanupLocalDynamicTLSPass() { return new LDTLSCleanup(); }
