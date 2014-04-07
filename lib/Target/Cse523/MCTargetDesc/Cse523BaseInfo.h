//===-- Cse523BaseInfo.h - Top level definitions for Cse523 -------- --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the Cse523 target useful for the compiler back-end and the MC libraries.
// As such, it deliberately does not include references to LLVM core
// code gen types, passes, etc..
//
//===----------------------------------------------------------------------===//

#ifndef Cse523BASEINFO_H
#define Cse523BASEINFO_H

#include "Cse523MCTargetDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

namespace Cse523 {
  // Enums for memory operand decoding.  Each memory operand is represented with
  // a 5 operand sequence in the form:
  //   [BaseReg, ScaleAmt, IndexReg, Disp, Segment]
  // These enums help decode this.
  enum {
    AddrBaseReg = 0,
    AddrScaleAmt = 1,
    AddrIndexReg = 2,
    AddrDisp = 3,

    /// AddrSegmentReg - The operand # of the segment in the memory operand.
    AddrSegmentReg = 4,

    /// AddrNumOperands - Total number of operands in a memory reference.
    AddrNumOperands = 5
  };
} // end namespace Cse523;

/// Cse523II - This namespace holds all of the target specific flags that
/// instruction info tracks.
///
namespace Cse523II {
  /// Target Operand Flag enum.
  enum TOF {
    //===------------------------------------------------------------------===//
    // Cse523 Specific MachineOperand flags.

    MO_NO_FLAG,

    /// MO_GOT_ABSOLUTE_ADDRESS - On a symbol operand, this represents a
    /// relocation of:
    ///    SYMBOL_LABEL + [. - PICBASELABEL]
    MO_GOT_ABSOLUTE_ADDRESS,

    /// MO_PIC_BASE_OFFSET - On a symbol operand this indicates that the
    /// immediate should get the value of the symbol minus the PIC base label:
    ///    SYMBOL_LABEL - PICBASELABEL
    MO_PIC_BASE_OFFSET,

    /// MO_GOT - On a symbol operand this indicates that the immediate is the
    /// offset to the GOT entry for the symbol name from the base of the GOT.
    ///
    /// See the Cse523-64 ELF ABI supplement for more details.
    ///    SYMBOL_LABEL @GOT
    MO_GOT,

    /// MO_GOTOFF - On a symbol operand this indicates that the immediate is
    /// the offset to the location of the symbol name from the base of the GOT.
    ///
    /// See the Cse523-64 ELF ABI supplement for more details.
    ///    SYMBOL_LABEL @GOTOFF
    MO_GOTOFF,

    /// MO_GOTPCREL - On a symbol operand this indicates that the immediate is
    /// offset to the GOT entry for the symbol name from the current code
    /// location.
    ///
    /// See the Cse523-64 ELF ABI supplement for more details.
    ///    SYMBOL_LABEL @GOTPCREL
    MO_GOTPCREL,

    /// MO_PLT - On a symbol operand this indicates that the immediate is
    /// offset to the PLT entry of symbol name from the current code location.
    ///
    /// See the Cse523-64 ELF ABI supplement for more details.
    ///    SYMBOL_LABEL @PLT
    MO_PLT,

    /// MO_TLSGD - On a symbol operand this indicates that the immediate is
    /// the offset of the GOT entry with the TLS index structure that contains
    /// the module number and variable offset for the symbol. Used in the
    /// general dynamic TLS access model.
    ///
    /// See 'ELF Handling for Thread-Local Storage' for more details.
    ///    SYMBOL_LABEL @TLSGD
    MO_TLSGD,

    /// MO_TLSLD - On a symbol operand this indicates that the immediate is
    /// the offset of the GOT entry with the TLS index for the module that
    /// contains the symbol. When this index is passed to a call to
    /// __tls_get_addr, the function will return the base address of the TLS
    /// block for the symbol. Used in the cse523-64 local dynamic TLS access model.
    ///
    /// See 'ELF Handling for Thread-Local Storage' for more details.
    ///    SYMBOL_LABEL @TLSLD
    MO_TLSLD,

