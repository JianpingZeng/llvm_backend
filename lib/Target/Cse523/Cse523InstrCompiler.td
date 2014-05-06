//===- Cse523InstrCompiler.td - Compiler Pseudos and Patterns -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the various pseudo instructions used by the compiler,
// as well as Pat patterns used during instruction selection.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Pattern Matching Support

def GetLo32XForm : SDNodeXForm<imm, [{
    // Transformation function: get the low 32 bits.
    return getI32Imm((unsigned)N->getZExtValue());
}]>;

def GetLo8XForm : SDNodeXForm<imm, [{
    // Transformation function: get the low 8 bits.
    return getI8Imm((uint8_t)N->getZExtValue());
}]>;


//===----------------------------------------------------------------------===//
// Random Pseudo Instructions.

// ADJCALLSTACKDOWN/UP implicitly use/def RSP because they may be expanded into
// a stack adjustment and the codegen must know that they may modify the stack
// pointer before prolog-epilog rewriting occurs.
// Pessimistically assume ADJCALLSTACKDOWN / ADJCALLSTACKUP will become
// sub / add which can clobber EFLAGS.
let Defs = [RSP, EFLAGS], Uses = [RSP] in {
    def ADJCALLSTACKDOWN64 : I<0, Pseudo, (outs), (ins i32imm:$amt),
        "#ADJCALLSTACKDOWN",
        [(Cse523callseq_start timm:$amt)]>,
        Requires<[In64BitMode]>;
    def ADJCALLSTACKUP64   : I<0, Pseudo, (outs), (ins i32imm:$amt1, i32imm:$amt2),
        "#ADJCALLSTACKUP",
        [(Cse523callseq_end timm:$amt1, timm:$amt2)]>,
        Requires<[In64BitMode]>;
}

// cse523-64 va_start lowering magic.
let usesCustomInserter = 1, Defs = [EFLAGS] in {
//    def VASTART_SAVE_XMM_REGS : I<0, Pseudo,
//        (outs),
//        (ins GR8:$al,
//         i64imm:$regsavefi, i64imm:$offset,
//         variable_ops),
//        "#VASTART_SAVE_XMM_REGS $al, $regsavefi, $offset",
//        [(Cse523vastart_save_xmm_regs GR8:$al,
//                imm:$regsavefi,
//                imm:$offset),
//        (implicit EFLAGS)]>;

    // The VAARG_64 pseudo-instruction takes the address of the va_list,
    // and places the address of the next argument into a register.
    let Defs = [EFLAGS] in
        def VAARG_64 : I<0, Pseudo,
            (outs GR64:$dst),
            (ins i8mem:$ap, i32imm:$size, i8imm:$mode, i32imm:$align),
            "#VAARG_64 $dst, $ap, $size, $mode, $align",
            [(set GR64:$dst,
                    (Cse523vaarg64 addr:$ap, imm:$size, imm:$mode, imm:$align)),
            (implicit EFLAGS)]>;

    // When using segmented stacks these are lowered into instructions which first
    // check if the current stacklet has enough free memory. If it does, memory is
    // allocated by bumping the stack pointer. Otherwise memory is allocated from 
    // the heap.

    let Defs = [RAX, RSP, EFLAGS], Uses = [RSP] in
        def SEG_ALLOCA_64 : I<0, Pseudo, (outs GR64:$dst), (ins GR64:$size),
            "# variable sized alloca for segmented stacks",
            [(set GR64:$dst,
                    (Cse523SegAlloca GR64:$size))]>,
            Requires<[In64BitMode]>;
}

// The MSVC runtime contains an _ftol2 routine for converting floating-point
// to integer values. It has a strange calling convention: the input is
// popped from the x87 stack, and the return value is given in EDX:EAX. ECX is
// used as a temporary register. No other registers (aside from flags) are
// touched.
// Microsoft toolchains do not support 80-bit precision, so a WIN_FTOL_80
// variant is unnecessary.

//===----------------------------------------------------------------------===//
// EH Pseudo Instructions
//
let SchedRW = [WriteSystem] in {

    let isTerminator = 1, isReturn = 1, isBarrier = 1,
        hasCtrlDep = 1, isCodeGenOnly = 1 in {
            def EH_RETURN64   : I<0xC3, RawFrm, (outs), (ins GR64:$addr),
            "ret\t#eh_return, addr: $addr",
            [(Cse523ehret GR64:$addr)], IIC_RET>, Sched<[WriteJumpLd]>;

        }

    let hasSideEffects = 1, isBarrier = 1, isCodeGenOnly = 1,
        usesCustomInserter = 1 in {
            let isTerminator = 1 in {
                def EH_SjLj_LongJmp32 : I<0, Pseudo, (outs), (ins i32mem:$buf),
                "#EH_SJLJ_LONGJMP32",
                [(Cse523eh_sjlj_longjmp addr:$buf)]>,
                Requires<[Not64BitMode]>;
                def EH_SjLj_LongJmp64 : I<0, Pseudo, (outs), (ins i64mem:$buf),
                    "#EH_SJLJ_LONGJMP64",
                    [(Cse523eh_sjlj_longjmp addr:$buf)]>,
                    Requires<[In64BitMode]>;
            }
        }

} // SchedRW

let isBranch = 1, isTerminator = 1, isCodeGenOnly = 1 in {
    def EH_SjLj_Setup : I<0, Pseudo, (outs), (ins brtarget:$dst),
        "#EH_SjLj_Setup\t$dst", []>;
}

//===----------------------------------------------------------------------===//
// Pseudo instructions used by segmented stacks.
//

// This is lowered into a RET instruction by MCInstLower.  We need
// this so that we don't have to have a MachineBasicBlock which ends
// with a RET and also has successors.
let isPseudo = 1 in {
    def MORESTACK_RET: I<0, Pseudo, (outs), (ins),
        "", []>;

    // This instruction is lowered to a RET followed by a MOV.  The two
    // instructions are not generated on a higher level since then the
    // verifier sees a MachineBasicBlock ending with a non-terminator.
    def MORESTACK_RET_RESTORE_R10 : I<0, Pseudo, (outs), (ins),
        "", []>;
}

//===----------------------------------------------------------------------===//
// Alias Instructions
//===----------------------------------------------------------------------===//

// Alias instruction mapping movr0 to xor.
// FIXME: remove when we can teach regalloc that xor reg, reg is ok.
let Defs = [EFLAGS], isReMaterializable = 1, isAsCheapAsAMove = 1,
    isPseudo = 1 in
    def MOV64r0  : I<0, Pseudo, (outs GR64:$dst), (ins), "",
    [(set GR64:$dst, 0)], IIC_ALU_NONMEM>, Sched<[WriteZero]>;
//let Defs = [EFLAGS], isReMaterializable = 1, isAsCheapAsAMove = 1,
//    isPseudo = 1 in
//    def MOV32r0  : I<0, Pseudo, (outs GR32:$dst), (ins), "",
//    [(set GR32:$dst, 0)], IIC_ALU_NONMEM>, Sched<[WriteZero]>;
//
//// Other widths can also make use of the 32-bit xor, which may have a smaller
//// encoding and avoid partial register updates.
//def : Pat<(i8 0), (EXTRACT_SUBREG (MOV32r0), sub_8bit)>;
//def : Pat<(i16 0), (EXTRACT_SUBREG (MOV32r0), sub_16bit)>;
//def : Pat<(i64 0), (SUBREG_TO_REG (i64 0), (MOV64r0), sub_64bit)> {
//    let AddedComplexity = 20;
//}

// Materialize i64 constant where top 32-bits are zero. This could theoretically
// use MOV32ri with a SUBREG_TO_REG to represent the zero-extension, however
// that would make it more difficult to rematerialize.
let AddedComplexity = 1, isReMaterializable = 1, isAsCheapAsAMove = 1,
    isCodeGenOnly = 1, neverHasSideEffects = 1 in
    def MOV64ri64 : Ii64<0xb8, AddRegFrm, (outs GR64:$dst), (ins i64imm:$src),
    "", [], IIC_ALU_NONMEM>, Sched<[WriteALU]>;

// This 64-bit pseudo-move can be used for both a 64-bit constant that is
// actually the zero-extension of a 32-bit constant, and for labels in the
// cse523-64 small code model.
//def mov64imm32 : ComplexPattern<i64, 1, "SelectMOV64Imm32", [imm, Cse523Wrapper]>;

//let AddedComplexity = 1 in
//def : Pat<(i64 mov64imm32:$src),
//    (SUBREG_TO_REG (i64 0), (MOV64ri64 mov64imm32:$src), sub_64bit)>;

// Use sbb to materialize carry bit.
let Uses = [EFLAGS], Defs = [EFLAGS], isPseudo = 1, SchedRW = [WriteALU] in {
    // FIXME: These are pseudo ops that should be replaced with Pat<> patterns.
    // However, Pat<> can't replicate the destination reg into the inputs of the
    // result.
    def SETB_C64r : I<0, Pseudo, (outs GR64:$dst), (ins), "",
        [(set GR64:$dst, (Cse523setcc_c Cse523_COND_B, EFLAGS))]>;
} // isCodeGenOnly
  
  
def : Pat<(i64 (anyext (i8 (Cse523setcc_c Cse523_COND_B, EFLAGS)))),
    (SETB_C64r)>;

def : Pat<(i64 (sext (i8 (Cse523setcc_c Cse523_COND_B, EFLAGS)))),
    (SETB_C64r)>;

//// We canonicalize 'setb' to "(and (sbb reg,reg), 1)" on the hope that the and
//// will be eliminated and that the sbb can be extended up to a wider type.  When
//// this happens, it is great.  However, if we are left with an 8-bit sbb and an
//// and, we might as well just match it as a setb.
//def : Pat<(and (i8 (Cse523setcc_c Cse523_COND_B, EFLAGS)), 1),
//    (SETBr)>;
//
//// (add OP, SETB) -> (adc OP, 0)
//def : Pat<(add (and (i64 (Cse523setcc_c Cse523_COND_B, EFLAGS)), 1), GR64:$op),
//    (ADC64ri8 GR64:$op, 0)>;
//
//// (sub OP, SETB) -> (sbb OP, 0)
//def : Pat<(sub GR64:$op, (and (i64 (Cse523setcc_c Cse523_COND_B, EFLAGS)), 1)),
//    (SBB64ri8 GR64:$op, 0)>;
//
//// (sub OP, SETCC_CARRY) -> (adc OP, 0)
//def : Pat<(sub GR64:$op, (i64 (Cse523setcc_c Cse523_COND_B, EFLAGS))),
//    (ADC64ri8 GR64:$op, 0)>;
//
//===----------------------------------------------------------------------===//
// String Pseudo Instructions
//
let SchedRW = [WriteMicrocoded] in {
    let Defs = [RCX,RDI,RSI], Uses = [RCX,RDI,RSI], isCodeGenOnly = 1 in {
        def REP_MOVSB_64 : I<0xA4, RawFrm, (outs), (ins), "{rep;movsb|rep movsb}",
            [(Cse523rep_movs i8)], IIC_REP_MOVS>, REP,
            Requires<[In64BitMode]>;
        def REP_MOVSW_64 : I<0xA5, RawFrm, (outs), (ins), "{rep;movsw|rep movsw}",
            [(Cse523rep_movs i16)], IIC_REP_MOVS>, REP, OpSize16,
            Requires<[In64BitMode]>;
        def REP_MOVSD_64 : I<0xA5, RawFrm, (outs), (ins), "{rep;movsl|rep movsd}",
            [(Cse523rep_movs i32)], IIC_REP_MOVS>, REP, OpSize32,
            Requires<[In64BitMode]>;
        def REP_MOVSQ_64 : RI<0xA5, RawFrm, (outs), (ins), "{rep;movsq|rep movsq}",
            [(Cse523rep_movs i64)], IIC_REP_MOVS>, REP,
            Requires<[In64BitMode]>;
    }

    let Defs = [RCX,RDI], isCodeGenOnly = 1 in {
        let Uses = [RAX,RCX,RDI] in
            def REP_STOSD_64 : I<0xAB, RawFrm, (outs), (ins), "{rep;stosl|rep stosd}",
                [(Cse523rep_stos i32)], IIC_REP_STOS>, REP, OpSize32,
                Requires<[In64BitMode]>;

        let Uses = [RAX,RCX,RDI] in
            def REP_STOSQ_64 : RI<0xAB, RawFrm, (outs), (ins), "{rep;stosq|rep stosq}",
                [(Cse523rep_stos i64)], IIC_REP_STOS>, REP,
                Requires<[In64BitMode]>;
    }
} // SchedRW

