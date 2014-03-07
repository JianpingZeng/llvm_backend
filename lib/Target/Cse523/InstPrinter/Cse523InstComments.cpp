//===-- Cse523InstComments.cpp - Generate verbose-asm comments for instrs ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This defines functionality used to emit comments about Cse523 instructions to
// an output stream for -fverbose-asm.
//
//===----------------------------------------------------------------------===//

#include "Cse523InstComments.h"
#include "MCTargetDesc/Cse523MCTargetDesc.h"
#include "Utils/Cse523ShuffleDecode.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

//===----------------------------------------------------------------------===//
// Top Level Entrypoint
//===----------------------------------------------------------------------===//

/// EmitAnyCse523InstComments - This function decodes x86 instructions and prints
/// newline terminated strings to the specified string if desired.  This
/// information is shown in disassembly dumps when verbose assembly is enabled.
void llvm::EmitAnyCse523InstComments(const MCInst *MI, raw_ostream &OS,
                                  const char *(*getRegName)(unsigned)) {
  // If this is a shuffle operation, the switch should fill in this state.
  SmallVector<int, 8> ShuffleMask;
  const char *DestName = 0, *Src1Name = 0, *Src2Name = 0;

//  switch (MI->getOpcode()) {
//  case Cse523::INSERTPSrr:
//  case Cse523::VINSERTPSrr:
//    DestName = getRegName(MI->getOperand(0).getReg());
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    if(MI->getOperand(3).isImm())
//      DecodeINSERTPSMask(MI->getOperand(3).getImm(), ShuffleMask);
//    break;
//
//  case Cse523::MOVLHPSrr:
//  case Cse523::VMOVLHPSrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeMOVLHPSMask(2, ShuffleMask);
//    break;
//
//  case Cse523::MOVHLPSrr:
//  case Cse523::VMOVHLPSrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeMOVHLPSMask(2, ShuffleMask);
//    break;
//
//  case Cse523::PALIGNR128rr:
//  case Cse523::VPALIGNR128rr:
//    Src1Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::PALIGNR128rm:
//  case Cse523::VPALIGNR128rm:
//    Src2Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodePALIGNRMask(MVT::v16i8,
//                        MI->getOperand(MI->getNumOperands()-1).getImm(),
//                        ShuffleMask);
//    break;
//  case Cse523::VPALIGNR256rr:
//    Src1Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VPALIGNR256rm:
//    Src2Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodePALIGNRMask(MVT::v32i8,
//                        MI->getOperand(MI->getNumOperands()-1).getImm(),
//                        ShuffleMask);
//    break;
//
//  case Cse523::PSHUFDri:
//  case Cse523::VPSHUFDri:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    // FALL THROUGH.
//  case Cse523::PSHUFDmi:
//  case Cse523::VPSHUFDmi:
//    DestName = getRegName(MI->getOperand(0).getReg());
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodePSHUFMask(MVT::v4i32,
//                      MI->getOperand(MI->getNumOperands()-1).getImm(),
//                      ShuffleMask);
//    break;
//  case Cse523::VPSHUFDYri:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    // FALL THROUGH.
//  case Cse523::VPSHUFDYmi:
//    DestName = getRegName(MI->getOperand(0).getReg());
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodePSHUFMask(MVT::v8i32,
//                      MI->getOperand(MI->getNumOperands()-1).getImm(),
//                      ShuffleMask);
//    break;
//
//
//  case Cse523::PSHUFHWri:
//  case Cse523::VPSHUFHWri:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    // FALL THROUGH.
//  case Cse523::PSHUFHWmi:
//  case Cse523::VPSHUFHWmi:
//    DestName = getRegName(MI->getOperand(0).getReg());
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodePSHUFHWMask(MVT::v8i16,
//                        MI->getOperand(MI->getNumOperands()-1).getImm(),
//                        ShuffleMask);
//    break;
//  case Cse523::VPSHUFHWYri:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    // FALL THROUGH.
//  case Cse523::VPSHUFHWYmi:
//    DestName = getRegName(MI->getOperand(0).getReg());
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodePSHUFHWMask(MVT::v16i16,
//                        MI->getOperand(MI->getNumOperands()-1).getImm(),
//                        ShuffleMask);
//    break;
//  case Cse523::PSHUFLWri:
//  case Cse523::VPSHUFLWri:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    // FALL THROUGH.
//  case Cse523::PSHUFLWmi:
//  case Cse523::VPSHUFLWmi:
//    DestName = getRegName(MI->getOperand(0).getReg());
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodePSHUFLWMask(MVT::v8i16,
//                        MI->getOperand(MI->getNumOperands()-1).getImm(),
//                        ShuffleMask);
//    break;
//  case Cse523::VPSHUFLWYri:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    // FALL THROUGH.
//  case Cse523::VPSHUFLWYmi:
//    DestName = getRegName(MI->getOperand(0).getReg());
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodePSHUFLWMask(MVT::v16i16,
//                        MI->getOperand(MI->getNumOperands()-1).getImm(),
//                        ShuffleMask);
//    break;
//
//  case Cse523::PUNPCKHBWrr:
//  case Cse523::VPUNPCKHBWrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::PUNPCKHBWrm:
//  case Cse523::VPUNPCKHBWrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKHMask(MVT::v16i8, ShuffleMask);
//    break;
//  case Cse523::VPUNPCKHBWYrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VPUNPCKHBWYrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKHMask(MVT::v32i8, ShuffleMask);
//    break;
//  case Cse523::PUNPCKHWDrr:
//  case Cse523::VPUNPCKHWDrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::PUNPCKHWDrm:
//  case Cse523::VPUNPCKHWDrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKHMask(MVT::v8i16, ShuffleMask);
//    break;
//  case Cse523::VPUNPCKHWDYrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VPUNPCKHWDYrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKHMask(MVT::v16i16, ShuffleMask);
//    break;
//  case Cse523::PUNPCKHDQrr:
//  case Cse523::VPUNPCKHDQrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::PUNPCKHDQrm:
//  case Cse523::VPUNPCKHDQrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKHMask(MVT::v4i32, ShuffleMask);
//    break;
//  case Cse523::VPUNPCKHDQYrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VPUNPCKHDQYrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKHMask(MVT::v8i32, ShuffleMask);
//    break;
//  case Cse523::PUNPCKHQDQrr:
//  case Cse523::VPUNPCKHQDQrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::PUNPCKHQDQrm:
//  case Cse523::VPUNPCKHQDQrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKHMask(MVT::v2i64, ShuffleMask);
//    break;
//  case Cse523::VPUNPCKHQDQYrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VPUNPCKHQDQYrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKHMask(MVT::v4i64, ShuffleMask);
//    break;
//
//  case Cse523::PUNPCKLBWrr:
//  case Cse523::VPUNPCKLBWrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::PUNPCKLBWrm:
//  case Cse523::VPUNPCKLBWrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKLMask(MVT::v16i8, ShuffleMask);
//    break;
//  case Cse523::VPUNPCKLBWYrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VPUNPCKLBWYrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKLMask(MVT::v32i8, ShuffleMask);
//    break;
//  case Cse523::PUNPCKLWDrr:
//  case Cse523::VPUNPCKLWDrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::PUNPCKLWDrm:
//  case Cse523::VPUNPCKLWDrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKLMask(MVT::v8i16, ShuffleMask);
//    break;
//  case Cse523::VPUNPCKLWDYrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VPUNPCKLWDYrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKLMask(MVT::v16i16, ShuffleMask);
//    break;
//  case Cse523::PUNPCKLDQrr:
//  case Cse523::VPUNPCKLDQrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::PUNPCKLDQrm:
//  case Cse523::VPUNPCKLDQrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKLMask(MVT::v4i32, ShuffleMask);
//    break;
//  case Cse523::VPUNPCKLDQYrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VPUNPCKLDQYrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKLMask(MVT::v8i32, ShuffleMask);
//    break;
//  case Cse523::PUNPCKLQDQrr:
//  case Cse523::VPUNPCKLQDQrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::PUNPCKLQDQrm:
//  case Cse523::VPUNPCKLQDQrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKLMask(MVT::v2i64, ShuffleMask);
//    break;
//  case Cse523::VPUNPCKLQDQYrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VPUNPCKLQDQYrm:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    DecodeUNPCKLMask(MVT::v4i64, ShuffleMask);
//    break;
//
//  case Cse523::SHUFPDrri:
//  case Cse523::VSHUFPDrri:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::SHUFPDrmi:
//  case Cse523::VSHUFPDrmi:
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodeSHUFPMask(MVT::v2f64,
//                      MI->getOperand(MI->getNumOperands()-1).getImm(),
//                      ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::VSHUFPDYrri:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VSHUFPDYrmi:
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodeSHUFPMask(MVT::v4f64,
//                      MI->getOperand(MI->getNumOperands()-1).getImm(),
//                      ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//
//  case Cse523::SHUFPSrri:
//  case Cse523::VSHUFPSrri:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::SHUFPSrmi:
//  case Cse523::VSHUFPSrmi:
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodeSHUFPMask(MVT::v4f32,
//                      MI->getOperand(MI->getNumOperands()-1).getImm(),
//                      ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::VSHUFPSYrri:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VSHUFPSYrmi:
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodeSHUFPMask(MVT::v8f32,
//                      MI->getOperand(MI->getNumOperands()-1).getImm(),
//                      ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//
//  case Cse523::UNPCKLPDrr:
//  case Cse523::VUNPCKLPDrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::UNPCKLPDrm:
//  case Cse523::VUNPCKLPDrm:
//    DecodeUNPCKLMask(MVT::v2f64, ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::VUNPCKLPDYrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VUNPCKLPDYrm:
//    DecodeUNPCKLMask(MVT::v4f64, ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::UNPCKLPSrr:
//  case Cse523::VUNPCKLPSrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::UNPCKLPSrm:
//  case Cse523::VUNPCKLPSrm:
//    DecodeUNPCKLMask(MVT::v4f32, ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::VUNPCKLPSYrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VUNPCKLPSYrm:
//    DecodeUNPCKLMask(MVT::v8f32, ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::UNPCKHPDrr:
//  case Cse523::VUNPCKHPDrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::UNPCKHPDrm:
//  case Cse523::VUNPCKHPDrm:
//    DecodeUNPCKHMask(MVT::v2f64, ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::VUNPCKHPDYrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VUNPCKHPDYrm:
//    DecodeUNPCKHMask(MVT::v4f64, ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::UNPCKHPSrr:
//  case Cse523::VUNPCKHPSrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::UNPCKHPSrm:
//  case Cse523::VUNPCKHPSrm:
//    DecodeUNPCKHMask(MVT::v4f32, ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::VUNPCKHPSYrr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VUNPCKHPSYrm:
//    DecodeUNPCKHMask(MVT::v8f32, ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::VPERMILPSri:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    // FALL THROUGH.
//  case Cse523::VPERMILPSmi:
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodePSHUFMask(MVT::v4f32,
//                      MI->getOperand(MI->getNumOperands()-1).getImm(),
//                      ShuffleMask);
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::VPERMILPSYri:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    // FALL THROUGH.
//  case Cse523::VPERMILPSYmi:
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodePSHUFMask(MVT::v8f32,
//                      MI->getOperand(MI->getNumOperands()-1).getImm(),
//                      ShuffleMask);
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::VPERMILPDri:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    // FALL THROUGH.
//  case Cse523::VPERMILPDmi:
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodePSHUFMask(MVT::v2f64,
//                      MI->getOperand(MI->getNumOperands()-1).getImm(),
//                      ShuffleMask);
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::VPERMILPDYri:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    // FALL THROUGH.
//  case Cse523::VPERMILPDYmi:
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodePSHUFMask(MVT::v4f64,
//                      MI->getOperand(MI->getNumOperands()-1).getImm(),
//                      ShuffleMask);
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::VPERM2F128rr:
//  case Cse523::VPERM2I128rr:
//    Src2Name = getRegName(MI->getOperand(2).getReg());
//    // FALL THROUGH.
//  case Cse523::VPERM2F128rm:
//  case Cse523::VPERM2I128rm:
//    // For instruction comments purpose, assume the 256-bit vector is v4i64.
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodeVPERM2X128Mask(MVT::v4i64,
//                           MI->getOperand(MI->getNumOperands()-1).getImm(),
//                           ShuffleMask);
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  case Cse523::VPERMQYri:
//  case Cse523::VPERMPDYri:
//    Src1Name = getRegName(MI->getOperand(1).getReg());
//    // FALL THROUGH.
//  case Cse523::VPERMQYmi:
//  case Cse523::VPERMPDYmi:
//    if(MI->getOperand(MI->getNumOperands()-1).isImm())
//      DecodeVPERMMask(MI->getOperand(MI->getNumOperands()-1).getImm(),
//                      ShuffleMask);
//    DestName = getRegName(MI->getOperand(0).getReg());
//    break;
//  }


  // If this was a shuffle operation, print the shuffle mask.
  if (!ShuffleMask.empty()) {
    if (DestName == 0) DestName = Src1Name;
    OS << (DestName ? DestName : "mem") << " = ";

    // If the two sources are the same, canonicalize the input elements to be
    // from the first src so that we get larger element spans.
    if (Src1Name == Src2Name) {
      for (unsigned i = 0, e = ShuffleMask.size(); i != e; ++i) {
        if ((int)ShuffleMask[i] >= 0 && // Not sentinel.
            ShuffleMask[i] >= (int)e)        // From second mask.
          ShuffleMask[i] -= e;
      }
    }

    // The shuffle mask specifies which elements of the src1/src2 fill in the
    // destination, with a few sentinel values.  Loop through and print them
    // out.
    for (unsigned i = 0, e = ShuffleMask.size(); i != e; ++i) {
      if (i != 0)
        OS << ',';
      if (ShuffleMask[i] == SM_SentinelZero) {
        OS << "zero";
        continue;
      }

      // Otherwise, it must come from src1 or src2.  Print the span of elements
      // that comes from this src.
      bool isSrc1 = ShuffleMask[i] < (int)ShuffleMask.size();
      const char *SrcName = isSrc1 ? Src1Name : Src2Name;
      OS << (SrcName ? SrcName : "mem") << '[';
      bool IsFirst = true;
      while (i != e &&
             (int)ShuffleMask[i] >= 0 &&
             (ShuffleMask[i] < (int)ShuffleMask.size()) == isSrc1) {
        if (!IsFirst)
          OS << ',';
        else
          IsFirst = false;
        OS << ShuffleMask[i] % ShuffleMask.size();
        ++i;
      }
      OS << ']';
      --i;  // For loop increments element #.
    }
    //MI->print(OS, 0);
    OS << "\n";
  }

}