    /// MO_TLSLDM - On a symbol operand this indicates that the immediate is
    /// the offset of the GOT entry with the TLS index for the module that
    /// contains the symbol. When this index is passed to a call to
    /// ___tls_get_addr, the function will return the base address of the TLS
    /// block for the symbol. Used in the IA32 local dynamic TLS access model.
    ///
    /// See 'ELF Handling for Thread-Local Storage' for more details.
    ///    SYMBOL_LABEL @TLSLDM
    MO_TLSLDM,

    /// MO_GOTTPOFF - On a symbol operand this indicates that the immediate is
    /// the offset of the GOT entry with the thread-pointer offset for the
    /// symbol. Used in the cse523-64 initial exec TLS access model.
    ///
    /// See 'ELF Handling for Thread-Local Storage' for more details.
    ///    SYMBOL_LABEL @GOTTPOFF
    MO_GOTTPOFF,

    /// MO_INDNTPOFF - On a symbol operand this indicates that the immediate is
    /// the absolute address of the GOT entry with the negative thread-pointer
    /// offset for the symbol. Used in the non-PIC IA32 initial exec TLS access
    /// model.
    ///
    /// See 'ELF Handling for Thread-Local Storage' for more details.
    ///    SYMBOL_LABEL @INDNTPOFF
    MO_INDNTPOFF,

    /// MO_TPOFF - On a symbol operand this indicates that the immediate is
    /// the thread-pointer offset for the symbol. Used in the cse523-64 local
    /// exec TLS access model.
    ///
    /// See 'ELF Handling for Thread-Local Storage' for more details.
    ///    SYMBOL_LABEL @TPOFF
    MO_TPOFF,

    /// MO_DTPOFF - On a symbol operand this indicates that the immediate is
    /// the offset of the GOT entry with the TLS offset of the symbol. Used
    /// in the local dynamic TLS access model.
    ///
    /// See 'ELF Handling for Thread-Local Storage' for more details.
    ///    SYMBOL_LABEL @DTPOFF
    MO_DTPOFF,

    /// MO_NTPOFF - On a symbol operand this indicates that the immediate is
    /// the negative thread-pointer offset for the symbol. Used in the IA32
    /// local exec TLS access model.
    ///
    /// See 'ELF Handling for Thread-Local Storage' for more details.
    ///    SYMBOL_LABEL @NTPOFF
    MO_NTPOFF,

    /// MO_GOTNTPOFF - On a symbol operand this indicates that the immediate is
    /// the offset of the GOT entry with the negative thread-pointer offset for
    /// the symbol. Used in the PIC IA32 initial exec TLS access model.
    ///
    /// See 'ELF Handling for Thread-Local Storage' for more details.
    ///    SYMBOL_LABEL @GOTNTPOFF
    MO_GOTNTPOFF,

    /// MO_DLLIMPORT - On a symbol operand "FOO", this indicates that the
    /// reference is actually to the "__imp_FOO" symbol.  This is used for
    /// dllimport linkage on windows.
    MO_DLLIMPORT,

    /// MO_DARWIN_STUB - On a symbol operand "FOO", this indicates that the
    /// reference is actually to the "FOO$stub" symbol.  This is used for calls
    /// and jumps to external functions on Tiger and earlier.
    MO_DARWIN_STUB,

    /// MO_DARWIN_NONLAZY - On a symbol operand "FOO", this indicates that the
    /// reference is actually to the "FOO$non_lazy_ptr" symbol, which is a
    /// non-PIC-base-relative reference to a non-hidden dyld lazy pointer stub.
    MO_DARWIN_NONLAZY,

    /// MO_DARWIN_NONLAZY_PIC_BASE - On a symbol operand "FOO", this indicates
    /// that the reference is actually to "FOO$non_lazy_ptr - PICBASE", which is
    /// a PIC-base-relative reference to a non-hidden dyld lazy pointer stub.
    MO_DARWIN_NONLAZY_PIC_BASE,

    /// MO_DARWIN_HIDDEN_NONLAZY_PIC_BASE - On a symbol operand "FOO", this
    /// indicates that the reference is actually to "FOO$non_lazy_ptr -PICBASE",
    /// which is a PIC-base-relative reference to a hidden dyld lazy pointer
    /// stub.
    MO_DARWIN_HIDDEN_NONLAZY_PIC_BASE,

    /// MO_TLVP - On a symbol operand this indicates that the immediate is
    /// some TLS offset.
    ///
    /// This is the TLS offset for the Darwin TLS mechanism.
    MO_TLVP,