//===----------------------------------------------------------------------===//
// Thread Local Storage Instructions
//

// ELF TLS Support

// All calls clobber the non-callee saved registers. RSP is marked as
// a use to prevent stack-pointer assignments that appear immediately
// before calls from potentially appearing dead.
let Defs = [RAX, RCX, RDX, RSI, RDI, R8, R9, R10, R11, EFLAGS],
    Uses = [RSP] in {
        def TLS_addr64 : I<0, Pseudo, (outs), (ins i64mem:$sym),
        "# TLS_addr64",
        [(Cse523tlsaddr tls64addr:$sym)]>,
        Requires<[In64BitMode]>;
        def TLS_base_addr64 : I<0, Pseudo, (outs), (ins i64mem:$sym),
            "# TLS_base_addr64",
            [(Cse523tlsbaseaddr tls64baseaddr:$sym)]>,
            Requires<[In64BitMode]>;
    }

// Darwin TLS Support

// For cse523_64, the address of the thunk is passed in %rdi, on return
// the address of the variable is in %rax.  All other registers are preserved.
let Defs = [RAX, EFLAGS],
    Uses = [RSP, RDI],
    usesCustomInserter = 1 in
    def TLSCall_64 : I<0, Pseudo, (outs), (ins i64mem:$sym),
    "# TLSCall_64",
    [(Cse523TLSCall addr:$sym)]>,
    Requires<[In64BitMode]>;


////===----------------------------------------------------------------------===//
//// Conditional Move Pseudo Instruction
//
let usesCustomInserter = 1, Uses = [EFLAGS] in {
    def CMOV_GR64 : I<0, Pseudo,
        (outs GR64:$dst), (ins GR64:$src1, GR64:$src2, i8imm:$cond),
        "#CMOV_GR64 PSEUDO!",
        [(set GR64:$dst, (Cse523cmov GR64:$src1, GR64:$src2,
                    imm:$cond, EFLAGS))]>;

} // UsesCustomInserter = 1, Uses = [EFLAGS]

let usesCustomInserter = 1, Uses = [EFLAGS] in {
    def SETCC_GR64 : I<0, Pseudo,
        (outs GR64:$dst), (ins i8imm:$cond),
        "#SETCC_GR64 PSEUDO!",
        [(set GR64:$dst, (Cse523setcc imm:$cond, EFLAGS))]>;

} // UsesCustomInserter = 1, Uses = [EFLAGS]