    /// MO_TLVP_PIC_BASE - On a symbol operand this indicates that the immediate
    /// is some TLS offset from the picbase.
    ///
    /// This is the 32-bit TLS offset for Darwin TLS in PIC mode.
    MO_TLVP_PIC_BASE,

    /// MO_SECREL - On a symbol operand this indicates that the immediate is
    /// the offset from beginning of section.
    ///
    /// This is the TLS offset for the COFF/Windows TLS mechanism.
    MO_SECREL
  };

  enum {
    //===------------------------------------------------------------------===//
    // Instruction encodings.  These are the standard/most common forms for Cse523
    // instructions.
    //

    // PseudoFrm - This represents an instruction that is a pseudo instruction
    // or one that has not been implemented yet.  It is illegal to code generate
    // it, but tolerated for intermediate implementation stages.
    Pseudo         = 0,

    /// Raw - This form is for instructions that don't have any operands, so
    /// they are just a fixed opcode value, like 'leave'.
    RawFrm         = 1,

    /// AddRegFrm - This form is used for instructions like 'push r32' that have
    /// their one register operand added to their opcode.
    AddRegFrm      = 2,

    /// MRMDestReg - This form is used for instructions that use the Mod/RM byte
    /// to specify a destination, which in this case is a register.
    ///
    MRMDestReg     = 3,

    /// MRMDestMem - This form is used for instructions that use the Mod/RM byte
    /// to specify a destination, which in this case is memory.
    ///
    MRMDestMem     = 4,

    /// MRMSrcReg - This form is used for instructions that use the Mod/RM byte
    /// to specify a source, which in this case is a register.
    ///
    MRMSrcReg      = 5,

    /// MRMSrcMem - This form is used for instructions that use the Mod/RM byte
    /// to specify a source, which in this case is memory.
    ///
    MRMSrcMem      = 6,

    /// RawFrmMemOffs - This form is for instructions that store an absolute
    /// memory offset as an immediate with a possible segment override.
    RawFrmMemOffs  = 7,

    /// RawFrmSrc - This form is for instructions that use the source index
    /// register SI/ESI/RSI with a possible segment override.
    RawFrmSrc      = 8,

    /// RawFrmDst - This form is for instructions that use the destination index
    /// register DI/EDI/ESI.
    RawFrmDst      = 9,

    /// RawFrmSrc - This form is for instructions that use the the source index
    /// register SI/ESI/ERI with a possible segment override, and also the
    /// destination index register DI/ESI/RDI.
    RawFrmDstSrc   = 10,

    /// RawFrmImm8 - This is used for the ENTER instruction, which has two
    /// immediates, the first of which is a 16-bit immediate (specified by
    /// the imm encoding) and the second is a 8-bit fixed value.
    RawFrmImm8 = 11,

    /// RawFrmImm16 - This is used for CALL FAR instructions, which have two
    /// immediates, the first of which is a 16 or 32-bit immediate (specified by
    /// the imm encoding) and the second is a 16-bit fixed value.  In the AMD
    /// manual, this operand is described as pntr16:32 and pntr16:16
    RawFrmImm16 = 12,

    /// MRMX[rm] - The forms are used to represent instructions that use a
    /// Mod/RM byte, and don't use the middle field for anything.
    MRMXr = 14, MRMXm = 15,

    /// MRM[0-7][rm] - These forms are used to represent instructions that use
    /// a Mod/RM byte, and use the middle field to hold extended opcode
    /// information.  In the intel manual these are represented as /0, /1, ...
    ///

    // First, instructions that operate on a register r/m operand...
    MRM0r = 16,  MRM1r = 17,  MRM2r = 18,  MRM3r = 19, // Format /0 /1 /2 /3
    MRM4r = 20,  MRM5r = 21,  MRM6r = 22,  MRM7r = 23, // Format /4 /5 /6 /7

    // Next, instructions that operate on a memory r/m operand...
    MRM0m = 24,  MRM1m = 25,  MRM2m = 26,  MRM3m = 27, // Format /0 /1 /2 /3
    MRM4m = 28,  MRM5m = 29,  MRM6m = 30,  MRM7m = 31, // Format /4 /5 /6 /7

    //// MRM_XX - A mod/rm byte of exactly 0xXX.
    MRM_C0 = 32, MRM_C1 = 33, MRM_C2 = 34, MRM_C3 = 35,
    MRM_C4 = 36, MRM_C8 = 37, MRM_C9 = 38, MRM_CA = 39,
    MRM_CB = 40, MRM_D0 = 41, MRM_D1 = 42, MRM_D4 = 43,
    MRM_D5 = 44, MRM_D6 = 45, MRM_D8 = 46, MRM_D9 = 47,
    MRM_DA = 48, MRM_DB = 49, MRM_DC = 50, MRM_DD = 51,
    MRM_DE = 52, MRM_DF = 53, MRM_E0 = 54, MRM_E1 = 55,
    MRM_E2 = 56, MRM_E3 = 57, MRM_E4 = 58, MRM_E5 = 59,
    MRM_E8 = 60, MRM_E9 = 61, MRM_EA = 62, MRM_EB = 63,
    MRM_EC = 64, MRM_ED = 65, MRM_EE = 66, MRM_F0 = 67,
    MRM_F1 = 68, MRM_F2 = 69, MRM_F3 = 70, MRM_F4 = 71,
    MRM_F5 = 72, MRM_F6 = 73, MRM_F7 = 74, MRM_F8 = 75,
    MRM_F9 = 76, MRM_FA = 77, MRM_FB = 78, MRM_FC = 79,
    MRM_FD = 80, MRM_FE = 81, MRM_FF = 82,

    FormMask       = 127,

    //===------------------------------------------------------------------===//
    // Actual flags...

    // OpSize - OpSizeFixed implies instruction never needs a 0x66 prefix.
    // OpSize16 means this is a 16-bit instruction and needs 0x66 prefix in
    // 32-bit mode. OpSize32 means this is a 32-bit instruction needs a 0x66
    // prefix in 16-bit mode.
    OpSizeShift = 7,
    OpSizeMask = 0x3 << OpSizeShift,

    OpSize16 = 1,
    OpSize32 = 2,

    // AsSize - Set if this instruction requires an operand size prefix (0x67),
    // which most often indicates that the instruction address 16 bit address
    // instead of 32 bit address (or 32 bit address in 64 bit mode).
    AdSizeShift = OpSizeShift + 2,
    AdSize      = 1 << AdSizeShift,

    //===------------------------------------------------------------------===//
    // OpPrefix - There are several prefix bytes that are used as opcode
    // extensions. These are 0x66, 0xF3, and 0xF2. If this field is 0 there is
    // no prefix.
    //
    OpPrefixShift = AdSizeShift + 1,
    OpPrefixMask  = 0x7 << OpPrefixShift,

    // PS, PD - Prefix code for packed single and double precision vector
    // floating point operations performed in the SSE registers.
    PS = 1 << OpPrefixShift, PD = 2 << OpPrefixShift,

    // XS, XD - These prefix codes are for single and double precision scalar
    // floating point operations performed in the SSE registers.
    XS = 3 << OpPrefixShift,  XD = 4 << OpPrefixShift,

    //===------------------------------------------------------------------===//
    // OpMap - This field determines which opcode map this instruction
    // belongs to. i.e. one-byte, two-byte, 0x0f 0x38, 0x0f 0x3a, etc.
    //
    OpMapShift = OpPrefixShift + 3,
    OpMapMask  = 0x7 << OpMapShift,

    // OB - OneByte - Set if this instruction has a one byte opcode.
    OB = 0 << OpMapShift,

    // TB - TwoByte - Set if this instruction has a two byte opcode, which
    // starts with a 0x0F byte before the real opcode.
    TB = 1 << OpMapShift,

    // T8, TA - Prefix after the 0x0F prefix.
    T8 = 2 << OpMapShift,  TA = 3 << OpMapShift,

    // XOP8 - Prefix to include use of imm byte.
    XOP8 = 4 << OpMapShift,

    // XOP9 - Prefix to exclude use of imm byte.
    XOP9 = 5 << OpMapShift,

    // XOPA - Prefix to encode 0xA in VEX.MMMM of XOP instructions.
    XOPA = 6 << OpMapShift,