////===----------------------------------------------------------------------===//
//// Atomic Instruction Pseudo Instructions
////===----------------------------------------------------------------------===//
//
//// Pseudo atomic instructions
//
//multiclass PSEUDO_ATOMIC_LOAD_BINOP<string mnemonic> {
//    let usesCustomInserter = 1, mayLoad = 1, mayStore = 1 in {
//        let Defs = [EFLAGS, AL] in
//            def NAME#8  : I<0, Pseudo, (outs GR8:$dst),
//                (ins i8mem:$ptr, GR8:$val),
//                !strconcat(mnemonic, "8 PSEUDO!"), []>;
//        let Defs = [EFLAGS, AX] in
//            def NAME#16 : I<0, Pseudo,(outs GR16:$dst),
//                (ins i16mem:$ptr, GR16:$val),
//                !strconcat(mnemonic, "16 PSEUDO!"), []>;
//        let Defs = [EFLAGS, EAX] in
//            def NAME#32 : I<0, Pseudo, (outs GR32:$dst),
//                (ins i32mem:$ptr, GR32:$val),
//                !strconcat(mnemonic, "32 PSEUDO!"), []>;
//        let Defs = [EFLAGS, RAX] in
//            def NAME#64 : I<0, Pseudo, (outs GR64:$dst),
//                (ins i64mem:$ptr, GR64:$val),
//                !strconcat(mnemonic, "64 PSEUDO!"), []>;
//    }
//}
//
//multiclass PSEUDO_ATOMIC_LOAD_BINOP_PATS<string name, string frag> {
//def : Pat<(!cast<PatFrag>(frag # "_8") addr:$ptr, GR8:$val),
//    (!cast<Instruction>(name # "8") addr:$ptr, GR8:$val)>;
//def : Pat<(!cast<PatFrag>(frag # "_16") addr:$ptr, GR16:$val),
//    (!cast<Instruction>(name # "16") addr:$ptr, GR16:$val)>;
//def : Pat<(!cast<PatFrag>(frag # "_32") addr:$ptr, GR32:$val),
//    (!cast<Instruction>(name # "32") addr:$ptr, GR32:$val)>;
//def : Pat<(!cast<PatFrag>(frag # "_64") addr:$ptr, GR64:$val),
//    (!cast<Instruction>(name # "64") addr:$ptr, GR64:$val)>;
//}
//
//// Atomic exchange, and, or, xor
//defm ATOMAND  : PSEUDO_ATOMIC_LOAD_BINOP<"#ATOMAND">;
//defm ATOMOR   : PSEUDO_ATOMIC_LOAD_BINOP<"#ATOMOR">;
//defm ATOMXOR  : PSEUDO_ATOMIC_LOAD_BINOP<"#ATOMXOR">;
//defm ATOMNAND : PSEUDO_ATOMIC_LOAD_BINOP<"#ATOMNAND">;
//defm ATOMMAX  : PSEUDO_ATOMIC_LOAD_BINOP<"#ATOMMAX">;
//defm ATOMMIN  : PSEUDO_ATOMIC_LOAD_BINOP<"#ATOMMIN">;
//defm ATOMUMAX : PSEUDO_ATOMIC_LOAD_BINOP<"#ATOMUMAX">;
//defm ATOMUMIN : PSEUDO_ATOMIC_LOAD_BINOP<"#ATOMUMIN">;
//
//defm : PSEUDO_ATOMIC_LOAD_BINOP_PATS<"ATOMAND",  "atomic_load_and">;
//defm : PSEUDO_ATOMIC_LOAD_BINOP_PATS<"ATOMOR",   "atomic_load_or">;
//defm : PSEUDO_ATOMIC_LOAD_BINOP_PATS<"ATOMXOR",  "atomic_load_xor">;
//defm : PSEUDO_ATOMIC_LOAD_BINOP_PATS<"ATOMNAND", "atomic_load_nand">;
//defm : PSEUDO_ATOMIC_LOAD_BINOP_PATS<"ATOMMAX",  "atomic_load_max">;
//defm : PSEUDO_ATOMIC_LOAD_BINOP_PATS<"ATOMMIN",  "atomic_load_min">;
//defm : PSEUDO_ATOMIC_LOAD_BINOP_PATS<"ATOMUMAX", "atomic_load_umax">;
//defm : PSEUDO_ATOMIC_LOAD_BINOP_PATS<"ATOMUMIN", "atomic_load_umin">;
//
//multiclass PSEUDO_ATOMIC_LOAD_BINOP6432<string mnemonic> {
//    let usesCustomInserter = 1, Defs = [EFLAGS, EAX, EDX],
//        mayLoad = 1, mayStore = 1, hasSideEffects = 0 in
//            def NAME#6432 : I<0, Pseudo, (outs GR32:$dst1, GR32:$dst2),
//        (ins i64mem:$ptr, GR32:$val1, GR32:$val2),
//        !strconcat(mnemonic, "6432 PSEUDO!"), []>;
//}
//
//defm ATOMAND  : PSEUDO_ATOMIC_LOAD_BINOP6432<"#ATOMAND">;
//defm ATOMOR   : PSEUDO_ATOMIC_LOAD_BINOP6432<"#ATOMOR">;
//defm ATOMXOR  : PSEUDO_ATOMIC_LOAD_BINOP6432<"#ATOMXOR">;
//defm ATOMNAND : PSEUDO_ATOMIC_LOAD_BINOP6432<"#ATOMNAND">;
//defm ATOMADD  : PSEUDO_ATOMIC_LOAD_BINOP6432<"#ATOMADD">;
//defm ATOMSUB  : PSEUDO_ATOMIC_LOAD_BINOP6432<"#ATOMSUB">;
//defm ATOMMAX  : PSEUDO_ATOMIC_LOAD_BINOP6432<"#ATOMMAX">;
//defm ATOMMIN  : PSEUDO_ATOMIC_LOAD_BINOP6432<"#ATOMMIN">;
//defm ATOMUMAX : PSEUDO_ATOMIC_LOAD_BINOP6432<"#ATOMUMAX">;
//defm ATOMUMIN : PSEUDO_ATOMIC_LOAD_BINOP6432<"#ATOMUMIN">;
//defm ATOMSWAP : PSEUDO_ATOMIC_LOAD_BINOP6432<"#ATOMSWAP">;
//
////===----------------------------------------------------------------------===//
//// Normal-Instructions-With-Lock-Prefix Pseudo Instructions
////===----------------------------------------------------------------------===//
//
//// FIXME: Use normal instructions and add lock prefix dynamically.
//
//// Memory barriers
//
//// TODO: Get this to fold the constant into the instruction.
//let isCodeGenOnly = 1, Defs = [EFLAGS] in
//def OR32mrLocked  : I<0x09, MRMDestMem, (outs), (ins i32mem:$dst, GR32:$zero),
//    "or{l}\t{$zero, $dst|$dst, $zero}",
//    [], IIC_ALU_MEM>, Requires<[Not64BitMode]>, LOCK,
//    Sched<[WriteALULd, WriteRMW]>;
//
//let hasSideEffects = 1 in
//def Int_MemBarrier : I<0, Pseudo, (outs), (ins),
//    "#MEMBARRIER",
//    [(Cse523MemBarrier)]>, Sched<[WriteLoad]>;
//
//// RegOpc corresponds to the mr version of the instruction
//// ImmOpc corresponds to the mi version of the instruction
//// ImmOpc8 corresponds to the mi8 version of the instruction
//// ImmMod corresponds to the instruction format of the mi and mi8 versions
//multiclass LOCK_ArithBinOp<bits<8> RegOpc, bits<8> ImmOpc, bits<8> ImmOpc8,
//           Format ImmMod, string mnemonic> {
//               let Defs = [EFLAGS], mayLoad = 1, mayStore = 1, isCodeGenOnly = 1,
//               SchedRW = [WriteALULd, WriteRMW] in {
//
//                   def NAME#8mr : I<{RegOpc{7}, RegOpc{6}, RegOpc{5}, RegOpc{4},
//                       RegOpc{3}, RegOpc{2}, RegOpc{1}, 0 },
//                   MRMDestMem, (outs), (ins i8mem:$dst, GR8:$src2),
//                   !strconcat(mnemonic, "{b}\t",
//                           "{$src2, $dst|$dst, $src2}"),
//                   [], IIC_ALU_NONMEM>, LOCK;
//                   def NAME#16mr : I<{RegOpc{7}, RegOpc{6}, RegOpc{5}, RegOpc{4},
//                       RegOpc{3}, RegOpc{2}, RegOpc{1}, 1 },
//                       MRMDestMem, (outs), (ins i16mem:$dst, GR16:$src2),
//                       !strconcat(mnemonic, "{w}\t",
//                               "{$src2, $dst|$dst, $src2}"),
//                       [], IIC_ALU_NONMEM>, OpSize16, LOCK;
//                   def NAME#32mr : I<{RegOpc{7}, RegOpc{6}, RegOpc{5}, RegOpc{4},
//                       RegOpc{3}, RegOpc{2}, RegOpc{1}, 1 },
//                       MRMDestMem, (outs), (ins i32mem:$dst, GR32:$src2),
//                       !strconcat(mnemonic, "{l}\t",
//                               "{$src2, $dst|$dst, $src2}"),
//                       [], IIC_ALU_NONMEM>, OpSize32, LOCK;
//                   def NAME#64mr : RI<{RegOpc{7}, RegOpc{6}, RegOpc{5}, RegOpc{4},
//                       RegOpc{3}, RegOpc{2}, RegOpc{1}, 1 },
//                       MRMDestMem, (outs), (ins i64mem:$dst, GR64:$src2),
//                       !strconcat(mnemonic, "{q}\t",
//                               "{$src2, $dst|$dst, $src2}"),
//                       [], IIC_ALU_NONMEM>, LOCK;
//
//                   def NAME#8mi : Ii8<{ImmOpc{7}, ImmOpc{6}, ImmOpc{5}, ImmOpc{4},
//                       ImmOpc{3}, ImmOpc{2}, ImmOpc{1}, 0 },
//                       ImmMod, (outs), (ins i8mem :$dst, i8imm :$src2),
//                       !strconcat(mnemonic, "{b}\t",
//                               "{$src2, $dst|$dst, $src2}"),
//                       [], IIC_ALU_MEM>, LOCK;
//
//                   def NAME#16mi : Ii16<{ImmOpc{7}, ImmOpc{6}, ImmOpc{5}, ImmOpc{4},
//                       ImmOpc{3}, ImmOpc{2}, ImmOpc{1}, 1 },
//                       ImmMod, (outs), (ins i16mem :$dst, i16imm :$src2),
//                       !strconcat(mnemonic, "{w}\t",
//                               "{$src2, $dst|$dst, $src2}"),
//                       [], IIC_ALU_MEM>, OpSize16, LOCK;
//
//                   def NAME#32mi : Ii32<{ImmOpc{7}, ImmOpc{6}, ImmOpc{5}, ImmOpc{4},
//                       ImmOpc{3}, ImmOpc{2}, ImmOpc{1}, 1 },
//                       ImmMod, (outs), (ins i32mem :$dst, i32imm :$src2),
//                       !strconcat(mnemonic, "{l}\t",
//                               "{$src2, $dst|$dst, $src2}"),
//                       [], IIC_ALU_MEM>, OpSize32, LOCK;
//
//                   def NAME#64mi32 : RIi32<{ImmOpc{7}, ImmOpc{6}, ImmOpc{5}, ImmOpc{4},
//                       ImmOpc{3}, ImmOpc{2}, ImmOpc{1}, 1 },
//                       ImmMod, (outs), (ins i64mem :$dst, i64i32imm :$src2),
//                       !strconcat(mnemonic, "{q}\t",
//                               "{$src2, $dst|$dst, $src2}"),
//                       [], IIC_ALU_MEM>, LOCK;
//
//                   def NAME#16mi8 : Ii8<{ImmOpc8{7}, ImmOpc8{6}, ImmOpc8{5}, ImmOpc8{4},
//                       ImmOpc8{3}, ImmOpc8{2}, ImmOpc8{1}, 1 },
//                       ImmMod, (outs), (ins i16mem :$dst, i16i8imm :$src2),
//                       !strconcat(mnemonic, "{w}\t",
//                               "{$src2, $dst|$dst, $src2}"),
//                       [], IIC_ALU_MEM>, OpSize16, LOCK;
//                   def NAME#32mi8 : Ii8<{ImmOpc8{7}, ImmOpc8{6}, ImmOpc8{5}, ImmOpc8{4},
//                       ImmOpc8{3}, ImmOpc8{2}, ImmOpc8{1}, 1 },
//                       ImmMod, (outs), (ins i32mem :$dst, i32i8imm :$src2),
//                       !strconcat(mnemonic, "{l}\t",
//                               "{$src2, $dst|$dst, $src2}"),
//                       [], IIC_ALU_MEM>, OpSize32, LOCK;
//                   def NAME#64mi8 : RIi8<{ImmOpc8{7}, ImmOpc8{6}, ImmOpc8{5}, ImmOpc8{4},
//                       ImmOpc8{3}, ImmOpc8{2}, ImmOpc8{1}, 1 },
//                       ImmMod, (outs), (ins i64mem :$dst, i64i8imm :$src2),
//                       !strconcat(mnemonic, "{q}\t",
//                               "{$src2, $dst|$dst, $src2}"),
//                       [], IIC_ALU_MEM>, LOCK;
//
//               }
//
//           }
//
//defm LOCK_ADD : LOCK_ArithBinOp<0x00, 0x80, 0x83, MRM0m, "add">;
//defm LOCK_SUB : LOCK_ArithBinOp<0x28, 0x80, 0x83, MRM5m, "sub">;
//defm LOCK_OR  : LOCK_ArithBinOp<0x08, 0x80, 0x83, MRM1m, "or">;
//defm LOCK_AND : LOCK_ArithBinOp<0x20, 0x80, 0x83, MRM4m, "and">;
//defm LOCK_XOR : LOCK_ArithBinOp<0x30, 0x80, 0x83, MRM6m, "xor">;
//
//// Optimized codegen when the non-memory output is not used.
//multiclass LOCK_ArithUnOp<bits<8> Opc8, bits<8> Opc, Format Form,
//           string mnemonic> {
//               let Defs = [EFLAGS], mayLoad = 1, mayStore = 1, isCodeGenOnly = 1,
//               SchedRW = [WriteALULd, WriteRMW] in {
//
//                   def NAME#8m  : I<Opc8, Form, (outs), (ins i8mem :$dst),
//                   !strconcat(mnemonic, "{b}\t$dst"),
//                   [], IIC_UNARY_MEM>, LOCK;
//                   def NAME#16m : I<Opc, Form, (outs), (ins i16mem:$dst),
//                       !strconcat(mnemonic, "{w}\t$dst"),
//                       [], IIC_UNARY_MEM>, OpSize16, LOCK;
//                   def NAME#32m : I<Opc, Form, (outs), (ins i32mem:$dst),
//                       !strconcat(mnemonic, "{l}\t$dst"),
//                       [], IIC_UNARY_MEM>, OpSize32, LOCK;
//                   def NAME#64m : RI<Opc, Form, (outs), (ins i64mem:$dst),
//                       !strconcat(mnemonic, "{q}\t$dst"),
//                       [], IIC_UNARY_MEM>, LOCK;
//               }
//           }
//
//defm LOCK_INC    : LOCK_ArithUnOp<0xFE, 0xFF, MRM0m, "inc">;
//defm LOCK_DEC    : LOCK_ArithUnOp<0xFE, 0xFF, MRM1m, "dec">;
//
//// Atomic compare and swap.
//multiclass LCMPXCHG_UnOp<bits<8> Opc, Format Form, string mnemonic,
//           SDPatternOperator frag, Cse523MemOperand cse523memop,
//           InstrItinClass itin> {
//               let isCodeGenOnly = 1 in {
//                   def NAME : I<Opc, Form, (outs), (ins cse523memop:$ptr),
//                   !strconcat(mnemonic, "\t$ptr"),
//                   [(frag addr:$ptr)], itin>, TB, LOCK;
//               }
//           }
//
//multiclass LCMPXCHG_BinOp<bits<8> Opc8, bits<8> Opc, Format Form,
//           string mnemonic, SDPatternOperator frag,
//           InstrItinClass itin8, InstrItinClass itin> {
//               let isCodeGenOnly = 1, SchedRW = [WriteALULd, WriteRMW] in {
//                   let Defs = [AL, EFLAGS], Uses = [AL] in
//                       def NAME#8  : I<Opc8, Form, (outs), (ins i8mem:$ptr, GR8:$swap),
//                   !strconcat(mnemonic, "{b}\t{$swap, $ptr|$ptr, $swap}"),
//                   [(frag addr:$ptr, GR8:$swap, 1)], itin8>, TB, LOCK;
//                   let Defs = [AX, EFLAGS], Uses = [AX] in
//                       def NAME#16 : I<Opc, Form, (outs), (ins i16mem:$ptr, GR16:$swap),
//                           !strconcat(mnemonic, "{w}\t{$swap, $ptr|$ptr, $swap}"),
//                           [(frag addr:$ptr, GR16:$swap, 2)], itin>, TB, OpSize16, LOCK;
//                   let Defs = [EAX, EFLAGS], Uses = [EAX] in
//                       def NAME#32 : I<Opc, Form, (outs), (ins i32mem:$ptr, GR32:$swap),
//                           !strconcat(mnemonic, "{l}\t{$swap, $ptr|$ptr, $swap}"),
//                           [(frag addr:$ptr, GR32:$swap, 4)], itin>, TB, OpSize32, LOCK;
//                   let Defs = [RAX, EFLAGS], Uses = [RAX] in
//                       def NAME#64 : RI<Opc, Form, (outs), (ins i64mem:$ptr, GR64:$swap),
//                           !strconcat(mnemonic, "{q}\t{$swap, $ptr|$ptr, $swap}"),
//                           [(frag addr:$ptr, GR64:$swap, 8)], itin>, TB, LOCK;
//               }
//           }
//
//let Defs = [EAX, EDX, EFLAGS], Uses = [EAX, EBX, ECX, EDX],
//    SchedRW = [WriteALULd, WriteRMW] in {
//        defm LCMPXCHG8B : LCMPXCHG_UnOp<0xC7, MRM1m, "cmpxchg8b",
//        Cse523cas8, i64mem,
//        IIC_CMPX_LOCK_8B>;
//    }
//
//let Defs = [RAX, RDX, EFLAGS], Uses = [RAX, RBX, RCX, RDX],
//    Predicates = [HasCmpxchg16b], SchedRW = [WriteALULd, WriteRMW] in {
//        defm LCMPXCHG16B : LCMPXCHG_UnOp<0xC7, MRM1m, "cmpxchg16b",
//        Cse523cas16, i128mem,
//        IIC_CMPX_LOCK_16B>, REX_W;
//    }
//
//defm LCMPXCHG : LCMPXCHG_BinOp<0xB0, 0xB1, MRMDestMem, "cmpxchg",
//     Cse523cas, IIC_CMPX_LOCK_8, IIC_CMPX_LOCK>;
//
//// Atomic exchange and add
//multiclass ATOMIC_LOAD_BINOP<bits<8> opc8, bits<8> opc, string mnemonic,
//           string frag,
//           InstrItinClass itin8, InstrItinClass itin> {
//               let Constraints = "$val = $dst", Defs = [EFLAGS], isCodeGenOnly = 1,
//               SchedRW = [WriteALULd, WriteRMW] in {
//                   def NAME#8  : I<opc8, MRMSrcMem, (outs GR8:$dst),
//                   (ins GR8:$val, i8mem:$ptr),
//                   !strconcat(mnemonic, "{b}\t{$val, $ptr|$ptr, $val}"),
//                   [(set GR8:$dst,
//                           (!cast<PatFrag>(frag # "_8") addr:$ptr, GR8:$val))],
//                   itin8>;
//                   def NAME#16 : I<opc, MRMSrcMem, (outs GR16:$dst),
//                       (ins GR16:$val, i16mem:$ptr),
//                       !strconcat(mnemonic, "{w}\t{$val, $ptr|$ptr, $val}"),
//                       [(set
//                               GR16:$dst,
//                               (!cast<PatFrag>(frag # "_16") addr:$ptr, GR16:$val))],
//                       itin>, OpSize16;
//                   def NAME#32 : I<opc, MRMSrcMem, (outs GR32:$dst),
//                       (ins GR32:$val, i32mem:$ptr),
//                       !strconcat(mnemonic, "{l}\t{$val, $ptr|$ptr, $val}"),
//                       [(set
//                               GR32:$dst,
//                               (!cast<PatFrag>(frag # "_32") addr:$ptr, GR32:$val))],
//                       itin>, OpSize32;
//                   def NAME#64 : RI<opc, MRMSrcMem, (outs GR64:$dst),
//                       (ins GR64:$val, i64mem:$ptr),
//                       !strconcat(mnemonic, "{q}\t{$val, $ptr|$ptr, $val}"),
//                       [(set
//                               GR64:$dst,
//                               (!cast<PatFrag>(frag # "_64") addr:$ptr, GR64:$val))],
//                       itin>;
//               }
//           }
//
//defm LXADD : ATOMIC_LOAD_BINOP<0xc0, 0xc1, "xadd", "atomic_load_add",
//     IIC_XADD_LOCK_MEM8, IIC_XADD_LOCK_MEM>,
//     TB, LOCK;
//
//def ACQUIRE_MOV8rm  : I<0, Pseudo, (outs GR8 :$dst), (ins i8mem :$src),
//    "#ACQUIRE_MOV PSEUDO!",
//    [(set GR8:$dst,  (atomic_load_8  addr:$src))]>;
//def ACQUIRE_MOV16rm : I<0, Pseudo, (outs GR16:$dst), (ins i16mem:$src),
//    "#ACQUIRE_MOV PSEUDO!",
//    [(set GR16:$dst, (atomic_load_16 addr:$src))]>;
//def ACQUIRE_MOV32rm : I<0, Pseudo, (outs GR32:$dst), (ins i32mem:$src),
//    "#ACQUIRE_MOV PSEUDO!",
//    [(set GR32:$dst, (atomic_load_32 addr:$src))]>;
def ACQUIRE_MOV64rm : I<0, Pseudo, (outs GR64:$dst), (ins i64mem:$src),
    "#ACQUIRE_MOV PSEUDO!",
    [(set GR64:$dst, (atomic_load_64 addr:$src))]>;