    //===------------------------------------------------------------------===//
    // REX_W - REX prefixes are instruction prefixes used in 64-bit mode.
    // They are used to specify GPRs and SSE registers, 64-bit operand size,
    // etc. We only cares about REX.W and REX.R bits and only the former is
    // statically determined.
    //
    REXShift    = OpMapShift + 3,
    REX_W       = 1 << REXShift,

    //===------------------------------------------------------------------===//
    // This three-bit field describes the size of an immediate operand.  Zero is
    // unused so that we can tell if we forgot to set a value.
    ImmShift = REXShift + 1,
    ImmMask    = 15 << ImmShift,
    Imm8       = 1 << ImmShift,
    Imm8PCRel  = 2 << ImmShift,
    Imm16      = 3 << ImmShift,
    Imm16PCRel = 4 << ImmShift,
    Imm32      = 5 << ImmShift,
    Imm32PCRel = 6 << ImmShift,
    Imm32S     = 7 << ImmShift,
    Imm64      = 8 << ImmShift,

    //===------------------------------------------------------------------===//
    // FP Instruction Classification...  Zero is non-fp instruction.

    // FPTypeMask - Mask for all of the FP types...
    FPTypeShift = ImmShift + 4,
    FPTypeMask  = 7 << FPTypeShift,

    // NotFP - The default, set for instructions that do not use FP registers.
    NotFP      = 0 << FPTypeShift,

    // ZeroArgFP - 0 arg FP instruction which implicitly pushes ST(0), f.e. fld0
    ZeroArgFP  = 1 << FPTypeShift,

    // OneArgFP - 1 arg FP instructions which implicitly read ST(0), such as fst
    OneArgFP   = 2 << FPTypeShift,

    // OneArgFPRW - 1 arg FP instruction which implicitly read ST(0) and write a
    // result back to ST(0).  For example, fcos, fsqrt, etc.
    //
    OneArgFPRW = 3 << FPTypeShift,

    // TwoArgFP - 2 arg FP instructions which implicitly read ST(0), and an
    // explicit argument, storing the result to either ST(0) or the implicit
    // argument.  For example: fadd, fsub, fmul, etc...
    TwoArgFP   = 4 << FPTypeShift,

    // CompareFP - 2 arg FP instructions which implicitly read ST(0) and an
    // explicit argument, but have no destination.  Example: fucom, fucomi, ...
    CompareFP  = 5 << FPTypeShift,

    // CondMovFP - "2 operand" floating point conditional move instructions.
    CondMovFP  = 6 << FPTypeShift,

    // SpecialFP - Special instruction forms.  Dispatch by opcode explicitly.
    SpecialFP  = 7 << FPTypeShift,

    // Lock prefix
    LOCKShift = FPTypeShift + 3,
    LOCK = 1 << LOCKShift,

    // REP prefix
    REPShift = LOCKShift + 1,
    REP = 1 << REPShift,

    // Execution domain for SSE instructions.
    // 0 means normal, non-SSE instruction.
    SSEDomainShift = REPShift + 1,

    // Encoding
    EncodingShift = SSEDomainShift + 2,
    EncodingMask = 0x3 << EncodingShift,

    // VEX - encoding using 0xC4/0xC5
    VEX = 1,

    /// XOP - Opcode prefix used by XOP instructions.
    XOP = 2,

    // VEX_EVEX - Specifies that this instruction use EVEX form which provides
    // syntax support up to 32 512-bit register operands and up to 7 16-bit
    // mask operands as well as source operand data swizzling/memory operand
    // conversion, eviction hint, and rounding mode.
    EVEX = 3,

    // Opcode
    OpcodeShift   = EncodingShift + 2,

    //===------------------------------------------------------------------===//
    /// VEX - The opcode prefix used by AVX instructions
    VEXShift = OpcodeShift + 8,

    /// VEX_W - Has a opcode specific functionality, but is used in the same
    /// way as REX_W is for regular SSE instructions.
    VEX_W       = 1U << 0,

    /// VEX_4V - Used to specify an additional AVX/SSE register. Several 2
    /// address instructions in SSE are represented as 3 address ones in AVX
    /// and the additional register is encoded in VEX_VVVV prefix.
    VEX_4V      = 1U << 1,

    /// VEX_4VOp3 - Similar to VEX_4V, but used on instructions that encode
    /// operand 3 with VEX.vvvv.
    VEX_4VOp3   = 1U << 2,