//def RELEASE_MOV8mr  : I<0, Pseudo, (outs), (ins i8mem :$dst, GR8 :$src),
//    "#RELEASE_MOV PSEUDO!",
//    [(atomic_store_8  addr:$dst, GR8 :$src)]>;
//def RELEASE_MOV16mr : I<0, Pseudo, (outs), (ins i16mem:$dst, GR16:$src),
//    "#RELEASE_MOV PSEUDO!",
//    [(atomic_store_16 addr:$dst, GR16:$src)]>;
//def RELEASE_MOV32mr : I<0, Pseudo, (outs), (ins i32mem:$dst, GR32:$src),
//    "#RELEASE_MOV PSEUDO!",
//    [(atomic_store_32 addr:$dst, GR32:$src)]>;
def RELEASE_MOV64mr : I<0, Pseudo, (outs), (ins i64mem:$dst, GR64:$src),
    "#RELEASE_MOV PSEUDO!",
    [(atomic_store_64 addr:$dst, GR64:$src)]>;

////===----------------------------------------------------------------------===//
//// Conditional Move Pseudo Instructions.
////===----------------------------------------------------------------------===//
//
//
//// CMOV* - Used to implement the SSE SELECT DAG operation.  Expanded after
//// instruction selection into a branch sequence.
//let Uses = [EFLAGS], usesCustomInserter = 1 in {
//    def CMOV_FR32 : I<0, Pseudo,
//        (outs FR32:$dst), (ins FR32:$t, FR32:$f, i8imm:$cond),
//        "#CMOV_FR32 PSEUDO!",
//        [(set FR32:$dst, (Cse523cmov FR32:$t, FR32:$f, imm:$cond,
//                    EFLAGS))]>;
//    def CMOV_FR64 : I<0, Pseudo,
//        (outs FR64:$dst), (ins FR64:$t, FR64:$f, i8imm:$cond),
//        "#CMOV_FR64 PSEUDO!",
//        [(set FR64:$dst, (Cse523cmov FR64:$t, FR64:$f, imm:$cond,
//                    EFLAGS))]>;
//    def CMOV_V4F32 : I<0, Pseudo,
//        (outs VR128:$dst), (ins VR128:$t, VR128:$f, i8imm:$cond),
//        "#CMOV_V4F32 PSEUDO!",
//        [(set VR128:$dst,
//                (v4f32 (Cse523cmov VR128:$t, VR128:$f, imm:$cond,
//                        EFLAGS)))]>;
//    def CMOV_V2F64 : I<0, Pseudo,
//        (outs VR128:$dst), (ins VR128:$t, VR128:$f, i8imm:$cond),
//        "#CMOV_V2F64 PSEUDO!",
//        [(set VR128:$dst,
//                (v2f64 (Cse523cmov VR128:$t, VR128:$f, imm:$cond,
//                        EFLAGS)))]>;
//    def CMOV_V2I64 : I<0, Pseudo,
//        (outs VR128:$dst), (ins VR128:$t, VR128:$f, i8imm:$cond),
//        "#CMOV_V2I64 PSEUDO!",
//        [(set VR128:$dst,
//                (v2i64 (Cse523cmov VR128:$t, VR128:$f, imm:$cond,
//                        EFLAGS)))]>;
//    def CMOV_V8F32 : I<0, Pseudo,
//        (outs VR256:$dst), (ins VR256:$t, VR256:$f, i8imm:$cond),
//        "#CMOV_V8F32 PSEUDO!",
//        [(set VR256:$dst,
//                (v8f32 (Cse523cmov VR256:$t, VR256:$f, imm:$cond,
//                        EFLAGS)))]>;
//    def CMOV_V4F64 : I<0, Pseudo,
//        (outs VR256:$dst), (ins VR256:$t, VR256:$f, i8imm:$cond),
//        "#CMOV_V4F64 PSEUDO!",
//        [(set VR256:$dst,
//                (v4f64 (Cse523cmov VR256:$t, VR256:$f, imm:$cond,
//                        EFLAGS)))]>;
//    def CMOV_V4I64 : I<0, Pseudo,
//        (outs VR256:$dst), (ins VR256:$t, VR256:$f, i8imm:$cond),
//        "#CMOV_V4I64 PSEUDO!",
//        [(set VR256:$dst,
//                (v4i64 (Cse523cmov VR256:$t, VR256:$f, imm:$cond,
//                        EFLAGS)))]>;
//    def CMOV_V8I64 : I<0, Pseudo,
//        (outs VR512:$dst), (ins VR512:$t, VR512:$f, i8imm:$cond),
//        "#CMOV_V8I64 PSEUDO!",
//        [(set VR512:$dst,
//                (v8i64 (Cse523cmov VR512:$t, VR512:$f, imm:$cond,
//                        EFLAGS)))]>;
//    def CMOV_V8F64 : I<0, Pseudo,
//        (outs VR512:$dst), (ins VR512:$t, VR512:$f, i8imm:$cond),
//        "#CMOV_V8F64 PSEUDO!",
//        [(set VR512:$dst,
//                (v8f64 (Cse523cmov VR512:$t, VR512:$f, imm:$cond,
//                        EFLAGS)))]>;
//    def CMOV_V16F32 : I<0, Pseudo,
//        (outs VR512:$dst), (ins VR512:$t, VR512:$f, i8imm:$cond),
//        "#CMOV_V16F32 PSEUDO!",
//        [(set VR512:$dst,
//                (v16f32 (Cse523cmov VR512:$t, VR512:$f, imm:$cond,
//                         EFLAGS)))]>;
//}


////===----------------------------------------------------------------------===//
//// DAG Pattern Matching Rules
////===----------------------------------------------------------------------===//

// ConstantPool GlobalAddress, ExternalSymbol, and JumpTable
def : Pat<(i64 (Cse523Wrapper tconstpool  :$dst)), (MOV64ri tconstpool  :$dst)>;
def : Pat<(i64 (Cse523Wrapper tjumptable  :$dst)), (MOV64ri tjumptable  :$dst)>;
def : Pat<(i64 (Cse523Wrapper tglobaltlsaddr:$dst)),(MOV64ri tglobaltlsaddr:$dst)>;
def : Pat<(i64 (Cse523Wrapper tglobaladdr :$dst)), (MOV64ri tglobaladdr :$dst)>;
def : Pat<(i64 (Cse523Wrapper texternalsym:$dst)), (MOV64ri texternalsym:$dst)>;
def : Pat<(i64 (Cse523Wrapper tblockaddress:$dst)), (MOV64ri tblockaddress:$dst)>;

def : Pat<(add GR64:$src1, (Cse523Wrapper tconstpool:$src2)),
    (ADD64ri32 GR64:$src1, tconstpool:$src2)>;
def : Pat<(add GR64:$src1, (Cse523Wrapper tjumptable:$src2)),
    (ADD64ri32 GR64:$src1, tjumptable:$src2)>;
def : Pat<(add GR64:$src1, (Cse523Wrapper tglobaladdr :$src2)),
    (ADD64ri32 GR64:$src1, tglobaladdr:$src2)>;
def : Pat<(add GR64:$src1, (Cse523Wrapper texternalsym:$src2)),
    (ADD64ri32 GR64:$src1, texternalsym:$src2)>;
def : Pat<(add GR64:$src1, (Cse523Wrapper tblockaddress:$src2)),
    (ADD64ri32 GR64:$src1, tblockaddress:$src2)>;

def : Pat<(store (i64 (Cse523Wrapper tglobaladdr:$src)), addr:$dst),
    (MOV64mi32 addr:$dst, tglobaladdr:$src)>;
def : Pat<(store (i64 (Cse523Wrapper texternalsym:$src)), addr:$dst),
    (MOV64mi32 addr:$dst, texternalsym:$src)>;
def : Pat<(store (i64 (Cse523Wrapper tblockaddress:$src)), addr:$dst),
    (MOV64mi32 addr:$dst, tblockaddress:$src)>;

// ConstantPool GlobalAddress, ExternalSymbol, and JumpTable when not in small
// code model mode, should use 'movabs'.  FIXME: This is really a hack, the
//  'movabs' predicate should handle this sort of thing.
def : Pat<(i64 (Cse523Wrapper tconstpool  :$dst)),
    (MOV64ri tconstpool  :$dst)>, Requires<[FarData]>;
def : Pat<(i64 (Cse523Wrapper tjumptable  :$dst)),
    (MOV64ri tjumptable  :$dst)>, Requires<[FarData]>;
def : Pat<(i64 (Cse523Wrapper tglobaladdr :$dst)),
    (MOV64ri tglobaladdr :$dst)>, Requires<[FarData]>;
def : Pat<(i64 (Cse523Wrapper texternalsym:$dst)),
    (MOV64ri texternalsym:$dst)>, Requires<[FarData]>;