    /// VEX_I8IMM - Specifies that the last register used in a AVX instruction,
    /// must be encoded in the i8 immediate field. This usually happens in
    /// instructions with 4 operands.
    VEX_I8IMM   = 1U << 3,

    /// VEX_L - Stands for a bit in the VEX opcode prefix meaning the current
    /// instruction uses 256-bit wide registers. This is usually auto detected
    /// if a VR256 register is used, but some AVX instructions also have this
    /// field marked when using a f256 memory references.
    VEX_L       = 1U << 4,

    // VEX_LIG - Specifies that this instruction ignores the L-bit in the VEX
    // prefix. Usually used for scalar instructions. Needed by disassembler.
    VEX_LIG     = 1U << 5,

    // TODO: we should combine VEX_L and VEX_LIG together to form a 2-bit field
    // with following encoding:
    // - 00 V128
    // - 01 V256
    // - 10 V512
    // - 11 LIG (but, in insn encoding, leave VEX.L and EVEX.L in zeros.
    // this will save 1 tsflag bit

    // EVEX_K - Set if this instruction requires masking
    EVEX_K      = 1U << 6,

    // EVEX_Z - Set if this instruction has EVEX.Z field set.
    EVEX_Z      = 1U << 7,

    // EVEX_L2 - Set if this instruction has EVEX.L' field set.
    EVEX_L2     = 1U << 8,

    // EVEX_B - Set if this instruction has EVEX.B field set.
    EVEX_B      = 1U << 9,

    // EVEX_CD8E - compressed disp8 form, element-size
    EVEX_CD8EShift = VEXShift + 10,
    EVEX_CD8EMask = 3,

    // EVEX_CD8V - compressed disp8 form, vector-width
    EVEX_CD8VShift = EVEX_CD8EShift + 2,
    EVEX_CD8VMask = 7,

    /// Has3DNow0F0FOpcode - This flag indicates that the instruction uses the
    /// wacky 0x0F 0x0F prefix for 3DNow! instructions.  The manual documents
    /// this as having a 0x0F prefix with a 0x0F opcode, and each instruction
    /// storing a classifier in the imm8 field.  To simplify our implementation,
    /// we handle this by storeing the classifier in the opcode field and using
    /// this flag to indicate that the encoder should do the wacky 3DNow! thing.
    Has3DNow0F0FOpcode = 1U << 15,

    /// MemOp4 - Used to indicate swapping of operand 3 and 4 to be encoded in
    /// ModRM or I8IMM. This is used for FMA4 and XOP instructions.
    MemOp4 = 1U << 16,

    /// Explicitly specified rounding control
    EVEX_RC = 1U << 17
  };

  // getBaseOpcodeFor - This function returns the "base" Cse523 opcode for the
  // specified machine instruction.
  //
  inline unsigned char getBaseOpcodeFor(uint64_t TSFlags) {
    return TSFlags >> Cse523II::OpcodeShift;
  }

  inline bool hasImm(uint64_t TSFlags) {
    return (TSFlags & Cse523II::ImmMask) != 0;
  }

  /// getSizeOfImm - Decode the "size of immediate" field from the TSFlags field
  /// of the specified instruction.
  inline unsigned getSizeOfImm(uint64_t TSFlags) {
    switch (TSFlags & Cse523II::ImmMask) {
    default: llvm_unreachable("Unknown immediate size");
    case Cse523II::Imm8:
    case Cse523II::Imm8PCRel:  return 1;
    case Cse523II::Imm16:
    case Cse523II::Imm16PCRel: return 2;
    case Cse523II::Imm32:
    case Cse523II::Imm32S:
    case Cse523II::Imm32PCRel: return 4;
    case Cse523II::Imm64:      return 8;
    }
  }

  /// isImmPCRel - Return true if the immediate of the specified instruction's
  /// TSFlags indicates that it is pc relative.
  inline unsigned isImmPCRel(uint64_t TSFlags) {
    switch (TSFlags & Cse523II::ImmMask) {
    default: llvm_unreachable("Unknown immediate size");
    case Cse523II::Imm8PCRel:
    case Cse523II::Imm16PCRel:
    case Cse523II::Imm32PCRel:
      return true;
    case Cse523II::Imm8:
    case Cse523II::Imm16:
    case Cse523II::Imm32:
    case Cse523II::Imm32S:
    case Cse523II::Imm64:
      return false;
    }
  }