def : Pat<(i64 (Cse523Wrapper tblockaddress:$dst)),
    (MOV64ri tblockaddress:$dst)>, Requires<[FarData]>;

// In kernel code model, we can get the address of a label
// into a register with 'movq'.  FIXME: This is a hack, the 'imm' predicate of
// the MOV64ri32 should accept these.
def : Pat<(i64 (Cse523Wrapper tconstpool  :$dst)),
    (MOV64ri32 tconstpool  :$dst)>, Requires<[KernelCode]>;
def : Pat<(i64 (Cse523Wrapper tjumptable  :$dst)),
    (MOV64ri32 tjumptable  :$dst)>, Requires<[KernelCode]>;
def : Pat<(i64 (Cse523Wrapper tglobaladdr :$dst)),
    (MOV64ri32 tglobaladdr :$dst)>, Requires<[KernelCode]>;
def : Pat<(i64 (Cse523Wrapper texternalsym:$dst)),
    (MOV64ri32 texternalsym:$dst)>, Requires<[KernelCode]>;
def : Pat<(i64 (Cse523Wrapper tblockaddress:$dst)),
    (MOV64ri32 tblockaddress:$dst)>, Requires<[KernelCode]>;

// If we have small model and -static mode, it is safe to store global addresses
// directly as immediates.  FIXME: This is really a hack, the 'imm' predicate
// for MOV64mi32 should handle this sort of thing.
def : Pat<(store (i64 (Cse523Wrapper tconstpool:$src)), addr:$dst),
    (MOV64mi32 addr:$dst, tconstpool:$src)>,
    Requires<[NearData, IsStatic]>;
def : Pat<(store (i64 (Cse523Wrapper tjumptable:$src)), addr:$dst),
    (MOV64mi32 addr:$dst, tjumptable:$src)>,
    Requires<[NearData, IsStatic]>;
def : Pat<(store (i64 (Cse523Wrapper tglobaladdr:$src)), addr:$dst),
    (MOV64mi32 addr:$dst, tglobaladdr:$src)>,
    Requires<[NearData, IsStatic]>;
def : Pat<(store (i64 (Cse523Wrapper texternalsym:$src)), addr:$dst),
    (MOV64mi32 addr:$dst, texternalsym:$src)>,
    Requires<[NearData, IsStatic]>;
def : Pat<(store (i64 (Cse523Wrapper tblockaddress:$src)), addr:$dst),
    (MOV64mi32 addr:$dst, tblockaddress:$src)>,
    Requires<[NearData, IsStatic]>;

// Calls

// tls has some funny stuff here...
// This corresponds to movabs $foo@tpoff, %rax
def : Pat<(i64 (Cse523Wrapper tglobaltlsaddr :$dst)),
    (MOV64ri32 tglobaltlsaddr :$dst)>;
// This corresponds to add $foo@tpoff, %rax
def : Pat<(add GR64:$src1, (Cse523Wrapper tglobaltlsaddr :$dst)),
    (ADD64ri32 GR64:$src1, tglobaltlsaddr :$dst)>;


// Direct PC relative function call for small code model. 32-bit displacement
// sign extended to 64-bit.
def : Pat<(Cse523call (i64 tglobaladdr:$dst)),
    (CALL64pcrel32 tglobaladdr:$dst)>;
def : Pat<(Cse523call (i64 texternalsym:$dst)),
    (CALL64pcrel32 texternalsym:$dst)>;

// Tailcall stuff. The TCRETURN instructions execute after the epilog, so they
// can never use callee-saved registers. That is the purpose of the GR64_TC
// register classes.
//
// The only volatile register that is never used by the calling convention is
// %r11. This happens when calling a vararg function with 6 arguments.
//
// Match an Cse523tcret that uses less than 7 volatile registers.
def Cse523tcret_6regs : PatFrag<(ops node:$ptr, node:$off),
    (Cse523tcret node:$ptr, node:$off), [{
        // Cse523tcret args: (*chain, ptr, imm, regs..., glue)
        unsigned NumRegs = 0;
        for (unsigned i = 3, e = N->getNumOperands(); i != e; ++i)
            if (isa<RegisterSDNode>(N->getOperand(i)) && ++NumRegs > 6)
                return false;
        return true;
    }]>;

def : Pat<(Cse523tcret ptr_rc_tailcall:$dst, imm:$off),
    (TCRETURNri64 ptr_rc_tailcall:$dst, imm:$off)>,
    Requires<[In64BitMode]>;

// Don't fold loads into Cse523tcret requiring more than 6 regs.
// There wouldn't be enough scratch registers for base+index.
def : Pat<(Cse523tcret_6regs (load addr:$dst), imm:$off),
    (TCRETURNmi64 addr:$dst, imm:$off)>,
    Requires<[In64BitMode]>;

def : Pat<(Cse523tcret (i64 tglobaladdr:$dst), imm:$off),
    (TCRETURNdi64 tglobaladdr:$dst, imm:$off)>,
    Requires<[In64BitMode]>;

def : Pat<(Cse523tcret (i64 texternalsym:$dst), imm:$off),
    (TCRETURNdi64 texternalsym:$dst, imm:$off)>,
    Requires<[In64BitMode]>;

// Normal calls, with various flavors of addresses.
//def : Pat<(Cse523call (i32 tglobaladdr:$dst)),
//    (CALLpcrel32 tglobaladdr:$dst)>;
//def : Pat<(Cse523call (i32 texternalsym:$dst)),
//    (CALLpcrel32 texternalsym:$dst)>;
//def : Pat<(Cse523call (i32 imm:$dst)),
//    (CALLpcrel32 imm:$dst)>, Requires<[CallImmAddr]>;

// Comparisons.

// TEST R,R is smaller than CMP R,0
def : Pat<(Cse523cmp GR64:$src1, 0),
    (TEST64rr GR64:$src1, GR64:$src1)>;