  /// isImmSigned - Return true if the immediate of the specified instruction's
  /// TSFlags indicates that it is signed.
  inline unsigned isImmSigned(uint64_t TSFlags) {
    switch (TSFlags & Cse523II::ImmMask) {
    default: llvm_unreachable("Unknown immediate signedness");
    case Cse523II::Imm32S:
      return true;
    case Cse523II::Imm8:
    case Cse523II::Imm8PCRel:
    case Cse523II::Imm16:
    case Cse523II::Imm16PCRel:
    case Cse523II::Imm32:
    case Cse523II::Imm32PCRel:
    case Cse523II::Imm64:
      return false;
    }
  }

  /// getOperandBias - compute any additional adjustment needed to
  ///                  the offset to the start of the memory operand
  ///                  in this instruction.
  /// If this is a two-address instruction,skip one of the register operands.
  /// FIXME: This should be handled during MCInst lowering.
  inline int getOperandBias(const MCInstrDesc& Desc)
  {
    unsigned NumOps = Desc.getNumOperands();
    unsigned CurOp = 0;
    if (NumOps > 1 && Desc.getOperandConstraint(1, MCOI::TIED_TO) == 0)
      ++CurOp;
    else if (NumOps > 3 && Desc.getOperandConstraint(2, MCOI::TIED_TO) == 0 &&
             Desc.getOperandConstraint(3, MCOI::TIED_TO) == 1)
      // Special case for AVX-512 GATHER with 2 TIED_TO operands
      // Skip the first 2 operands: dst, mask_wb
      CurOp += 2;
    else if (NumOps > 3 && Desc.getOperandConstraint(2, MCOI::TIED_TO) == 0 &&
             Desc.getOperandConstraint(NumOps - 1, MCOI::TIED_TO) == 1)
      // Special case for GATHER with 2 TIED_TO operands
      // Skip the first 2 operands: dst, mask_wb
      CurOp += 2;
    else if (NumOps > 2 && Desc.getOperandConstraint(NumOps - 2, MCOI::TIED_TO) == 0)
      // SCATTER
      ++CurOp;
    return CurOp;
  }