//// Conditional moves with folded loads with operands swapped and conditions
//// inverted.
//multiclass CMOVmr<PatLeaf InvertedCond, Instruction Inst16, Instruction Inst32,
//           Instruction Inst64> {
//               let Predicates = [HasCMov] in {
//def : Pat<(Cse523cmov (loadi16 addr:$src1), GR16:$src2, InvertedCond, EFLAGS),
//      (Inst16 GR16:$src2, addr:$src1)>;
//def : Pat<(Cse523cmov (loadi32 addr:$src1), GR32:$src2, InvertedCond, EFLAGS),
//    (Inst32 GR32:$src2, addr:$src1)>;
//def : Pat<(Cse523cmov (loadi64 addr:$src1), GR64:$src2, InvertedCond, EFLAGS),
//    (Inst64 GR64:$src2, addr:$src1)>;
//               }
//           }
//
//defm : CMOVmr<Cse523_COND_B , CMOVAE16rm, CMOVAE32rm, CMOVAE64rm>;
//defm : CMOVmr<Cse523_COND_AE, CMOVB16rm , CMOVB32rm , CMOVB64rm>;
//defm : CMOVmr<Cse523_COND_E , CMOVNE16rm, CMOVNE32rm, CMOVNE64rm>;
//defm : CMOVmr<Cse523_COND_NE, CMOVE16rm , CMOVE32rm , CMOVE64rm>;
//defm : CMOVmr<Cse523_COND_BE, CMOVA16rm , CMOVA32rm , CMOVA64rm>;
//defm : CMOVmr<Cse523_COND_A , CMOVBE16rm, CMOVBE32rm, CMOVBE64rm>;
//defm : CMOVmr<Cse523_COND_L , CMOVGE16rm, CMOVGE32rm, CMOVGE64rm>;
//defm : CMOVmr<Cse523_COND_GE, CMOVL16rm , CMOVL32rm , CMOVL64rm>;
//defm : CMOVmr<Cse523_COND_LE, CMOVG16rm , CMOVG32rm , CMOVG64rm>;
//defm : CMOVmr<Cse523_COND_G , CMOVLE16rm, CMOVLE32rm, CMOVLE64rm>;
//defm : CMOVmr<Cse523_COND_P , CMOVNP16rm, CMOVNP32rm, CMOVNP64rm>;
//defm : CMOVmr<Cse523_COND_NP, CMOVP16rm , CMOVP32rm , CMOVP64rm>;
//defm : CMOVmr<Cse523_COND_S , CMOVNS16rm, CMOVNS32rm, CMOVNS64rm>;
//defm : CMOVmr<Cse523_COND_NS, CMOVS16rm , CMOVS32rm , CMOVS64rm>;
//defm : CMOVmr<Cse523_COND_O , CMOVNO16rm, CMOVNO32rm, CMOVNO64rm>;
//defm : CMOVmr<Cse523_COND_NO, CMOVO16rm , CMOVO32rm , CMOVO64rm>;
//
//// zextload bool -> zextload byte
//def : Pat<(zextloadi8i1  addr:$src), (MOV8rm     addr:$src)>;
//def : Pat<(zextloadi16i1 addr:$src), (MOVZX16rm8 addr:$src)>;
//def : Pat<(zextloadi32i1 addr:$src), (MOVZX32rm8 addr:$src)>;
//def : Pat<(zextloadi64i1 addr:$src),
//    (SUBREG_TO_REG (i64 0), (MOVZX32rm8 addr:$src), sub_32bit)>;
//
//// extload bool -> extload byte
//// When extloading from 16-bit and smaller memory locations into 64-bit
//// registers, use zero-extending loads so that the entire 64-bit register is
//// defined, avoiding partial-register updates.
//
//def : Pat<(extloadi8i1 addr:$src),   (MOV8rm      addr:$src)>;
//def : Pat<(extloadi16i1 addr:$src),  (MOVZX16rm8  addr:$src)>;
//def : Pat<(extloadi32i1 addr:$src),  (MOVZX32rm8  addr:$src)>;
//def : Pat<(extloadi16i8 addr:$src),  (MOVZX16rm8  addr:$src)>;
//def : Pat<(extloadi32i8 addr:$src),  (MOVZX32rm8  addr:$src)>;
//def : Pat<(extloadi32i16 addr:$src), (MOVZX32rm16 addr:$src)>;
//
//// For other extloads, use subregs, since the high contents of the register are
//// defined after an extload.
//def : Pat<(extloadi64i1 addr:$src),
//    (SUBREG_TO_REG (i64 0), (MOVZX32rm8 addr:$src), sub_32bit)>;
//def : Pat<(extloadi64i8 addr:$src),
//    (SUBREG_TO_REG (i64 0), (MOVZX32rm8 addr:$src), sub_32bit)>;
//def : Pat<(extloadi64i16 addr:$src),
//    (SUBREG_TO_REG (i64 0), (MOVZX32rm16 addr:$src), sub_32bit)>;
//def : Pat<(extloadi64i32 addr:$src),
//    (SUBREG_TO_REG (i64 0), (MOV32rm addr:$src), sub_32bit)>;
//
//// anyext. Define these to do an explicit zero-extend to
//// avoid partial-register updates.
//def : Pat<(i16 (anyext GR8 :$src)), (EXTRACT_SUBREG
//              (MOVZX32rr8 GR8 :$src), sub_16bit)>;
//def : Pat<(i32 (anyext GR8 :$src)), (MOVZX32rr8  GR8 :$src)>;
//
//// Except for i16 -> i32 since isel expect i16 ops to be promoted to i32.
//def : Pat<(i32 (anyext GR16:$src)),
//    (INSERT_SUBREG (i32 (IMPLICIT_DEF)), GR16:$src, sub_16bit)>;
//
//def : Pat<(i64 (anyext GR8 :$src)),
//    (SUBREG_TO_REG (i64 0), (MOVZX32rr8  GR8  :$src), sub_32bit)>;
//def : Pat<(i64 (anyext GR16:$src)),
//    (SUBREG_TO_REG (i64 0), (MOVZX32rr16 GR16 :$src), sub_32bit)>;
//def : Pat<(i64 (anyext GR32:$src)),
//    (SUBREG_TO_REG (i64 0), GR32:$src, sub_32bit)>;
//
//
//// Any instruction that defines a 32-bit result leaves the high half of the
//// register. Truncate can be lowered to EXTRACT_SUBREG. CopyFromReg may
//// be copying from a truncate. And cse523's cmov doesn't do anything if the
//// condition is false. But any other 32-bit operation will zero-extend
//// up to 64 bits.
//def def32 : PatLeaf<(i32 GR32:$src), [{
//    return N->getOpcode() != ISD::TRUNCATE &&
//        N->getOpcode() != TargetOpcode::EXTRACT_SUBREG &&
//        N->getOpcode() != ISD::CopyFromReg &&
//        N->getOpcode() != Cse523ISD::CMOV;
//}]>;
//
//// In the case of a 32-bit def that is known to implicitly zero-extend,
//// we can use a SUBREG_TO_REG.
//def : Pat<(i64 (zext def32:$src)),
//    (SUBREG_TO_REG (i64 0), GR32:$src, sub_32bit)>;
//
////===----------------------------------------------------------------------===//
//// Pattern match OR as ADD
////===----------------------------------------------------------------------===//
//
//// If safe, we prefer to pattern match OR as ADD at isel time. ADD can be
//// 3-addressified into an LEA instruction to avoid copies.  However, we also
//// want to finally emit these instructions as an or at the end of the code
//// generator to make the generated code easier to read.  To do this, we select
//// into "disjoint bits" pseudo ops.
//
//// Treat an 'or' node is as an 'add' if the or'ed bits are known to be zero.
//def or_is_add : PatFrag<(ops node:$lhs, node:$rhs), (or node:$lhs, node:$rhs),[{
//    if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(N->getOperand(1)))
//        return CurDAG->MaskedValueIsZero(N->getOperand(0), CN->getAPIntValue());
//
//    APInt KnownZero0, KnownOne0;
//    CurDAG->ComputeMaskedBits(N->getOperand(0), KnownZero0, KnownOne0, 0);
//    APInt KnownZero1, KnownOne1;
//    CurDAG->ComputeMaskedBits(N->getOperand(1), KnownZero1, KnownOne1, 0);
//    return (~KnownZero0 & ~KnownZero1) == 0;
//}]>;
//
//
//// (or x1, x2) -> (add x1, x2) if two operands are known not to share bits.
//// Try this before the selecting to OR.
//let AddedComplexity = 5, SchedRW = [WriteALU] in {
//
//    let isConvertibleToThreeAddress = 1,
//        Constraints = "$src1 = $dst", Defs = [EFLAGS] in {
//            let isCommutable = 1 in {
//                def ADD16rr_DB  : I<0, Pseudo, (outs GR16:$dst), (ins GR16:$src1, GR16:$src2),
//                "", // orw/addw REG, REG
//                [(set GR16:$dst, (or_is_add GR16:$src1, GR16:$src2))]>;
//                def ADD32rr_DB  : I<0, Pseudo, (outs GR32:$dst), (ins GR32:$src1, GR32:$src2),
//                    "", // orl/addl REG, REG
//                    [(set GR32:$dst, (or_is_add GR32:$src1, GR32:$src2))]>;
//                def ADD64rr_DB  : I<0, Pseudo, (outs GR64:$dst), (ins GR64:$src1, GR64:$src2),
//                    "", // orq/addq REG, REG
//                    [(set GR64:$dst, (or_is_add GR64:$src1, GR64:$src2))]>;
//            } // isCommutable
//
//            // NOTE: These are order specific, we want the ri8 forms to be listed
//            // first so that they are slightly preferred to the ri forms.
//
//            def ADD16ri8_DB : I<0, Pseudo,
//                (outs GR16:$dst), (ins GR16:$src1, i16i8imm:$src2),
//                "", // orw/addw REG, imm8
//                [(set GR16:$dst,(or_is_add GR16:$src1,i16immSExt8:$src2))]>;
//            def ADD16ri_DB  : I<0, Pseudo, (outs GR16:$dst), (ins GR16:$src1, i16imm:$src2),
//                "", // orw/addw REG, imm
//                [(set GR16:$dst, (or_is_add GR16:$src1, imm:$src2))]>;
//
//            def ADD32ri8_DB : I<0, Pseudo,
//                (outs GR32:$dst), (ins GR32:$src1, i32i8imm:$src2),
//                "", // orl/addl REG, imm8
//                [(set GR32:$dst,(or_is_add GR32:$src1,i32immSExt8:$src2))]>;
//            def ADD32ri_DB  : I<0, Pseudo, (outs GR32:$dst), (ins GR32:$src1, i32imm:$src2),
//                "", // orl/addl REG, imm
//                [(set GR32:$dst, (or_is_add GR32:$src1, imm:$src2))]>;
//
//
//            def ADD64ri8_DB : I<0, Pseudo,
//                (outs GR64:$dst), (ins GR64:$src1, i64i8imm:$src2),
//                "", // orq/addq REG, imm8
//                [(set GR64:$dst, (or_is_add GR64:$src1,
//                            i64immSExt8:$src2))]>;
//            def ADD64ri32_DB : I<0, Pseudo,
//                (outs GR64:$dst), (ins GR64:$src1, i64i32imm:$src2),
//                "", // orq/addq REG, imm
//                [(set GR64:$dst, (or_is_add GR64:$src1,
//                            i64immSExt32:$src2))]>;
//        }
//} // AddedComplexity, SchedRW
//
//
////===----------------------------------------------------------------------===//
//// Some peepholes
////===----------------------------------------------------------------------===//
//
//// Odd encoding trick: -128 fits into an 8-bit immediate field while
//// +128 doesn't, so in this special case use a sub instead of an add.
//def : Pat<(add GR16:$src1, 128),
//    (SUB16ri8 GR16:$src1, -128)>;
//def : Pat<(store (add (loadi16 addr:$dst), 128), addr:$dst),
//    (SUB16mi8 addr:$dst, -128)>;
//
//def : Pat<(add GR32:$src1, 128),
//    (SUB32ri8 GR32:$src1, -128)>;
//def : Pat<(store (add (loadi32 addr:$dst), 128), addr:$dst),
//    (SUB32mi8 addr:$dst, -128)>;
//
//def : Pat<(add GR64:$src1, 128),
//    (SUB64ri8 GR64:$src1, -128)>;
//def : Pat<(store (add (loadi64 addr:$dst), 128), addr:$dst),
//    (SUB64mi8 addr:$dst, -128)>;
//
//// The same trick applies for 32-bit immediate fields in 64-bit
//// instructions.
//def : Pat<(add GR64:$src1, 0x0000000080000000),
//    (SUB64ri32 GR64:$src1, 0xffffffff80000000)>;
//def : Pat<(store (add (loadi64 addr:$dst), 0x00000000800000000), addr:$dst),
//    (SUB64mi32 addr:$dst, 0xffffffff80000000)>;
//
//// To avoid needing to materialize an immediate in a register, use a 32-bit and
//// with implicit zero-extension instead of a 64-bit and if the immediate has at
//// least 32 bits of leading zeros. If in addition the last 32 bits can be
//// represented with a sign extension of a 8 bit constant, use that.
//
//def : Pat<(and GR64:$src, i64immZExt32SExt8:$imm),
//    (SUBREG_TO_REG
//     (i64 0),
//     (AND32ri8
//      (EXTRACT_SUBREG GR64:$src, sub_32bit),
//      (i32 (GetLo8XForm imm:$imm))),
//     sub_32bit)>;
//
//def : Pat<(and GR64:$src, i64immZExt32:$imm),
//    (SUBREG_TO_REG
//     (i64 0),
//     (AND32ri
//      (EXTRACT_SUBREG GR64:$src, sub_32bit),
//      (i32 (GetLo32XForm imm:$imm))),
//     sub_32bit)>;
//
//
//// r & (2^16-1) ==> movz
//def : Pat<(and GR32:$src1, 0xffff),
//    (MOVZX32rr16 (EXTRACT_SUBREG GR32:$src1, sub_16bit))>;
//// r & (2^8-1) ==> movz
//def : Pat<(and GR32:$src1, 0xff),
//    (MOVZX32rr8 (EXTRACT_SUBREG (i32 (COPY_TO_REGCLASS GR32:$src1,
//                                      GR32_ABCD)),
//                 sub_8bit))>,
//    Requires<[Not64BitMode]>;
//// r & (2^8-1) ==> movz
//def : Pat<(and GR16:$src1, 0xff),
//    (EXTRACT_SUBREG (MOVZX32rr8 (EXTRACT_SUBREG
//                                 (i16 (COPY_TO_REGCLASS GR16:$src1, GR16_ABCD)), sub_8bit)),
//     sub_16bit)>,
//    Requires<[Not64BitMode]>;
//
//// r & (2^32-1) ==> movz
//def : Pat<(and GR64:$src, 0x00000000FFFFFFFF),
//    (SUBREG_TO_REG (i64 0),
//     (MOV32rr (EXTRACT_SUBREG GR64:$src, sub_32bit)),
//     sub_32bit)>;
//// r & (2^16-1) ==> movz
//def : Pat<(and GR64:$src, 0xffff),
//    (SUBREG_TO_REG (i64 0),
//     (MOVZX32rr16 (i16 (EXTRACT_SUBREG GR64:$src, sub_16bit))),
//     sub_32bit)>;
//// r & (2^8-1) ==> movz
//def : Pat<(and GR64:$src, 0xff),
//    (SUBREG_TO_REG (i64 0),
//     (MOVZX32rr8 (i8 (EXTRACT_SUBREG GR64:$src, sub_8bit))),
//     sub_32bit)>;
//// r & (2^8-1) ==> movz
//def : Pat<(and GR32:$src1, 0xff),
//    (MOVZX32rr8 (EXTRACT_SUBREG GR32:$src1, sub_8bit))>,
//    Requires<[In64BitMode]>;
//// r & (2^8-1) ==> movz
//def : Pat<(and GR16:$src1, 0xff),
//    (EXTRACT_SUBREG (MOVZX32rr8 (i8
//                                 (EXTRACT_SUBREG GR16:$src1, sub_8bit))), sub_16bit)>,
//    Requires<[In64BitMode]>;
//
//
//// sext_inreg patterns
//def : Pat<(sext_inreg GR32:$src, i16),
//    (MOVSX32rr16 (EXTRACT_SUBREG GR32:$src, sub_16bit))>;
//def : Pat<(sext_inreg GR32:$src, i8),
//    (MOVSX32rr8 (EXTRACT_SUBREG (i32 (COPY_TO_REGCLASS GR32:$src,
//                                      GR32_ABCD)),
//                 sub_8bit))>,
//    Requires<[Not64BitMode]>;
//
//def : Pat<(sext_inreg GR16:$src, i8),
//    (EXTRACT_SUBREG (i32 (MOVSX32rr8 (EXTRACT_SUBREG
//                                      (i32 (COPY_TO_REGCLASS GR16:$src, GR16_ABCD)), sub_8bit))),
//     sub_16bit)>,
//    Requires<[Not64BitMode]>;
//
//def : Pat<(sext_inreg GR64:$src, i32),
//    (MOVSX64rr32 (EXTRACT_SUBREG GR64:$src, sub_32bit))>;
//def : Pat<(sext_inreg GR64:$src, i16),
//    (MOVSX64rr16 (EXTRACT_SUBREG GR64:$src, sub_16bit))>;
//def : Pat<(sext_inreg GR64:$src, i8),
//    (MOVSX64rr8 (EXTRACT_SUBREG GR64:$src, sub_8bit))>;
//def : Pat<(sext_inreg GR32:$src, i8),
//    (MOVSX32rr8 (EXTRACT_SUBREG GR32:$src, sub_8bit))>,
//    Requires<[In64BitMode]>;
//def : Pat<(sext_inreg GR16:$src, i8),
//    (EXTRACT_SUBREG (MOVSX32rr8
//                     (EXTRACT_SUBREG GR16:$src, sub_8bit)), sub_16bit)>,
//    Requires<[In64BitMode]>;
//
//// sext, sext_load, zext, zext_load
//def: Pat<(i16 (sext GR8:$src)),
//    (EXTRACT_SUBREG (MOVSX32rr8 GR8:$src), sub_16bit)>;
//def: Pat<(sextloadi16i8 addr:$src),
//    (EXTRACT_SUBREG (MOVSX32rm8 addr:$src), sub_16bit)>;
//def: Pat<(i16 (zext GR8:$src)),
//    (EXTRACT_SUBREG (MOVZX32rr8 GR8:$src), sub_16bit)>;
//def: Pat<(zextloadi16i8 addr:$src),
//    (EXTRACT_SUBREG (MOVZX32rm8 addr:$src), sub_16bit)>;
//
//// trunc patterns
//def : Pat<(i16 (trunc GR32:$src)),
//    (EXTRACT_SUBREG GR32:$src, sub_16bit)>;
//def : Pat<(i8 (trunc GR32:$src)),
//    (EXTRACT_SUBREG (i32 (COPY_TO_REGCLASS GR32:$src, GR32_ABCD)),
//     sub_8bit)>,
//    Requires<[Not64BitMode]>;
//def : Pat<(i8 (trunc GR16:$src)),
//    (EXTRACT_SUBREG (i16 (COPY_TO_REGCLASS GR16:$src, GR16_ABCD)),
//     sub_8bit)>,
//    Requires<[Not64BitMode]>;
//def : Pat<(i32 (trunc GR64:$src)),
//    (EXTRACT_SUBREG GR64:$src, sub_32bit)>;
//def : Pat<(i16 (trunc GR64:$src)),
//    (EXTRACT_SUBREG GR64:$src, sub_16bit)>;
//def : Pat<(i8 (trunc GR64:$src)),
//    (EXTRACT_SUBREG GR64:$src, sub_8bit)>;
//def : Pat<(i8 (trunc GR32:$src)),
//    (EXTRACT_SUBREG GR32:$src, sub_8bit)>,
//    Requires<[In64BitMode]>;
//def : Pat<(i8 (trunc GR16:$src)),
//    (EXTRACT_SUBREG GR16:$src, sub_8bit)>,
//    Requires<[In64BitMode]>;
//
//// h-register tricks
//def : Pat<(i8 (trunc (srl_su GR16:$src, (i8 8)))),
//    (EXTRACT_SUBREG (i16 (COPY_TO_REGCLASS GR16:$src, GR16_ABCD)),
//     sub_8bit_hi)>,
//    Requires<[Not64BitMode]>;
//def : Pat<(i8 (trunc (srl_su GR32:$src, (i8 8)))),
//    (EXTRACT_SUBREG (i32 (COPY_TO_REGCLASS GR32:$src, GR32_ABCD)),
//     sub_8bit_hi)>,
//    Requires<[Not64BitMode]>;
//def : Pat<(srl GR16:$src, (i8 8)),
//    (EXTRACT_SUBREG
//     (MOVZX32rr8
//      (EXTRACT_SUBREG (i16 (COPY_TO_REGCLASS GR16:$src, GR16_ABCD)),
//       sub_8bit_hi)),
//     sub_16bit)>,
//    Requires<[Not64BitMode]>;
//def : Pat<(i32 (zext (srl_su GR16:$src, (i8 8)))),
//    (MOVZX32rr8 (EXTRACT_SUBREG (i16 (COPY_TO_REGCLASS GR16:$src,
//                                      GR16_ABCD)),
//                 sub_8bit_hi))>,
//    Requires<[Not64BitMode]>;
//def : Pat<(i32 (anyext (srl_su GR16:$src, (i8 8)))),
//    (MOVZX32rr8 (EXTRACT_SUBREG (i16 (COPY_TO_REGCLASS GR16:$src,
//                                      GR16_ABCD)),
//                 sub_8bit_hi))>,
//    Requires<[Not64BitMode]>;
//def : Pat<(and (srl_su GR32:$src, (i8 8)), (i32 255)),
//    (MOVZX32rr8 (EXTRACT_SUBREG (i32 (COPY_TO_REGCLASS GR32:$src,
//                                      GR32_ABCD)),
//                 sub_8bit_hi))>,
//    Requires<[Not64BitMode]>;
//def : Pat<(srl (and_su GR32:$src, 0xff00), (i8 8)),
//    (MOVZX32rr8 (EXTRACT_SUBREG (i32 (COPY_TO_REGCLASS GR32:$src,
//                                      GR32_ABCD)),
//                 sub_8bit_hi))>,
//    Requires<[Not64BitMode]>;
//
//// h-register tricks.
//// For now, be conservative on cse523-64 and use an h-register extract only if the
//// value is immediately zero-extended or stored, which are somewhat common
//// cases. This uses a bunch of code to prevent a register requiring a REX prefix
//// from being allocated in the same instruction as the h register, as there's
//// currently no way to describe this requirement to the register allocator.
//
//// h-register extract and zero-extend.
//def : Pat<(and (srl_su GR64:$src, (i8 8)), (i64 255)),
//    (SUBREG_TO_REG
//     (i64 0),
//     (MOVZX32_NOREXrr8
//      (EXTRACT_SUBREG (i64 (COPY_TO_REGCLASS GR64:$src, GR64_ABCD)),
//       sub_8bit_hi)),
//     sub_32bit)>;
//def : Pat<(and (srl_su GR32:$src, (i8 8)), (i32 255)),
//    (MOVZX32_NOREXrr8
//     (EXTRACT_SUBREG (i32 (COPY_TO_REGCLASS GR32:$src, GR32_ABCD)),
//      sub_8bit_hi))>,
//    Requires<[In64BitMode]>;
//def : Pat<(srl (and_su GR32:$src, 0xff00), (i8 8)),
//    (MOVZX32_NOREXrr8 (EXTRACT_SUBREG (i32 (COPY_TO_REGCLASS GR32:$src,
//                                            GR32_ABCD)),
//                       sub_8bit_hi))>,
//    Requires<[In64BitMode]>;
//def : Pat<(srl GR16:$src, (i8 8)),
//    (EXTRACT_SUBREG
//     (MOVZX32_NOREXrr8
//      (EXTRACT_SUBREG (i16 (COPY_TO_REGCLASS GR16:$src, GR16_ABCD)),
//       sub_8bit_hi)),
//     sub_16bit)>,
//    Requires<[In64BitMode]>;
//def : Pat<(i32 (zext (srl_su GR16:$src, (i8 8)))),
//    (MOVZX32_NOREXrr8
//     (EXTRACT_SUBREG (i16 (COPY_TO_REGCLASS GR16:$src, GR16_ABCD)),
//      sub_8bit_hi))>,
//    Requires<[In64BitMode]>;
//def : Pat<(i32 (anyext (srl_su GR16:$src, (i8 8)))),
//    (MOVZX32_NOREXrr8
//     (EXTRACT_SUBREG (i16 (COPY_TO_REGCLASS GR16:$src, GR16_ABCD)),
//      sub_8bit_hi))>,
//    Requires<[In64BitMode]>;
//def : Pat<(i64 (zext (srl_su GR16:$src, (i8 8)))),
//    (SUBREG_TO_REG
//     (i64 0),
//     (MOVZX32_NOREXrr8
//      (EXTRACT_SUBREG (i16 (COPY_TO_REGCLASS GR16:$src, GR16_ABCD)),
//       sub_8bit_hi)),
//     sub_32bit)>;
//def : Pat<(i64 (anyext (srl_su GR16:$src, (i8 8)))),
//    (SUBREG_TO_REG
//     (i64 0),
//     (MOVZX32_NOREXrr8
//      (EXTRACT_SUBREG (i16 (COPY_TO_REGCLASS GR16:$src, GR16_ABCD)),
//       sub_8bit_hi)),
//     sub_32bit)>;
//
//// h-register extract and store.
//def : Pat<(store (i8 (trunc_su (srl_su GR64:$src, (i8 8)))), addr:$dst),
//    (MOV8mr_NOREX
//     addr:$dst,
//     (EXTRACT_SUBREG (i64 (COPY_TO_REGCLASS GR64:$src, GR64_ABCD)),
//      sub_8bit_hi))>;
//def : Pat<(store (i8 (trunc_su (srl_su GR32:$src, (i8 8)))), addr:$dst),
//    (MOV8mr_NOREX
//     addr:$dst,
//     (EXTRACT_SUBREG (i32 (COPY_TO_REGCLASS GR32:$src, GR32_ABCD)),
//      sub_8bit_hi))>,
//    Requires<[In64BitMode]>;
//def : Pat<(store (i8 (trunc_su (srl_su GR16:$src, (i8 8)))), addr:$dst),
//    (MOV8mr_NOREX
//     addr:$dst,
//     (EXTRACT_SUBREG (i16 (COPY_TO_REGCLASS GR16:$src, GR16_ABCD)),
//      sub_8bit_hi))>,
//    Requires<[In64BitMode]>;
//
//
// (shl x, 1) ==> (add x, x)
// Note that if x is undef (immediate or otherwise), we could theoretically
// end up with the two uses of x getting different values, producing a result
// where the least significant bit is not 0. However, the probability of this
// happening is considered low enough that this is officially not a
// "real problem".
def : Pat<(shl GR64:$src1, (i64 1)), (ADD64rr GR64:$src1, GR64:$src1)>;

// Helper imms that check if a mask doesn't change significant shift bits.
//def immShift32 : ImmLeaf<i8, [{ return CountTrailingOnes_32(Imm) >= 5; }]>;
def immShift64 : ImmLeaf<i64, [{ return CountTrailingOnes_32(Imm) >= 6; }]>;

//// (shl x (and y, 31)) ==> (shl x, y)
//def : Pat<(shl GR8:$src1, (and CL, immShift32)),
//    (SHL8rCL GR8:$src1)>;
//def : Pat<(shl GR16:$src1, (and CL, immShift32)),
//    (SHL16rCL GR16:$src1)>;
//def : Pat<(shl GR32:$src1, (and CL, immShift32)),
//    (SHL32rCL GR32:$src1)>;
//def : Pat<(store (shl (loadi8 addr:$dst), (and CL, immShift32)), addr:$dst),
//    (SHL8mCL addr:$dst)>;
//def : Pat<(store (shl (loadi16 addr:$dst), (and CL, immShift32)), addr:$dst),
//    (SHL16mCL addr:$dst)>;
//def : Pat<(store (shl (loadi32 addr:$dst), (and CL, immShift32)), addr:$dst),
//    (SHL32mCL addr:$dst)>;
//
//def : Pat<(srl GR8:$src1, (and CL, immShift32)),
//    (SHR8rCL GR8:$src1)>;
//def : Pat<(srl GR16:$src1, (and CL, immShift32)),
//    (SHR16rCL GR16:$src1)>;
//def : Pat<(srl GR32:$src1, (and CL, immShift32)),
//    (SHR32rCL GR32:$src1)>;
//def : Pat<(store (srl (loadi8 addr:$dst), (and CL, immShift32)), addr:$dst),
//    (SHR8mCL addr:$dst)>;
//def : Pat<(store (srl (loadi16 addr:$dst), (and CL, immShift32)), addr:$dst),
//    (SHR16mCL addr:$dst)>;
//def : Pat<(store (srl (loadi32 addr:$dst), (and CL, immShift32)), addr:$dst),
//    (SHR32mCL addr:$dst)>;
//
//def : Pat<(sra GR8:$src1, (and CL, immShift32)),
//    (SAR8rCL GR8:$src1)>;
//def : Pat<(sra GR16:$src1, (and CL, immShift32)),
//    (SAR16rCL GR16:$src1)>;
//def : Pat<(sra GR32:$src1, (and CL, immShift32)),
//    (SAR32rCL GR32:$src1)>;
//def : Pat<(store (sra (loadi8 addr:$dst), (and CL, immShift32)), addr:$dst),
//    (SAR8mCL addr:$dst)>;
//def : Pat<(store (sra (loadi16 addr:$dst), (and CL, immShift32)), addr:$dst),
//    (SAR16mCL addr:$dst)>;
//def : Pat<(store (sra (loadi32 addr:$dst), (and CL, immShift32)), addr:$dst),
//    (SAR32mCL addr:$dst)>;
//
// (shl x (and y, 63)) ==> (shl x, y)
def : Pat<(shl GR64:$src1, (and RCX, immShift64)),
    (SHL64rCL GR64:$src1)>;