  /// getMemoryOperandNo - The function returns the MCInst operand # for the
  /// first field of the memory operand.  If the instruction doesn't have a
  /// memory operand, this returns -1.
  ///
  /// Note that this ignores tied operands.  If there is a tied register which
  /// is duplicated in the MCInst (e.g. "EAX = addl EAX, [mem]") it is only
  /// counted as one operand.
  ///
  inline int getMemoryOperandNo(uint64_t TSFlags, unsigned Opcode) {
    switch (TSFlags & Cse523II::FormMask) {
    default: llvm_unreachable("Unknown FormMask value in getMemoryOperandNo!");
    case Cse523II::Pseudo:
    case Cse523II::RawFrm:
    case Cse523II::AddRegFrm:
    case Cse523II::MRMDestReg:
    case Cse523II::MRMSrcReg:
    case Cse523II::RawFrmImm8:
    case Cse523II::RawFrmImm16:
    case Cse523II::RawFrmMemOffs:
    case Cse523II::RawFrmSrc:
    case Cse523II::RawFrmDst:
    case Cse523II::RawFrmDstSrc:
       return -1;
    case Cse523II::MRMDestMem:
      return 0;
    case Cse523II::MRMSrcMem: {
      bool HasVEX_4V = (TSFlags >> Cse523II::VEXShift) & Cse523II::VEX_4V;
      bool HasMemOp4 = (TSFlags >> Cse523II::VEXShift) & Cse523II::MemOp4;
      bool HasEVEX_K = ((TSFlags >> Cse523II::VEXShift) & Cse523II::EVEX_K);
      unsigned FirstMemOp = 1;
      if (HasVEX_4V)
        ++FirstMemOp;// Skip the register source (which is encoded in VEX_VVVV).
      if (HasMemOp4)
        ++FirstMemOp;// Skip the register source (which is encoded in I8IMM).
      if (HasEVEX_K)
        ++FirstMemOp;// Skip the mask register
      // FIXME: Maybe lea should have its own form?  This is a horrible hack.
      //if (Opcode == Cse523::LEA64r || Opcode == Cse523::LEA64_32r ||
      //    Opcode == Cse523::LEA16r || Opcode == Cse523::LEA32r)
      return FirstMemOp;
    }
    case Cse523II::MRMXr:
    case Cse523II::MRM0r: case Cse523II::MRM1r:
    case Cse523II::MRM2r: case Cse523II::MRM3r:
    case Cse523II::MRM4r: case Cse523II::MRM5r:
    case Cse523II::MRM6r: case Cse523II::MRM7r:
      return -1;
    case Cse523II::MRMXm:
    case Cse523II::MRM0m: case Cse523II::MRM1m:
    case Cse523II::MRM2m: case Cse523II::MRM3m:
    case Cse523II::MRM4m: case Cse523II::MRM5m:
    case Cse523II::MRM6m: case Cse523II::MRM7m: {
      bool HasVEX_4V = (TSFlags >> Cse523II::VEXShift) & Cse523II::VEX_4V;
      unsigned FirstMemOp = 0;
      if (HasVEX_4V)
        ++FirstMemOp;// Skip the register dest (which is encoded in VEX_VVVV).
      return FirstMemOp;
    }
    case Cse523II::MRM_C0: case Cse523II::MRM_C1: case Cse523II::MRM_C2:
    case Cse523II::MRM_C3: case Cse523II::MRM_C4: case Cse523II::MRM_C8:
    case Cse523II::MRM_C9: case Cse523II::MRM_CA: case Cse523II::MRM_CB:
    case Cse523II::MRM_D0: case Cse523II::MRM_D1: case Cse523II::MRM_D4:
    case Cse523II::MRM_D5: case Cse523II::MRM_D6: case Cse523II::MRM_D8:
    case Cse523II::MRM_D9: case Cse523II::MRM_DA: case Cse523II::MRM_DB:
    case Cse523II::MRM_DC: case Cse523II::MRM_DD: case Cse523II::MRM_DE:
    case Cse523II::MRM_DF: case Cse523II::MRM_E0: case Cse523II::MRM_E1:
    case Cse523II::MRM_E2: case Cse523II::MRM_E3: case Cse523II::MRM_E4:
    case Cse523II::MRM_E5: case Cse523II::MRM_E8: case Cse523II::MRM_E9:
    case Cse523II::MRM_EA: case Cse523II::MRM_EB: case Cse523II::MRM_EC:
    case Cse523II::MRM_ED: case Cse523II::MRM_EE: case Cse523II::MRM_F0:
    case Cse523II::MRM_F1: case Cse523II::MRM_F2: case Cse523II::MRM_F3:
    case Cse523II::MRM_F4: case Cse523II::MRM_F5: case Cse523II::MRM_F6:
    case Cse523II::MRM_F7: case Cse523II::MRM_F8: case Cse523II::MRM_F9:
    case Cse523II::MRM_FA: case Cse523II::MRM_FB: case Cse523II::MRM_FC:
    case Cse523II::MRM_FD: case Cse523II::MRM_FE: case Cse523II::MRM_FF:
      return -1;
    }
  }

  /// isCse523ExtendedReg - Is the MachineOperand a cse523-64 extended (r8 or
  /// higher) register?  e.g. r8, xmm8, xmm13, etc.
  inline bool isCse523ExtendedReg(unsigned RegNo) {

    switch (RegNo) {
        default: break;
        case Cse523::R8:    case Cse523::R9:    case Cse523::R10:   case Cse523::R11:
        case Cse523::R12:   case Cse523::R13:   case Cse523::R14:   case Cse523::R15:
        case Cse523::CR8:   case Cse523::CR9:   case Cse523::CR10:  case Cse523::CR11:
        case Cse523::CR12:  case Cse523::CR13:  case Cse523::CR14:  case Cse523::CR15:
            return true;
    }
    return false;
  }

  /// is32ExtendedReg - Is the MemoryOperand a 32 extended (zmm16 or higher)
  /// registers? e.g. zmm21, etc.
  static inline bool is32ExtendedReg(unsigned RegNo) {
    return false;
  }
  
  inline bool isCse523NonExtLowByteReg(unsigned reg) {
    return false;
           
  }
}

} // end namespace llvm;

#endif