//def : Pat<(store (shl (loadi64 addr:$dst), (and CL, 63)), addr:$dst),
//    (SHL64mCL addr:$dst)>;
//
//def : Pat<(srl GR64:$src1, (and RCX, immShift64)),
//    (SHR64rCL GR64:$src1)>;
//def : Pat<(store (srl (loadi64 addr:$dst), (and CL, 63)), addr:$dst),
//    (SHR64mCL addr:$dst)>;
//
//def : Pat<(sra GR64:$src1, (and RCX, immShift64)),
//    (SAR64rCL GR64:$src1)>;
//def : Pat<(store (sra (loadi64 addr:$dst), (and CL, 63)), addr:$dst),
//    (SAR64mCL addr:$dst)>;

//
//// (anyext (setcc_carry)) -> (setcc_carry)
//def : Pat<(i16 (anyext (i8 (Cse523setcc_c Cse523_COND_B, EFLAGS)))),
//    (SETB_C16r)>;
//def : Pat<(i32 (anyext (i8 (Cse523setcc_c Cse523_COND_B, EFLAGS)))),
//    (SETB_C32r)>;
//def : Pat<(i32 (anyext (i16 (Cse523setcc_c Cse523_COND_B, EFLAGS)))),
//    (SETB_C32r)>;
//
////===----------------------------------------------------------------------===//
//// EFLAGS-defining Patterns
////===----------------------------------------------------------------------===//
//
//// add reg, reg
//def : Pat<(add GR8 :$src1, GR8 :$src2), (ADD8rr  GR8 :$src1, GR8 :$src2)>;
//def : Pat<(add GR16:$src1, GR16:$src2), (ADD16rr GR16:$src1, GR16:$src2)>;
//def : Pat<(add GR32:$src1, GR32:$src2), (ADD32rr GR32:$src1, GR32:$src2)>;
//
//// add reg, mem
//def : Pat<(add GR8:$src1, (loadi8 addr:$src2)),
//    (ADD8rm GR8:$src1, addr:$src2)>;
//def : Pat<(add GR16:$src1, (loadi16 addr:$src2)),
//    (ADD16rm GR16:$src1, addr:$src2)>;
//def : Pat<(add GR32:$src1, (loadi32 addr:$src2)),
//    (ADD32rm GR32:$src1, addr:$src2)>;
//
//// add reg, imm
//def : Pat<(add GR8 :$src1, imm:$src2), (ADD8ri  GR8:$src1 , imm:$src2)>;
//def : Pat<(add GR16:$src1, imm:$src2), (ADD16ri GR16:$src1, imm:$src2)>;
//def : Pat<(add GR32:$src1, imm:$src2), (ADD32ri GR32:$src1, imm:$src2)>;
//def : Pat<(add GR16:$src1, i16immSExt8:$src2),
//    (ADD16ri8 GR16:$src1, i16immSExt8:$src2)>;
//def : Pat<(add GR32:$src1, i32immSExt8:$src2),
//    (ADD32ri8 GR32:$src1, i32immSExt8:$src2)>;
//
//// sub reg, reg
//def : Pat<(sub GR8 :$src1, GR8 :$src2), (SUB8rr  GR8 :$src1, GR8 :$src2)>;
//def : Pat<(sub GR16:$src1, GR16:$src2), (SUB16rr GR16:$src1, GR16:$src2)>;
//def : Pat<(sub GR32:$src1, GR32:$src2), (SUB32rr GR32:$src1, GR32:$src2)>;
//
//// sub reg, mem
//def : Pat<(sub GR8:$src1, (loadi8 addr:$src2)),
//    (SUB8rm GR8:$src1, addr:$src2)>;
//def : Pat<(sub GR16:$src1, (loadi16 addr:$src2)),
//    (SUB16rm GR16:$src1, addr:$src2)>;
//def : Pat<(sub GR32:$src1, (loadi32 addr:$src2)),
//    (SUB32rm GR32:$src1, addr:$src2)>;
//
//// sub reg, imm
//def : Pat<(sub GR8:$src1, imm:$src2),
//    (SUB8ri GR8:$src1, imm:$src2)>;
//def : Pat<(sub GR16:$src1, imm:$src2),
//    (SUB16ri GR16:$src1, imm:$src2)>;
//def : Pat<(sub GR32:$src1, imm:$src2),
//    (SUB32ri GR32:$src1, imm:$src2)>;
//def : Pat<(sub GR16:$src1, i16immSExt8:$src2),
//    (SUB16ri8 GR16:$src1, i16immSExt8:$src2)>;
//def : Pat<(sub GR32:$src1, i32immSExt8:$src2),
//    (SUB32ri8 GR32:$src1, i32immSExt8:$src2)>;
//
// sub 0, reg
def : Pat<(Cse523sub_flag 0, GR64:$src), (NEG64r GR64:$src)>;
//
//// mul reg, reg
//def : Pat<(mul GR16:$src1, GR16:$src2),
//    (IMUL16rr GR16:$src1, GR16:$src2)>;
//def : Pat<(mul GR32:$src1, GR32:$src2),
//    (IMUL32rr GR32:$src1, GR32:$src2)>;
//
//// mul reg, mem
//def : Pat<(mul GR16:$src1, (loadi16 addr:$src2)),
//    (IMUL16rm GR16:$src1, addr:$src2)>;
//def : Pat<(mul GR32:$src1, (loadi32 addr:$src2)),
//    (IMUL32rm GR32:$src1, addr:$src2)>;
//
//// mul reg, imm
//def : Pat<(mul GR16:$src1, imm:$src2),
//    (IMUL16rri GR16:$src1, imm:$src2)>;
//def : Pat<(mul GR32:$src1, imm:$src2),
//    (IMUL32rri GR32:$src1, imm:$src2)>;
//def : Pat<(mul GR16:$src1, i16immSExt8:$src2),
//    (IMUL16rri8 GR16:$src1, i16immSExt8:$src2)>;
//def : Pat<(mul GR32:$src1, i32immSExt8:$src2),
//    (IMUL32rri8 GR32:$src1, i32immSExt8:$src2)>;
//
//// reg = mul mem, imm
//def : Pat<(mul (loadi16 addr:$src1), imm:$src2),
//    (IMUL16rmi addr:$src1, imm:$src2)>;
//def : Pat<(mul (loadi32 addr:$src1), imm:$src2),
//    (IMUL32rmi addr:$src1, imm:$src2)>;
//def : Pat<(mul (loadi16 addr:$src1), i16immSExt8:$src2),
//    (IMUL16rmi8 addr:$src1, i16immSExt8:$src2)>;
//def : Pat<(mul (loadi32 addr:$src1), i32immSExt8:$src2),
//    (IMUL32rmi8 addr:$src1, i32immSExt8:$src2)>;
//
// Patterns for nodes that do not produce flags, for instructions that do.

// addition
def : Pat<(add GR64:$src1, GR64:$src2),
    (ADD64rr GR64:$src1, GR64:$src2)>;
def : Pat<(add GR64:$src1, i64immSExt32:$src2),
    (ADD64ri32 GR64:$src1, i64immSExt32:$src2)>;
def : Pat<(add GR64:$src1, (loadi64 addr:$src2)),
    (ADD64rm GR64:$src1, addr:$src2)>;

// subtraction
def : Pat<(sub GR64:$src1, GR64:$src2),
    (SUB64rr GR64:$src1, GR64:$src2)>;
def : Pat<(sub GR64:$src1, (loadi64 addr:$src2)),
    (SUB64rm GR64:$src1, addr:$src2)>;
def : Pat<(sub GR64:$src1, i64immSExt32:$src2),
    (SUB64ri32 GR64:$src1, i64immSExt32:$src2)>;

// Multiply
def : Pat<(mul GR64:$src1, GR64:$src2),
    (IMUL64rr GR64:$src1, GR64:$src2)>;
def : Pat<(mul GR64:$src1, (loadi64 addr:$src2)),
    (IMUL64rm GR64:$src1, addr:$src2)>;
def : Pat<(mul GR64:$src1, i64immSExt32:$src2),
    (IMUL64rri32 GR64:$src1, i64immSExt32:$src2)>;
def : Pat<(mul (loadi64 addr:$src1), i64immSExt32:$src2),
    (IMUL64rmi32 addr:$src1, i64immSExt32:$src2)>;

// Increment reg.
def : Pat<(add GR64:$src, 1), (INC64r    GR64:$src)>;

// Decrement reg.
def : Pat<(add GR64:$src, -1), (DEC64r    GR64:$src)>;

// or reg/reg.
def : Pat<(or GR64:$src1, GR64:$src2), (OR64rr GR64:$src1, GR64:$src2)>;

// or reg/mem
def : Pat<(or GR64:$src1, (loadi64 addr:$src2)),
    (OR64rm GR64:$src1, addr:$src2)>;

// or reg/imm
def : Pat<(or GR64:$src1, i64immSExt32:$src2),
    (OR64ri32 GR64:$src1, i64immSExt32:$src2)>;

// xor reg/reg
def : Pat<(xor GR64:$src1, GR64:$src2), (XOR64rr GR64:$src1, GR64:$src2)>;

// xor reg/mem
def : Pat<(xor GR64:$src1, (loadi64 addr:$src2)),
    (XOR64rm GR64:$src1, addr:$src2)>;

// xor reg/imm
def : Pat<(xor GR64:$src1, i64immSExt32:$src2),
    (XOR64ri32 GR64:$src1, i64immSExt32:$src2)>;

// and reg/reg
def : Pat<(and GR64:$src1, GR64:$src2), (AND64rr GR64:$src1, GR64:$src2)>;

// and reg/mem
def : Pat<(and GR64:$src1, (loadi64 addr:$src2)),
    (AND64rm GR64:$src1, addr:$src2)>;

// and reg/imm
def : Pat<(and GR64:$src1, i64immSExt32:$src2),
    (AND64ri32 GR64:$src1, i64immSExt32:$src2)>;

