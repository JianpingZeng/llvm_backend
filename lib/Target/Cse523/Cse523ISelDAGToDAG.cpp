//===- Cse523ISelDAGToDAG.cpp - A DAG pattern matching inst selector for Cse523 -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a DAG pattern matching instruction selector for Cse523,
// converting from a legalized dag to a Cse523 dag.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "cse523-isel"
#include "Cse523.h"
#include "Cse523InstrBuilder.h"
#include "Cse523MachineFunctionInfo.h"
#include "Cse523RegisterInfo.h"
#include "Cse523Subtarget.h"
#include "Cse523TargetMachine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

STATISTIC(NumLoadMoved, "Number of loads moved below TokenFactor");

//===----------------------------------------------------------------------===//
//                      Pattern Matcher Implementation
//===----------------------------------------------------------------------===//

namespace {
    /// Cse523ISelAddressMode - This corresponds to Cse523AddressMode, but uses
    /// SDValue's instead of register numbers for the leaves of the matched
    /// tree.
    struct Cse523ISelAddressMode {
        enum {
            RegBase,
            FrameIndexBase
        } BaseType;

        // This is really a union, discriminated by BaseType!
        SDValue Base_Reg;
        int Base_FrameIndex;

        unsigned Scale;
        SDValue IndexReg;
        int32_t Disp;
        SDValue Segment;
        const GlobalValue *GV;
        const Constant *CP;
        const BlockAddress *BlockAddr;
        const char *ES;
        int JT;
        unsigned Align;    // CP alignment.
        unsigned char SymbolFlags;  // Cse523II::MO_*

        Cse523ISelAddressMode()
            : BaseType(RegBase), Base_FrameIndex(0), Scale(1), IndexReg(), Disp(0),
            Segment(), GV(0), CP(0), BlockAddr(0), ES(0), JT(-1), Align(0),
            SymbolFlags(Cse523II::MO_NO_FLAG) {
            }

        bool hasSymbolicDisplacement() const {
            return GV != 0 || CP != 0 || ES != 0 || JT != -1 || BlockAddr != 0;
        }

        bool hasBaseOrIndexReg() const {
            return BaseType == FrameIndexBase ||
                IndexReg.getNode() != 0 || Base_Reg.getNode() != 0;
        }

        /// isRIPRelative - Return true if this addressing mode is already RIP
        /// relative.
        bool isRIPRelative() const {
            if (BaseType != RegBase) return false;
            if (RegisterSDNode *RegNode =
                    dyn_cast_or_null<RegisterSDNode>(Base_Reg.getNode()))
                return RegNode->getReg() == Cse523::RIP;
            return false;
        }

        void setBaseReg(SDValue Reg) {
            BaseType = RegBase;
            Base_Reg = Reg;
        }

#if !defined(NDEBUG) || defined(LLVM_ENABLE_DUMP)
        void dump() {
            dbgs() << "Cse523ISelAddressMode " << this << '\n';
            dbgs() << "Base_Reg ";
            if (Base_Reg.getNode() != 0)
                Base_Reg.getNode()->dump();
            else
                dbgs() << "nul";
            dbgs() << " Base.FrameIndex " << Base_FrameIndex << '\n'
                << " Scale" << Scale << '\n'
                << "IndexReg ";
            if (IndexReg.getNode() != 0)
                IndexReg.getNode()->dump();
            else
                dbgs() << "nul";
            dbgs() << " Disp " << Disp << '\n'
                << "GV <commented> ";
            if (GV)
                GV->dump();
            else
                dbgs() << "nul";
            dbgs() << " CP ";
            if (CP)
                CP->dump();
            else
                dbgs() << "nul";
            dbgs() << '\n'
                << "ES ";
            if (ES)
                dbgs() << ES;
            else
                dbgs() << "nul";
            dbgs() << " JT" << JT << " Align" << Align << '\n';
        }
#endif
    };
}

namespace {
    //===--------------------------------------------------------------------===//
    /// ISel - Cse523 specific code to select Cse523 machine instructions for
    /// SelectionDAG operations.
    ///
    class Cse523DAGToDAGISel : public SelectionDAGISel {
        /// Subtarget - Keep a pointer to the Cse523Subtarget around so that we can
        /// make the right decision when generating code for different targets.
        const Cse523Subtarget *Subtarget;

        /// OptForSize - If true, selector should try to optimize for code size
        /// instead of performance.
        bool OptForSize;

        public:
        explicit Cse523DAGToDAGISel(Cse523TargetMachine &tm, CodeGenOpt::Level OptLevel)
            : SelectionDAGISel(tm, OptLevel),
            Subtarget(&tm.getSubtarget<Cse523Subtarget>()),
            OptForSize(false) {}

        virtual const char *getPassName() const {
            return "Cse523 DAG->DAG Instruction Selection";
        }

        virtual void EmitFunctionEntryCode();

        virtual bool IsProfitableToFold(SDValue N, SDNode *U, SDNode *Root) const;

        virtual void PreprocessISelDAG();

        inline bool immSext8(SDNode *N) const {
            return isInt<8>(cast<ConstantSDNode>(N)->getSExtValue());
        }

        // i64immSExt32 predicate - True if the 64-bit immediate fits in a 32-bit
        // sign extended field.
        inline bool i64immSExt32(SDNode *N) const {
            uint64_t v = cast<ConstantSDNode>(N)->getZExtValue();
            return (int64_t)v == (int32_t)v;
        }

        // Include the pieces autogenerated from the target description.
#include "Cse523GenDAGISel.inc"

        private:
        SDNode *Select(SDNode *N);
        SDNode *SelectGather(SDNode *N, unsigned Opc);
        SDNode *SelectAtomic64(SDNode *Node, unsigned Opc);
        SDNode *SelectAtomicLoadArith(SDNode *Node, MVT NVT);

        bool FoldOffsetIntoAddress(uint64_t Offset, Cse523ISelAddressMode &AM);
        bool MatchLoadInAddress(LoadSDNode *N, Cse523ISelAddressMode &AM);
        bool MatchWrapper(SDValue N, Cse523ISelAddressMode &AM);
        bool MatchAddress(SDValue N, Cse523ISelAddressMode &AM);
        bool MatchAddressRecursively(SDValue N, Cse523ISelAddressMode &AM,
                unsigned Depth);
        bool MatchAddressBase(SDValue N, Cse523ISelAddressMode &AM);
        bool SelectAddr(SDNode *Parent, SDValue N, SDValue &Base,
                SDValue &Scale, SDValue &Index, SDValue &Disp,
                SDValue &Segment);
        bool SelectMOV64Imm32(SDValue N, SDValue &Imm);
        bool SelectLEAAddr(SDValue N, SDValue &Base,
                SDValue &Scale, SDValue &Index, SDValue &Disp,
                SDValue &Segment);
        bool SelectLEA64_32Addr(SDValue N, SDValue &Base,
                SDValue &Scale, SDValue &Index, SDValue &Disp,
                SDValue &Segment);
        bool SelectTLSADDRAddr(SDValue N, SDValue &Base,
                SDValue &Scale, SDValue &Index, SDValue &Disp,
                SDValue &Segment);
        bool SelectScalarSSELoad(SDNode *Root, SDValue N,
                SDValue &Base, SDValue &Scale,
                SDValue &Index, SDValue &Disp,
                SDValue &Segment,
                SDValue &NodeWithChain);

        bool TryFoldLoad(SDNode *P, SDValue N,
                SDValue &Base, SDValue &Scale,
                SDValue &Index, SDValue &Disp,
                SDValue &Segment);

        /// SelectInlineAsmMemoryOperand - Implement addressing mode selection for
        /// inline asm expressions.
        virtual bool SelectInlineAsmMemoryOperand(const SDValue &Op,
                char ConstraintCode,
                std::vector<SDValue> &OutOps);

        void EmitSpecialCodeForMain(MachineBasicBlock *BB, MachineFrameInfo *MFI);

        inline void getAddressOperands(Cse523ISelAddressMode &AM, SDValue &Base,
                SDValue &Scale, SDValue &Index,
                SDValue &Disp, SDValue &Segment) {
            Base  = (AM.BaseType == Cse523ISelAddressMode::FrameIndexBase) ?
                CurDAG->getTargetFrameIndex(AM.Base_FrameIndex,
                        getTargetLowering()->getPointerTy()) :
                AM.Base_Reg;
            Scale = getI8Imm(AM.Scale);
            Index = AM.IndexReg;
            // These are 32-bit even in 64-bit mode since RIP relative offset
            // is 32-bit.
            if (AM.GV)
                Disp = CurDAG->getTargetGlobalAddress(AM.GV, SDLoc(),
                        MVT::i32, AM.Disp,
                        AM.SymbolFlags);
            else if (AM.CP)
                Disp = CurDAG->getTargetConstantPool(AM.CP, MVT::i32,
                        AM.Align, AM.Disp, AM.SymbolFlags);
            else if (AM.ES) {
                assert(!AM.Disp && "Non-zero displacement is ignored with ES.");
                Disp = CurDAG->getTargetExternalSymbol(AM.ES, MVT::i32, AM.SymbolFlags);
            } else if (AM.JT != -1) {
                assert(!AM.Disp && "Non-zero displacement is ignored with JT.");
                Disp = CurDAG->getTargetJumpTable(AM.JT, MVT::i32, AM.SymbolFlags);
            } else if (AM.BlockAddr)
                Disp = CurDAG->getTargetBlockAddress(AM.BlockAddr, MVT::i32, AM.Disp,
                        AM.SymbolFlags);
            else
                Disp = CurDAG->getTargetConstant(AM.Disp, MVT::i32);

            if (AM.Segment.getNode())
                Segment = AM.Segment;
            else
                Segment = CurDAG->getRegister(0, MVT::i32);
        }

        /// getI8Imm - Return a target constant with the specified value, of type
        /// i8.
        inline SDValue getI8Imm(unsigned Imm) {
            return CurDAG->getTargetConstant(Imm, MVT::i8);
        }

        /// getI32Imm - Return a target constant with the specified value, of type
        /// i32.
        inline SDValue getI32Imm(unsigned Imm) {
            return CurDAG->getTargetConstant(Imm, MVT::i32);
        }

        /// getGlobalBaseReg - Return an SDNode that returns the value of
        /// the global base register. Output instructions required to
        /// initialize the global base register, if necessary.
        ///
        SDNode *getGlobalBaseReg();

        /// getTargetMachine - Return a reference to the TargetMachine, casted
        /// to the target-specific type.
        const Cse523TargetMachine &getTargetMachine() const {
            return static_cast<const Cse523TargetMachine &>(TM);
        }

        /// getInstrInfo - Return a reference to the TargetInstrInfo, casted
        /// to the target-specific type.
        const Cse523InstrInfo *getInstrInfo() const {
            return getTargetMachine().getInstrInfo();
        }
    };
}


bool
Cse523DAGToDAGISel::IsProfitableToFold(SDValue N, SDNode *U, SDNode *Root) const {
    if (OptLevel == CodeGenOpt::None) return false;

    if (!N.hasOneUse())
        return false;

    if (N.getOpcode() != ISD::LOAD)
        return true;

    // If N is a load, do additional profitability checks.
    if (U == Root) {
        switch (U->getOpcode()) {
            default: break;
            case Cse523ISD::ADD:
            case Cse523ISD::SUB:
            case Cse523ISD::AND:
            case Cse523ISD::XOR:
            case Cse523ISD::OR:
            case ISD::ADD:
            case ISD::ADDC:
            case ISD::ADDE:
            case ISD::AND:
            case ISD::OR:
            case ISD::XOR: {
                               SDValue Op1 = U->getOperand(1);

                               // If the other operand is a 8-bit immediate we should fold the immediate
                               // instead. This reduces code size.
                               // e.g.
                               // movl 4(%esp), %eax
                               // addl $4, %eax
                               // vs.
                               // movl $4, %eax
                               // addl 4(%esp), %eax
                               // The former is 2 bytes shorter. In case where the increment is 1, then
                               // the saving can be 4 bytes (by using incl %eax).
                               if (ConstantSDNode *Imm = dyn_cast<ConstantSDNode>(Op1))
                                   if (Imm->getAPIntValue().isSignedIntN(8))
                                       return false;

                               // If the other operand is a TLS address, we should fold it instead.
                               // This produces
                               // movl    %gs:0, %eax
                               // leal    i@NTPOFF(%eax), %eax
                               // instead of
                               // movl    $i@NTPOFF, %eax
                               // addl    %gs:0, %eax
                               // if the block also has an access to a second TLS address this will save
                               // a load.
                               // FIXME: This is probably also true for non-TLS addresses.
                               if (Op1.getOpcode() == Cse523ISD::Wrapper) {
                                   SDValue Val = Op1.getOperand(0);
                                   if (Val.getOpcode() == ISD::TargetGlobalTLSAddress)
                                       return false;
                               }
                           }
        }
    }

    return true;
}

/// MoveBelowCallOrigChain - Replace the original chain operand of the call with
/// load's chain operand and move load below the call's chain operand.
static void MoveBelowOrigChain(SelectionDAG *CurDAG, SDValue Load,
        SDValue Call, SDValue OrigChain) {
    SmallVector<SDValue, 8> Ops;
    SDValue Chain = OrigChain.getOperand(0);
    if (Chain.getNode() == Load.getNode())
        Ops.push_back(Load.getOperand(0));
    else {
        assert(Chain.getOpcode() == ISD::TokenFactor &&
                "Unexpected chain operand");
        for (unsigned i = 0, e = Chain.getNumOperands(); i != e; ++i)
            if (Chain.getOperand(i).getNode() == Load.getNode())
                Ops.push_back(Load.getOperand(0));
            else
                Ops.push_back(Chain.getOperand(i));
        SDValue NewChain =
            CurDAG->getNode(ISD::TokenFactor, SDLoc(Load),
                    MVT::Other, &Ops[0], Ops.size());
        Ops.clear();
        Ops.push_back(NewChain);
    }
    for (unsigned i = 1, e = OrigChain.getNumOperands(); i != e; ++i)
        Ops.push_back(OrigChain.getOperand(i));
    CurDAG->UpdateNodeOperands(OrigChain.getNode(), &Ops[0], Ops.size());
    CurDAG->UpdateNodeOperands(Load.getNode(), Call.getOperand(0),
            Load.getOperand(1), Load.getOperand(2));

    unsigned NumOps = Call.getNode()->getNumOperands();
    Ops.clear();
    Ops.push_back(SDValue(Load.getNode(), 1));
    for (unsigned i = 1, e = NumOps; i != e; ++i)
        Ops.push_back(Call.getOperand(i));
    CurDAG->UpdateNodeOperands(Call.getNode(), &Ops[0], NumOps);
}

/// isCalleeLoad - Return true if call address is a load and it can be
/// moved below CALLSEQ_START and the chains leading up to the call.
/// Return the CALLSEQ_START by reference as a second output.
/// In the case of a tail call, there isn't a callseq node between the call
/// chain and the load.
static bool isCalleeLoad(SDValue Callee, SDValue &Chain, bool HasCallSeq) {
    // The transformation is somewhat dangerous if the call's chain was glued to
    // the call. After MoveBelowOrigChain the load is moved between the call and
    // the chain, this can create a cycle if the load is not folded. So it is
    // *really* important that we are sure the load will be folded.
    if (Callee.getNode() == Chain.getNode() || !Callee.hasOneUse())
        return false;
    LoadSDNode *LD = dyn_cast<LoadSDNode>(Callee.getNode());
    if (!LD ||
            LD->isVolatile() ||
            LD->getAddressingMode() != ISD::UNINDEXED ||
            LD->getExtensionType() != ISD::NON_EXTLOAD)
        return false;

    // Now let's find the callseq_start.
    while (HasCallSeq && Chain.getOpcode() != ISD::CALLSEQ_START) {
        if (!Chain.hasOneUse())
            return false;
        Chain = Chain.getOperand(0);
    }

    if (!Chain.getNumOperands())
        return false;
    // Since we are not checking for AA here, conservatively abort if the chain
    // writes to memory. It's not safe to move the callee (a load) across a store.
    if (isa<MemSDNode>(Chain.getNode()) &&
            cast<MemSDNode>(Chain.getNode())->writeMem())
        return false;
    if (Chain.getOperand(0).getNode() == Callee.getNode())
        return true;
    if (Chain.getOperand(0).getOpcode() == ISD::TokenFactor &&
            Callee.getValue(1).isOperandOf(Chain.getOperand(0).getNode()) &&
            Callee.getValue(1).hasOneUse())
        return true;
    return false;
}

void Cse523DAGToDAGISel::PreprocessISelDAG() {
    // OptForSize is used in pattern predicates that isel is matching.
    OptForSize = MF->getFunction()->getAttributes().
        hasAttribute(AttributeSet::FunctionIndex, Attribute::OptimizeForSize);

    for (SelectionDAG::allnodes_iterator I = CurDAG->allnodes_begin(),
            E = CurDAG->allnodes_end(); I != E; ) {
        SDNode *N = I++;  // Preincrement iterator to avoid invalidation issues.

        if (OptLevel != CodeGenOpt::None &&
                // Only does this when target favors doesn't favor register indirect
                // call.
                ((N->getOpcode() == Cse523ISD::CALL && !Subtarget->callRegIndirect()) ||
                 (N->getOpcode() == Cse523ISD::TC_RETURN &&
                  // Only does this if load can be folded into TC_RETURN.
                  (Subtarget->is64Bit() ||
                   getTargetMachine().getRelocationModel() != Reloc::PIC_)))) {
            /// Also try moving call address load from outside callseq_start to just
            /// before the call to allow it to be folded.
            ///
            ///     [Load chain]
            ///         ^
            ///         |
            ///       [Load]
            ///       ^    ^
            ///       |    |
            ///      /      \--
            ///     /          |
            ///[CALLSEQ_START] |
            ///     ^          |
            ///     |          |
            /// [LOAD/C2Reg]   |
            ///     |          |
            ///      \        /
            ///       \      /
            ///       [CALL]
            bool HasCallSeq = N->getOpcode() == Cse523ISD::CALL;
            SDValue Chain = N->getOperand(0);
            SDValue Load  = N->getOperand(1);
            if (!isCalleeLoad(Load, Chain, HasCallSeq))
                continue;
            MoveBelowOrigChain(CurDAG, Load, SDValue(N, 0), Chain);
            ++NumLoadMoved;
            continue;
        }

        // Lower fpround and fpextend nodes that target the FP stack to be store and
        // load to the stack.  This is a gross hack.  We would like to simply mark
        // these as being illegal, but when we do that, legalize produces these when
        // it expands calls, then expands these in the same legalize pass.  We would
        // like dag combine to be able to hack on these between the call expansion
        // and the node legalization.  As such this pass basically does "really
        // late" legalization of these inline with the Cse523 isel pass.
        // FIXME: This should only happen when not compiled with -O0.
        if (N->getOpcode() != ISD::FP_ROUND && N->getOpcode() != ISD::FP_EXTEND)
            continue;

        MVT SrcVT = N->getOperand(0).getSimpleValueType();
        MVT DstVT = N->getSimpleValueType(0);

        // If any of the sources are vectors, no fp stack involved.
        if (SrcVT.isVector() || DstVT.isVector())
            continue;

        // If the source and destination are SSE registers, then this is a legal
        // conversion that should not be lowered.
        const Cse523TargetLowering *Cse523Lowering =
            static_cast<const Cse523TargetLowering *>(getTargetLowering());
        bool SrcIsSSE = Cse523Lowering->isScalarFPTypeInSSEReg(SrcVT);
        bool DstIsSSE = Cse523Lowering->isScalarFPTypeInSSEReg(DstVT);
        if (SrcIsSSE && DstIsSSE)
            continue;

        if (!SrcIsSSE && !DstIsSSE) {
            // If this is an FPStack extension, it is a noop.
            if (N->getOpcode() == ISD::FP_EXTEND)
                continue;
            // If this is a value-preserving FPStack truncation, it is a noop.
            if (N->getConstantOperandVal(1))
                continue;
        }

        // Here we could have an FP stack truncation or an FPStack <-> SSE convert.
        // FPStack has extload and truncstore.  SSE can fold direct loads into other
        // operations.  Based on this, decide what we want to do.
        MVT MemVT;
        if (N->getOpcode() == ISD::FP_ROUND)
            MemVT = DstVT;  // FP_ROUND must use DstVT, we can't do a 'trunc load'.
        else
            MemVT = SrcIsSSE ? SrcVT : DstVT;

        SDValue MemTmp = CurDAG->CreateStackTemporary(MemVT);
        SDLoc dl(N);

        // FIXME: optimize the case where the src/dest is a load or store?
        SDValue Store = CurDAG->getTruncStore(CurDAG->getEntryNode(), dl,
                N->getOperand(0),
                MemTmp, MachinePointerInfo(), MemVT,
                false, false, 0);
        SDValue Result = CurDAG->getExtLoad(ISD::EXTLOAD, dl, DstVT, Store, MemTmp,
                MachinePointerInfo(),
                MemVT, false, false, 0);

        // We're about to replace all uses of the FP_ROUND/FP_EXTEND with the
        // extload we created.  This will cause general havok on the dag because
        // anything below the conversion could be folded into other existing nodes.
        // To avoid invalidating 'I', back it up to the convert node.
        --I;
        CurDAG->ReplaceAllUsesOfValueWith(SDValue(N, 0), Result);

        // Now that we did that, the node is dead.  Increment the iterator to the
        // next node to process, then delete N.
        ++I;
        CurDAG->DeleteNode(N);
    }
}


/// EmitSpecialCodeForMain - Emit any code that needs to be executed only in
/// the main function.
void Cse523DAGToDAGISel::EmitSpecialCodeForMain(MachineBasicBlock *BB,
        MachineFrameInfo *MFI) {
    const TargetInstrInfo *TII = TM.getInstrInfo();
    if (Subtarget->isTargetCygMing()) {
        unsigned CallOp = Cse523::CALL64pcrel32;
        BuildMI(BB, DebugLoc(),
                TII->get(CallOp)).addExternalSymbol("__main");
    }
}

void Cse523DAGToDAGISel::EmitFunctionEntryCode() {
    // If this is main, emit special code for main.
    if (const Function *Fn = MF->getFunction())
        if (Fn->hasExternalLinkage() && Fn->getName() == "main")
            EmitSpecialCodeForMain(MF->begin(), MF->getFrameInfo());
}

static bool isDispSafeForFrameIndex(int64_t Val) {
    // On 64-bit platforms, we can run into an issue where a frame index
    // includes a displacement that, when added to the explicit displacement,
    // will overflow the displacement field. Assuming that the frame index
    // displacement fits into a 31-bit integer  (which is only slightly more
    // aggressive than the current fundamental assumption that it fits into
    // a 32-bit integer), a 31-bit disp should always be safe.
    return isInt<31>(Val);
}

bool Cse523DAGToDAGISel::FoldOffsetIntoAddress(uint64_t Offset,
        Cse523ISelAddressMode &AM) {
    int64_t Val = AM.Disp + Offset;
    CodeModel::Model M = TM.getCodeModel();
    if (Subtarget->is64Bit()) {
        if (!Cse523::isOffsetSuitableForCodeModel(Val, M,
                    AM.hasSymbolicDisplacement()))
            return true;
        // In addition to the checks required for a register base, check that
        // we do not try to use an unsafe Disp with a frame index.
        if (AM.BaseType == Cse523ISelAddressMode::FrameIndexBase &&
                !isDispSafeForFrameIndex(Val))
            return true;
    }
    AM.Disp = Val;
    return false;

}

bool Cse523DAGToDAGISel::MatchLoadInAddress(LoadSDNode *N, Cse523ISelAddressMode &AM){
    SDValue Address = N->getOperand(1);

    // load gs:0 -> GS segment register.
    // load fs:0 -> FS segment register.
    //
    // This optimization is valid because the GNU TLS model defines that
    // gs:0 (or fs:0 on Cse523-64) contains its own address.
    // For more information see http://people.redhat.com/drepper/tls.pdf
    if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Address))
        if (C->getSExtValue() == 0 && AM.Segment.getNode() == 0 &&
                Subtarget->isTargetLinux())
            switch (N->getPointerInfo().getAddrSpace()) {
                case 256:
                    AM.Segment = CurDAG->getRegister(Cse523::GS, MVT::i16);
                    return false;
                case 257:
                    AM.Segment = CurDAG->getRegister(Cse523::FS, MVT::i16);
                    return false;
            }

    return true;
}

/// MatchWrapper - Try to match Cse523ISD::Wrapper and Cse523ISD::WrapperRIP nodes
/// into an addressing mode.  These wrap things that will resolve down into a
/// symbol reference.  If no match is possible, this returns true, otherwise it
/// returns false.
bool Cse523DAGToDAGISel::MatchWrapper(SDValue N, Cse523ISelAddressMode &AM) {
    // If the addressing mode already has a symbol as the displacement, we can
    // never match another symbol.
    if (AM.hasSymbolicDisplacement())
        return true;

    SDValue N0 = N.getOperand(0);
    CodeModel::Model M = TM.getCodeModel();

    // Handle Cse523-64 rip-relative addresses.  We check this before checking direct
    // folding because RIP is preferable to non-RIP accesses.
    if (Subtarget->is64Bit() && N.getOpcode() == Cse523ISD::WrapperRIP &&
            // Under Cse523-64 non-small code model, GV (and friends) are 64-bits, so
            // they cannot be folded into immediate fields.
            // FIXME: This can be improved for kernel and other models?
            (M == CodeModel::Small || M == CodeModel::Kernel)) {
        // Base and index reg must be 0 in order to use %rip as base.
        if (AM.hasBaseOrIndexReg())
            return true;
        if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(N0)) {
            Cse523ISelAddressMode Backup = AM;
            AM.GV = G->getGlobal();
            AM.SymbolFlags = G->getTargetFlags();
            if (FoldOffsetIntoAddress(G->getOffset(), AM)) {
                AM = Backup;
                return true;
            }
        } else if (ConstantPoolSDNode *CP = dyn_cast<ConstantPoolSDNode>(N0)) {
            Cse523ISelAddressMode Backup = AM;
            AM.CP = CP->getConstVal();
            AM.Align = CP->getAlignment();
            AM.SymbolFlags = CP->getTargetFlags();
            if (FoldOffsetIntoAddress(CP->getOffset(), AM)) {
                AM = Backup;
                return true;
            }
        } else if (ExternalSymbolSDNode *S = dyn_cast<ExternalSymbolSDNode>(N0)) {
            AM.ES = S->getSymbol();
            AM.SymbolFlags = S->getTargetFlags();
        } else if (JumpTableSDNode *J = dyn_cast<JumpTableSDNode>(N0)) {
            AM.JT = J->getIndex();
            AM.SymbolFlags = J->getTargetFlags();
        } else if (BlockAddressSDNode *BA = dyn_cast<BlockAddressSDNode>(N0)) {
            Cse523ISelAddressMode Backup = AM;
            AM.BlockAddr = BA->getBlockAddress();
            AM.SymbolFlags = BA->getTargetFlags();
            if (FoldOffsetIntoAddress(BA->getOffset(), AM)) {
                AM = Backup;
                return true;
            }
        } else
            llvm_unreachable("Unhandled symbol reference node.");

        if (N.getOpcode() == Cse523ISD::WrapperRIP)
            AM.setBaseReg(CurDAG->getRegister(Cse523::RIP, MVT::i64));
        return false;
    }

    // Handle the case when globals fit in our immediate field: This is true for
    // Cse523-32 always and Cse523-64 when in -mcmodel=small mode.  In 64-bit
    // mode, this only applies to a non-RIP-relative computation.
    if (!Subtarget->is64Bit() ||
            M == CodeModel::Small || M == CodeModel::Kernel) {
        assert(N.getOpcode() != Cse523ISD::WrapperRIP &&
                "RIP-relative addressing already handled");
        if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(N0)) {
            AM.GV = G->getGlobal();
            AM.Disp += G->getOffset();
            AM.SymbolFlags = G->getTargetFlags();
        } else if (ConstantPoolSDNode *CP = dyn_cast<ConstantPoolSDNode>(N0)) {
            AM.CP = CP->getConstVal();
            AM.Align = CP->getAlignment();
            AM.Disp += CP->getOffset();
            AM.SymbolFlags = CP->getTargetFlags();
        } else if (ExternalSymbolSDNode *S = dyn_cast<ExternalSymbolSDNode>(N0)) {
            AM.ES = S->getSymbol();
            AM.SymbolFlags = S->getTargetFlags();
        } else if (JumpTableSDNode *J = dyn_cast<JumpTableSDNode>(N0)) {
            AM.JT = J->getIndex();
            AM.SymbolFlags = J->getTargetFlags();
        } else if (BlockAddressSDNode *BA = dyn_cast<BlockAddressSDNode>(N0)) {
            AM.BlockAddr = BA->getBlockAddress();
            AM.Disp += BA->getOffset();
            AM.SymbolFlags = BA->getTargetFlags();
        } else
            llvm_unreachable("Unhandled symbol reference node.");
        return false;
    }

    return true;
}

/// MatchAddress - Add the specified node to the specified addressing mode,
/// returning true if it cannot be done.  This just pattern matches for the
/// addressing mode.
bool Cse523DAGToDAGISel::MatchAddress(SDValue N, Cse523ISelAddressMode &AM) {
    if (MatchAddressRecursively(N, AM, 0))
        return true;

    // Post-processing: Convert lea(,%reg,2) to lea(%reg,%reg), which has
    // a smaller encoding and avoids a scaled-index.
    if (AM.Scale == 2 &&
            AM.BaseType == Cse523ISelAddressMode::RegBase &&
            AM.Base_Reg.getNode() == 0) {
        AM.Base_Reg = AM.IndexReg;
        AM.Scale = 1;
    }

    // Post-processing: Convert foo to foo(%rip), even in non-PIC mode,
    // because it has a smaller encoding.
    // TODO: Which other code models can use this?
    if (TM.getCodeModel() == CodeModel::Small &&
            Subtarget->is64Bit() &&
            AM.Scale == 1 &&
            AM.BaseType == Cse523ISelAddressMode::RegBase &&
            AM.Base_Reg.getNode() == 0 &&
            AM.IndexReg.getNode() == 0 &&
            AM.SymbolFlags == Cse523II::MO_NO_FLAG &&
            AM.hasSymbolicDisplacement())
        AM.Base_Reg = CurDAG->getRegister(Cse523::RIP, MVT::i64);

    return false;
}

// Insert a node into the DAG at least before the Pos node's position. This
// will reposition the node as needed, and will assign it a node ID that is <=
// the Pos node's ID. Note that this does *not* preserve the uniqueness of node
// IDs! The selection DAG must no longer depend on their uniqueness when this
// is used.
static void InsertDAGNode(SelectionDAG &DAG, SDValue Pos, SDValue N) {
    if (N.getNode()->getNodeId() == -1 ||
            N.getNode()->getNodeId() > Pos.getNode()->getNodeId()) {
        DAG.RepositionNode(Pos.getNode(), N.getNode());
        N.getNode()->setNodeId(Pos.getNode()->getNodeId());
    }
}

// Transform "(X >> (8-C1)) & C2" to "(X >> 8) & 0xff)" if safe. This
// allows us to convert the shift and and into an h-register extract and
// a scaled index. Returns false if the simplification is performed.
static bool FoldMaskAndShiftToExtract(SelectionDAG &DAG, SDValue N,
        uint64_t Mask,
        SDValue Shift, SDValue X,
        Cse523ISelAddressMode &AM) {
    if (Shift.getOpcode() != ISD::SRL ||
            !isa<ConstantSDNode>(Shift.getOperand(1)) ||
            !Shift.hasOneUse())
        return true;

    int ScaleLog = 8 - Shift.getConstantOperandVal(1);
    if (ScaleLog <= 0 || ScaleLog >= 4 ||
            Mask != (0xffu << ScaleLog))
        return true;

    MVT VT = N.getSimpleValueType();
    SDLoc DL(N);
    SDValue Eight = DAG.getConstant(8, MVT::i8);
    SDValue NewMask = DAG.getConstant(0xff, VT);
    SDValue Srl = DAG.getNode(ISD::SRL, DL, VT, X, Eight);
    SDValue And = DAG.getNode(ISD::AND, DL, VT, Srl, NewMask);
    SDValue ShlCount = DAG.getConstant(ScaleLog, MVT::i64);
    SDValue Shl = DAG.getNode(ISD::SHL, DL, VT, And, ShlCount);

    // Insert the new nodes into the topological ordering. We must do this in
    // a valid topological ordering as nothing is going to go back and re-sort
    // these nodes. We continually insert before 'N' in sequence as this is
    // essentially a pre-flattened and pre-sorted sequence of nodes. There is no
    // hierarchy left to express.
    InsertDAGNode(DAG, N, Eight);
    InsertDAGNode(DAG, N, Srl);
    InsertDAGNode(DAG, N, NewMask);
    InsertDAGNode(DAG, N, And);
    InsertDAGNode(DAG, N, ShlCount);
    InsertDAGNode(DAG, N, Shl);
    DAG.ReplaceAllUsesWith(N, Shl);
    AM.IndexReg = And;
    AM.Scale = (1 << ScaleLog);
    return false;
}

// Transforms "(X << C1) & C2" to "(X & (C2>>C1)) << C1" if safe and if this
// allows us to fold the shift into this addressing mode. Returns false if the
// transform succeeded.
static bool FoldMaskedShiftToScaledMask(SelectionDAG &DAG, SDValue N,
        uint64_t Mask,
        SDValue Shift, SDValue X,
        Cse523ISelAddressMode &AM) {
    if (Shift.getOpcode() != ISD::SHL ||
            !isa<ConstantSDNode>(Shift.getOperand(1)))
        return true;

    // Not likely to be profitable if either the AND or SHIFT node has more
    // than one use (unless all uses are for address computation). Besides,
    // isel mechanism requires their node ids to be reused.
    if (!N.hasOneUse() || !Shift.hasOneUse())
        return true;

    // Verify that the shift amount is something we can fold.
    unsigned ShiftAmt = Shift.getConstantOperandVal(1);
    if (ShiftAmt != 1 && ShiftAmt != 2 && ShiftAmt != 3)
        return true;

    MVT VT = N.getSimpleValueType();
    SDLoc DL(N);
    SDValue NewMask = DAG.getConstant(Mask >> ShiftAmt, VT);
    SDValue NewAnd = DAG.getNode(ISD::AND, DL, VT, X, NewMask);
    SDValue NewShift = DAG.getNode(ISD::SHL, DL, VT, NewAnd, Shift.getOperand(1));

    // Insert the new nodes into the topological ordering. We must do this in
    // a valid topological ordering as nothing is going to go back and re-sort
    // these nodes. We continually insert before 'N' in sequence as this is
    // essentially a pre-flattened and pre-sorted sequence of nodes. There is no
    // hierarchy left to express.
    InsertDAGNode(DAG, N, NewMask);
    InsertDAGNode(DAG, N, NewAnd);
    InsertDAGNode(DAG, N, NewShift);
    DAG.ReplaceAllUsesWith(N, NewShift);

    AM.Scale = 1 << ShiftAmt;
    AM.IndexReg = NewAnd;
    return false;
}

// Implement some heroics to detect shifts of masked values where the mask can
// be replaced by extending the shift and undoing that in the addressing mode
// scale. Patterns such as (shl (srl x, c1), c2) are canonicalized into (and
// (srl x, SHIFT), MASK) by DAGCombines that don't know the shl can be done in
// the addressing mode. This results in code such as:
//
//   int f(short *y, int *lookup_table) {
//     ...
//     return *y + lookup_table[*y >> 11];
//   }
//
// Turning into:
//   movzwl (%rdi), %eax
//   movl %eax, %ecx
//   shrl $11, %ecx
//   addl (%rsi,%rcx,4), %eax
//
// Instead of:
//   movzwl (%rdi), %eax
//   movl %eax, %ecx
//   shrl $9, %ecx
//   andl $124, %rcx
//   addl (%rsi,%rcx), %eax
//
// Note that this function assumes the mask is provided as a mask *after* the
// value is shifted. The input chain may or may not match that, but computing
// such a mask is trivial.
static bool FoldMaskAndShiftToScale(SelectionDAG &DAG, SDValue N,
        uint64_t Mask,
        SDValue Shift, SDValue X,
        Cse523ISelAddressMode &AM) {
    if (Shift.getOpcode() != ISD::SRL || !Shift.hasOneUse() ||
            !isa<ConstantSDNode>(Shift.getOperand(1)))
        return true;

    unsigned ShiftAmt = Shift.getConstantOperandVal(1);
    unsigned MaskLZ = countLeadingZeros(Mask);
    unsigned MaskTZ = countTrailingZeros(Mask);

    // The amount of shift we're trying to fit into the addressing mode is taken
    // from the trailing zeros of the mask.
    unsigned AMShiftAmt = MaskTZ;

    // There is nothing we can do here unless the mask is removing some bits.
    // Also, the addressing mode can only represent shifts of 1, 2, or 3 bits.
    if (AMShiftAmt <= 0 || AMShiftAmt > 3) return true;

    // We also need to ensure that mask is a continuous run of bits.
    if (CountTrailingOnes_64(Mask >> MaskTZ) + MaskTZ + MaskLZ != 64) return true;

    // Scale the leading zero count down based on the actual size of the value.
    // Also scale it down based on the size of the shift.
    MaskLZ -= (64 - X.getSimpleValueType().getSizeInBits()) + ShiftAmt;

    // The final check is to ensure that any masked out high bits of X are
    // already known to be zero. Otherwise, the mask has a semantic impact
    // other than masking out a couple of low bits. Unfortunately, because of
    // the mask, zero extensions will be removed from operands in some cases.
    // This code works extra hard to look through extensions because we can
    // replace them with zero extensions cheaply if necessary.
    bool ReplacingAnyExtend = false;
    if (X.getOpcode() == ISD::ANY_EXTEND) {
        unsigned ExtendBits = X.getSimpleValueType().getSizeInBits() -
            X.getOperand(0).getSimpleValueType().getSizeInBits();
        // Assume that we'll replace the any-extend with a zero-extend, and
        // narrow the search to the extended value.
        X = X.getOperand(0);
        MaskLZ = ExtendBits > MaskLZ ? 0 : MaskLZ - ExtendBits;
        ReplacingAnyExtend = true;
    }
    APInt MaskedHighBits =
        APInt::getHighBitsSet(X.getSimpleValueType().getSizeInBits(), MaskLZ);
    APInt KnownZero, KnownOne;
    DAG.ComputeMaskedBits(X, KnownZero, KnownOne);
    if (MaskedHighBits != KnownZero) return true;

    // We've identified a pattern that can be transformed into a single shift
    // and an addressing mode. Make it so.
    MVT VT = N.getSimpleValueType();
    if (ReplacingAnyExtend) {
        assert(X.getValueType() != VT);
        // We looked through an ANY_EXTEND node, insert a ZERO_EXTEND.
        SDValue NewX = DAG.getNode(ISD::ZERO_EXTEND, SDLoc(X), VT, X);
        InsertDAGNode(DAG, N, NewX);
        X = NewX;
    }
    SDLoc DL(N);
    SDValue NewSRLAmt = DAG.getConstant(ShiftAmt + AMShiftAmt, MVT::i8);
    SDValue NewSRL = DAG.getNode(ISD::SRL, DL, VT, X, NewSRLAmt);
    SDValue NewSHLAmt = DAG.getConstant(AMShiftAmt, MVT::i64);
    SDValue NewSHL = DAG.getNode(ISD::SHL, DL, VT, NewSRL, NewSHLAmt);

    // Insert the new nodes into the topological ordering. We must do this in
    // a valid topological ordering as nothing is going to go back and re-sort
    // these nodes. We continually insert before 'N' in sequence as this is
    // essentially a pre-flattened and pre-sorted sequence of nodes. There is no
    // hierarchy left to express.
    InsertDAGNode(DAG, N, NewSRLAmt);
    InsertDAGNode(DAG, N, NewSRL);
    InsertDAGNode(DAG, N, NewSHLAmt);
    InsertDAGNode(DAG, N, NewSHL);
    DAG.ReplaceAllUsesWith(N, NewSHL);

    AM.Scale = 1 << AMShiftAmt;
    AM.IndexReg = NewSRL;
    return false;
}

bool Cse523DAGToDAGISel::MatchAddressRecursively(SDValue N, Cse523ISelAddressMode &AM,
        unsigned Depth) {
    SDLoc dl(N);
    DEBUG({
            dbgs() << "MatchAddress: ";
            AM.dump();
            });
    // Limit recursion.
    if (Depth > 5)
        return MatchAddressBase(N, AM);

    // If this is already a %rip relative address, we can only merge immediates
    // into it.  Instead of handling this in every case, we handle it here.
    // RIP relative addressing: %rip + 32-bit displacement!
    if (AM.isRIPRelative()) {
        // FIXME: JumpTable and ExternalSymbol address currently don't like
        // displacements.  It isn't very important, but this should be fixed for
        // consistency.
        if (!AM.ES && AM.JT != -1) return true;

        if (ConstantSDNode *Cst = dyn_cast<ConstantSDNode>(N))
            if (!FoldOffsetIntoAddress(Cst->getSExtValue(), AM))
                return false;
        return true;
    }

    switch (N.getOpcode()) {
        default: break;
        case ISD::Constant: {
                                uint64_t Val = cast<ConstantSDNode>(N)->getSExtValue();
                                if (!FoldOffsetIntoAddress(Val, AM))
                                    return false;
                                break;
                            }

        case Cse523ISD::Wrapper:
        case Cse523ISD::WrapperRIP:
                 if (!MatchWrapper(N, AM))
                     return false;
                 break;

        case ISD::LOAD:
                 if (!MatchLoadInAddress(cast<LoadSDNode>(N), AM))
                     return false;
                 break;

        case ISD::FrameIndex:
                 if (AM.BaseType == Cse523ISelAddressMode::RegBase &&
                         AM.Base_Reg.getNode() == 0 &&
                         (!Subtarget->is64Bit() || isDispSafeForFrameIndex(AM.Disp))) {
                     AM.BaseType = Cse523ISelAddressMode::FrameIndexBase;
                     AM.Base_FrameIndex = cast<FrameIndexSDNode>(N)->getIndex();
                     return false;
                 }
                 break;

        case ISD::SHL:
                 if (AM.IndexReg.getNode() != 0 || AM.Scale != 1)
                     break;

                 if (ConstantSDNode
                         *CN = dyn_cast<ConstantSDNode>(N.getNode()->getOperand(1))) {
                     unsigned Val = CN->getZExtValue();
                     // Note that we handle x<<1 as (,x,2) rather than (x,x) here so
                     // that the base operand remains free for further matching. If
                     // the base doesn't end up getting used, a post-processing step
                     // in MatchAddress turns (,x,2) into (x,x), which is cheaper.
                     if (Val == 1 || Val == 2 || Val == 3) {
                         AM.Scale = 1 << Val;
                         SDValue ShVal = N.getNode()->getOperand(0);

                         // Okay, we know that we have a scale by now.  However, if the scaled
                         // value is an add of something and a constant, we can fold the
                         // constant into the disp field here.
                         if (CurDAG->isBaseWithConstantOffset(ShVal)) {
                             AM.IndexReg = ShVal.getNode()->getOperand(0);
                             ConstantSDNode *AddVal =
                                 cast<ConstantSDNode>(ShVal.getNode()->getOperand(1));
                             uint64_t Disp = (uint64_t)AddVal->getSExtValue() << Val;
                             if (!FoldOffsetIntoAddress(Disp, AM))
                                 return false;
                         }

                         AM.IndexReg = ShVal;
                         return false;
                     }
                 }
                 break;

        case ISD::SRL: {
                           // Scale must not be used already.
                           if (AM.IndexReg.getNode() != 0 || AM.Scale != 1) break;

                           SDValue And = N.getOperand(0);
                           if (And.getOpcode() != ISD::AND) break;
                           SDValue X = And.getOperand(0);

                           // We only handle up to 64-bit values here as those are what matter for
                           // addressing mode optimizations.
                           if (X.getSimpleValueType().getSizeInBits() > 64) break;

                           // The mask used for the transform is expected to be post-shift, but we
                           // found the shift first so just apply the shift to the mask before passing
                           // it down.
                           if (!isa<ConstantSDNode>(N.getOperand(1)) ||
                                   !isa<ConstantSDNode>(And.getOperand(1)))
                               break;
                           uint64_t Mask = And.getConstantOperandVal(1) >> N.getConstantOperandVal(1);

                           // Try to fold the mask and shift into the scale, and return false if we
                           // succeed.
                           if (!FoldMaskAndShiftToScale(*CurDAG, N, Mask, N, X, AM))
                               return false;
                           break;
                       }

        case ISD::SMUL_LOHI:
        case ISD::UMUL_LOHI:
                       // A mul_lohi where we need the low part can be folded as a plain multiply.
                       if (N.getResNo() != 0) break;
                       // FALL THROUGH
        case ISD::MUL:
        case Cse523ISD::MUL_IMM:
                       // X*[3,5,9] -> X+X*[2,4,8]
                       if (AM.BaseType == Cse523ISelAddressMode::RegBase &&
                               AM.Base_Reg.getNode() == 0 &&
                               AM.IndexReg.getNode() == 0) {
                           if (ConstantSDNode
                                   *CN = dyn_cast<ConstantSDNode>(N.getNode()->getOperand(1)))
                               if (CN->getZExtValue() == 3 || CN->getZExtValue() == 5 ||
                                       CN->getZExtValue() == 9) {
                                   AM.Scale = unsigned(CN->getZExtValue())-1;

                                   SDValue MulVal = N.getNode()->getOperand(0);
                                   SDValue Reg;

                                   // Okay, we know that we have a scale by now.  However, if the scaled
                                   // value is an add of something and a constant, we can fold the
                                   // constant into the disp field here.
                                   if (MulVal.getNode()->getOpcode() == ISD::ADD && MulVal.hasOneUse() &&
                                           isa<ConstantSDNode>(MulVal.getNode()->getOperand(1))) {
                                       Reg = MulVal.getNode()->getOperand(0);
                                       ConstantSDNode *AddVal =
                                           cast<ConstantSDNode>(MulVal.getNode()->getOperand(1));
                                       uint64_t Disp = AddVal->getSExtValue() * CN->getZExtValue();
                                       if (FoldOffsetIntoAddress(Disp, AM))
                                           Reg = N.getNode()->getOperand(0);
                                   } else {
                                       Reg = N.getNode()->getOperand(0);
                                   }

                                   AM.IndexReg = AM.Base_Reg = Reg;
                                   return false;
                               }
                       }
                       break;

        case ISD::SUB: {
                           // Given A-B, if A can be completely folded into the address and
                           // the index field with the index field unused, use -B as the index.
                           // This is a win if a has multiple parts that can be folded into
                           // the address. Also, this saves a mov if the base register has
                           // other uses, since it avoids a two-address sub instruction, however
                           // it costs an additional mov if the index register has other uses.

                           // Add an artificial use to this node so that we can keep track of
                           // it if it gets CSE'd with a different node.
                           HandleSDNode Handle(N);

                           // Test if the LHS of the sub can be folded.
                           Cse523ISelAddressMode Backup = AM;
                           if (MatchAddressRecursively(N.getNode()->getOperand(0), AM, Depth+1)) {
                               AM = Backup;
                               break;
                           }
                           // Test if the index field is free for use.
                           if (AM.IndexReg.getNode() || AM.isRIPRelative()) {
                               AM = Backup;
                               break;
                           }

                           int Cost = 0;
                           SDValue RHS = Handle.getValue().getNode()->getOperand(1);
                           // If the RHS involves a register with multiple uses, this
                           // transformation incurs an extra mov, due to the neg instruction
                           // clobbering its operand.
                           if (!RHS.getNode()->hasOneUse() ||
                                   RHS.getNode()->getOpcode() == ISD::CopyFromReg ||
                                   RHS.getNode()->getOpcode() == ISD::TRUNCATE ||
                                   RHS.getNode()->getOpcode() == ISD::ANY_EXTEND ||
                                   (RHS.getNode()->getOpcode() == ISD::ZERO_EXTEND &&
                                    RHS.getNode()->getOperand(0).getValueType() == MVT::i32))
                               ++Cost;
                           // If the base is a register with multiple uses, this
                           // transformation may save a mov.
                           if ((AM.BaseType == Cse523ISelAddressMode::RegBase &&
                                       AM.Base_Reg.getNode() &&
                                       !AM.Base_Reg.getNode()->hasOneUse()) ||
                                   AM.BaseType == Cse523ISelAddressMode::FrameIndexBase)
                               --Cost;
                           // If the folded LHS was interesting, this transformation saves
                           // address arithmetic.
                           if ((AM.hasSymbolicDisplacement() && !Backup.hasSymbolicDisplacement()) +
                                   ((AM.Disp != 0) && (Backup.Disp == 0)) +
                                   (AM.Segment.getNode() && !Backup.Segment.getNode()) >= 2)
                               --Cost;
                           // If it doesn't look like it may be an overall win, don't do it.
                           if (Cost >= 0) {
                               AM = Backup;
                               break;
                           }

                           // Ok, the transformation is legal and appears profitable. Go for it.
                           SDValue Zero = CurDAG->getConstant(0, N.getValueType());
                           SDValue Neg = CurDAG->getNode(ISD::SUB, dl, N.getValueType(), Zero, RHS);
                           AM.IndexReg = Neg;
                           AM.Scale = 1;

                           // Insert the new nodes into the topological ordering.
                           InsertDAGNode(*CurDAG, N, Zero);
                           InsertDAGNode(*CurDAG, N, Neg);
                           return false;
                       }

        case ISD::ADD: {
                           // Add an artificial use to this node so that we can keep track of
                           // it if it gets CSE'd with a different node.
                           HandleSDNode Handle(N);

                           Cse523ISelAddressMode Backup = AM;
                           if (!MatchAddressRecursively(N.getOperand(0), AM, Depth+1) &&
                                   !MatchAddressRecursively(Handle.getValue().getOperand(1), AM, Depth+1))
                               return false;
                           AM = Backup;

                           // Try again after commuting the operands.
                           if (!MatchAddressRecursively(Handle.getValue().getOperand(1), AM, Depth+1)&&
                                   !MatchAddressRecursively(Handle.getValue().getOperand(0), AM, Depth+1))
                               return false;
                           AM = Backup;

                           // If we couldn't fold both operands into the address at the same time,
                           // see if we can just put each operand into a register and fold at least
                           // the add.
                           if (AM.BaseType == Cse523ISelAddressMode::RegBase &&
                                   !AM.Base_Reg.getNode() &&
                                   !AM.IndexReg.getNode()) {
                               N = Handle.getValue();
                               AM.Base_Reg = N.getOperand(0);
                               AM.IndexReg = N.getOperand(1);
                               AM.Scale = 1;
                               return false;
                           }
                           N = Handle.getValue();
                           break;
                       }

        case ISD::OR:
                       // Handle "X | C" as "X + C" iff X is known to have C bits clear.
                       if (CurDAG->isBaseWithConstantOffset(N)) {
                           Cse523ISelAddressMode Backup = AM;
                           ConstantSDNode *CN = cast<ConstantSDNode>(N.getOperand(1));

                           // Start with the LHS as an addr mode.
                           if (!MatchAddressRecursively(N.getOperand(0), AM, Depth+1) &&
                                   !FoldOffsetIntoAddress(CN->getSExtValue(), AM))
                               return false;
                           AM = Backup;
                       }
                       break;

        case ISD::AND: {
                           // Perform some heroic transforms on an and of a constant-count shift
                           // with a constant to enable use of the scaled offset field.

                           // Scale must not be used already.
                           if (AM.IndexReg.getNode() != 0 || AM.Scale != 1) break;

                           SDValue Shift = N.getOperand(0);
                           if (Shift.getOpcode() != ISD::SRL && Shift.getOpcode() != ISD::SHL) break;
                           SDValue X = Shift.getOperand(0);

                           // We only handle up to 64-bit values here as those are what matter for
                           // addressing mode optimizations.
                           if (X.getSimpleValueType().getSizeInBits() > 64) break;

                           if (!isa<ConstantSDNode>(N.getOperand(1)))
                               break;
                           uint64_t Mask = N.getConstantOperandVal(1);

                           // Try to fold the mask and shift into an extract and scale.
                           if (!FoldMaskAndShiftToExtract(*CurDAG, N, Mask, Shift, X, AM))
                               return false;

                           // Try to fold the mask and shift directly into the scale.
                           if (!FoldMaskAndShiftToScale(*CurDAG, N, Mask, Shift, X, AM))
                               return false;

                           // Try to swap the mask and shift to place shifts which can be done as
                           // a scale on the outside of the mask.
                           if (!FoldMaskedShiftToScaledMask(*CurDAG, N, Mask, Shift, X, AM))
                               return false;
                           break;
                       }
    }

    return MatchAddressBase(N, AM);
}

/// MatchAddressBase - Helper for MatchAddress. Add the specified node to the
/// specified addressing mode without any further recursion.
bool Cse523DAGToDAGISel::MatchAddressBase(SDValue N, Cse523ISelAddressMode &AM) {
    // Is the base register already occupied?
    if (AM.BaseType != Cse523ISelAddressMode::RegBase || AM.Base_Reg.getNode()) {
        // If so, check to see if the scale index register is set.
        if (AM.IndexReg.getNode() == 0) {
            AM.IndexReg = N;
            AM.Scale = 1;
            return false;
        }

        // Otherwise, we cannot select it.
        return true;
    }

    // Default, generate it as a register.
    AM.BaseType = Cse523ISelAddressMode::RegBase;
    AM.Base_Reg = N;
    return false;
}

/// SelectAddr - returns true if it is able pattern match an addressing mode.
/// It returns the operands which make up the maximal addressing mode it can
/// match by reference.
///
/// Parent is the parent node of the addr operand that is being matched.  It
/// is always a load, store, atomic node, or null.  It is only null when
/// checking memory operands for inline asm nodes.
bool Cse523DAGToDAGISel::SelectAddr(SDNode *Parent, SDValue N, SDValue &Base,
        SDValue &Scale, SDValue &Index,
        SDValue &Disp, SDValue &Segment) {
    Cse523ISelAddressMode AM;

    if (Parent &&
            // This list of opcodes are all the nodes that have an "addr:$ptr" operand
            // that are not a MemSDNode, and thus don't have proper addrspace info.
            Parent->getOpcode() != ISD::INTRINSIC_W_CHAIN && // unaligned loads, fixme
            Parent->getOpcode() != ISD::INTRINSIC_VOID && // nontemporal stores
            Parent->getOpcode() != Cse523ISD::TLSCALL && // Fixme
            Parent->getOpcode() != Cse523ISD::EH_SJLJ_SETJMP && // setjmp
            Parent->getOpcode() != Cse523ISD::EH_SJLJ_LONGJMP) { // longjmp
        unsigned AddrSpace =
            cast<MemSDNode>(Parent)->getPointerInfo().getAddrSpace();
        // AddrSpace 256 -> GS, 257 -> FS.
        if (AddrSpace == 256)
            AM.Segment = CurDAG->getRegister(Cse523::GS, MVT::i16);
        if (AddrSpace == 257)
            AM.Segment = CurDAG->getRegister(Cse523::FS, MVT::i16);
    }

    if (MatchAddress(N, AM))
        return false;

    MVT VT = N.getSimpleValueType();
    if (AM.BaseType == Cse523ISelAddressMode::RegBase) {
        if (!AM.Base_Reg.getNode())
            AM.Base_Reg = CurDAG->getRegister(0, VT);
    }

    if (!AM.IndexReg.getNode())
        AM.IndexReg = CurDAG->getRegister(0, VT);

    getAddressOperands(AM, Base, Scale, Index, Disp, Segment);
    return true;
}

/// SelectScalarSSELoad - Match a scalar SSE load.  In particular, we want to
/// match a load whose top elements are either undef or zeros.  The load flavor
/// is derived from the type of N, which is either v4f32 or v2f64.
///
/// We also return:
///   PatternChainNode: this is the matched node that has a chain input and
///   output.
bool Cse523DAGToDAGISel::SelectScalarSSELoad(SDNode *Root,
        SDValue N, SDValue &Base,
        SDValue &Scale, SDValue &Index,
        SDValue &Disp, SDValue &Segment,
        SDValue &PatternNodeWithChain) {
    if (N.getOpcode() == ISD::SCALAR_TO_VECTOR) {
        PatternNodeWithChain = N.getOperand(0);
        if (ISD::isNON_EXTLoad(PatternNodeWithChain.getNode()) &&
                PatternNodeWithChain.hasOneUse() &&
                IsProfitableToFold(N.getOperand(0), N.getNode(), Root) &&
                IsLegalToFold(N.getOperand(0), N.getNode(), Root, OptLevel)) {
            LoadSDNode *LD = cast<LoadSDNode>(PatternNodeWithChain);
            if (!SelectAddr(LD, LD->getBasePtr(), Base, Scale, Index, Disp, Segment))
                return false;
            return true;
        }
    }

    // Also handle the case where we explicitly require zeros in the top
    // elements.  This is a vector shuffle from the zero vector.
    if (N.getOpcode() == Cse523ISD::VZEXT_MOVL && N.getNode()->hasOneUse() &&
            // Check to see if the top elements are all zeros (or bitcast of zeros).
            N.getOperand(0).getOpcode() == ISD::SCALAR_TO_VECTOR &&
            N.getOperand(0).getNode()->hasOneUse() &&
            ISD::isNON_EXTLoad(N.getOperand(0).getOperand(0).getNode()) &&
            N.getOperand(0).getOperand(0).hasOneUse() &&
            IsProfitableToFold(N.getOperand(0), N.getNode(), Root) &&
            IsLegalToFold(N.getOperand(0), N.getNode(), Root, OptLevel)) {
        // Okay, this is a zero extending load.  Fold it.
        LoadSDNode *LD = cast<LoadSDNode>(N.getOperand(0).getOperand(0));
        if (!SelectAddr(LD, LD->getBasePtr(), Base, Scale, Index, Disp, Segment))
            return false;
        PatternNodeWithChain = SDValue(LD, 0);
        return true;
    }
    return false;
}


bool Cse523DAGToDAGISel::SelectMOV64Imm32(SDValue N, SDValue &Imm) {
    if (const ConstantSDNode *CN = dyn_cast<ConstantSDNode>(N)) {
        uint64_t ImmVal = CN->getZExtValue();
        if ((uint32_t)ImmVal != (uint64_t)ImmVal)
            return false;

        Imm = CurDAG->getTargetConstant(ImmVal, MVT::i64);
        return true;
    }

    // In static codegen with small code model, we can get the address of a label
    // into a register with 'movl'. TableGen has already made sure we're looking
    // at a label of some kind.
    assert(N->getOpcode() == Cse523ISD::Wrapper &&
            "Unexpected node type for MOV64ri64");
    N = N.getOperand(0);

    if (N->getOpcode() != ISD::TargetConstantPool &&
            N->getOpcode() != ISD::TargetJumpTable &&
            N->getOpcode() != ISD::TargetGlobalAddress &&
            N->getOpcode() != ISD::TargetExternalSymbol &&
            N->getOpcode() != ISD::TargetBlockAddress)
        return false;

    Imm = N;
    return TM.getCodeModel() == CodeModel::Small;
}

bool Cse523DAGToDAGISel::SelectLEA64_32Addr(SDValue N, SDValue &Base,
        SDValue &Scale, SDValue &Index,
        SDValue &Disp, SDValue &Segment) {
    if (!SelectLEAAddr(N, Base, Scale, Index, Disp, Segment))
        return false;

    SDLoc DL(N);
    RegisterSDNode *RN = dyn_cast<RegisterSDNode>(Base);
    if (RN && RN->getReg() == 0)
        Base = CurDAG->getRegister(0, MVT::i64);
    else if (Base.getValueType() == MVT::i32 && !dyn_cast<FrameIndexSDNode>(N)) {
        llvm_unreachable("32 bit addresses not supported");
        // Base could already be %rip, particularly in the x32 ABI.
        //Base = SDValue(CurDAG->getMachineNode(
        //            TargetOpcode::SUBREG_TO_REG, DL, MVT::i64,
        //            CurDAG->getTargetConstant(0, MVT::i64),
        //            Base,
        //            CurDAG->getTargetConstant(Cse523::sub_32bit, MVT::i32)),
        //        0);
    }

    RN = dyn_cast<RegisterSDNode>(Index);
    if (RN && RN->getReg() == 0)
        Index = CurDAG->getRegister(0, MVT::i64);
    else {
        assert(Index.getValueType() == MVT::i32 &&
                "Expect to be extending 32-bit registers for use in LEA");
        llvm_unreachable("32 bit addresses not supported");
        //Index = SDValue(CurDAG->getMachineNode(
        //            TargetOpcode::SUBREG_TO_REG, DL, MVT::i64,
        //            CurDAG->getTargetConstant(0, MVT::i64),
        //            Index,
        //            CurDAG->getTargetConstant(Cse523::sub_32bit, MVT::i32)),
        //        0);
    }

    return true;
}

/// SelectLEAAddr - it calls SelectAddr and determines if the maximal addressing
/// mode it matches can be cost effectively emitted as an LEA instruction.
bool Cse523DAGToDAGISel::SelectLEAAddr(SDValue N,
        SDValue &Base, SDValue &Scale,
        SDValue &Index, SDValue &Disp,
        SDValue &Segment) {
    Cse523ISelAddressMode AM;

    // Set AM.Segment to prevent MatchAddress from using one. LEA doesn't support
    // segments.
    SDValue Copy = AM.Segment;
    SDValue T = CurDAG->getRegister(0, MVT::i32);
    AM.Segment = T;
    if (MatchAddress(N, AM))
        return false;
    assert (T == AM.Segment);
    AM.Segment = Copy;

    MVT VT = N.getSimpleValueType();
    unsigned Complexity = 0;
    if (AM.BaseType == Cse523ISelAddressMode::RegBase)
        if (AM.Base_Reg.getNode())
            Complexity = 1;
        else
            AM.Base_Reg = CurDAG->getRegister(0, VT);
    else if (AM.BaseType == Cse523ISelAddressMode::FrameIndexBase)
        Complexity = 4;

    if (AM.IndexReg.getNode())
        Complexity++;
    else
        AM.IndexReg = CurDAG->getRegister(0, VT);

    // Don't match just leal(,%reg,2). It's cheaper to do addl %reg, %reg, or with
    // a simple shift.
    if (AM.Scale > 1)
        Complexity++;

    // FIXME: We are artificially lowering the criteria to turn ADD %reg, $GA
    // to a LEA. This is determined with some expermentation but is by no means
    // optimal (especially for code size consideration). LEA is nice because of
    // its three-address nature. Tweak the cost function again when we can run
    // convertToThreeAddress() at register allocation time.
    if (AM.hasSymbolicDisplacement()) {
        // For Cse523-64, we should always use lea to materialize RIP relative
        // addresses.
        if (Subtarget->is64Bit())
            Complexity = 4;
        else
            Complexity += 2;
    }

    if (AM.Disp && (AM.Base_Reg.getNode() || AM.IndexReg.getNode()))
        Complexity++;

    // If it isn't worth using an LEA, reject it.
    if (Complexity <= 2)
        return false;

    getAddressOperands(AM, Base, Scale, Index, Disp, Segment);
    return true;
}

/// SelectTLSADDRAddr - This is only run on TargetGlobalTLSAddress nodes.
bool Cse523DAGToDAGISel::SelectTLSADDRAddr(SDValue N, SDValue &Base,
        SDValue &Scale, SDValue &Index,
        SDValue &Disp, SDValue &Segment) {
    assert(N.getOpcode() == ISD::TargetGlobalTLSAddress);
    const GlobalAddressSDNode *GA = cast<GlobalAddressSDNode>(N);

    Cse523ISelAddressMode AM;
    AM.GV = GA->getGlobal();
    AM.Disp += GA->getOffset();
    AM.Base_Reg = CurDAG->getRegister(0, N.getValueType());
    AM.SymbolFlags = GA->getTargetFlags();

    if (N.getValueType() == MVT::i32) {
        llvm_unreachable("32 bit addresses not supported");
        //AM.Scale = 1;
        //AM.IndexReg = CurDAG->getRegister(Cse523::EBX, MVT::i32);
    } else {
        AM.IndexReg = CurDAG->getRegister(0, MVT::i64);
    }

    getAddressOperands(AM, Base, Scale, Index, Disp, Segment);
    return true;
}


bool Cse523DAGToDAGISel::TryFoldLoad(SDNode *P, SDValue N,
        SDValue &Base, SDValue &Scale,
        SDValue &Index, SDValue &Disp,
        SDValue &Segment) {
    if (!ISD::isNON_EXTLoad(N.getNode()) ||
            !IsProfitableToFold(N, P, P) ||
            !IsLegalToFold(N, P, P, OptLevel))
        return false;

    return SelectAddr(N.getNode(),
            N.getOperand(1), Base, Scale, Index, Disp, Segment);
}

/// getGlobalBaseReg - Return an SDNode that returns the value of
/// the global base register. Output instructions required to
/// initialize the global base register, if necessary.
///
SDNode *Cse523DAGToDAGISel::getGlobalBaseReg() {
    unsigned GlobalBaseReg = getInstrInfo()->getGlobalBaseReg(MF);
    return CurDAG->getRegister(GlobalBaseReg,
            getTargetLowering()->getPointerTy()).getNode();
}

SDNode *Cse523DAGToDAGISel::SelectAtomic64(SDNode *Node, unsigned Opc) {
    SDValue Chain = Node->getOperand(0);
    SDValue In1 = Node->getOperand(1);
    SDValue In2L = Node->getOperand(2);
    SDValue In2H = Node->getOperand(3);

    SDValue Tmp0, Tmp1, Tmp2, Tmp3, Tmp4;
    if (!SelectAddr(Node, In1, Tmp0, Tmp1, Tmp2, Tmp3, Tmp4))
        return NULL;
    MachineSDNode::mmo_iterator MemOp = MF->allocateMemRefsArray(1);
    MemOp[0] = cast<MemSDNode>(Node)->getMemOperand();
    const SDValue Ops[] = { Tmp0, Tmp1, Tmp2, Tmp3, Tmp4, In2L, In2H, Chain};
    SDNode *ResNode = CurDAG->getMachineNode(Opc, SDLoc(Node),
            MVT::i32, MVT::i32, MVT::Other, Ops);
    cast<MachineSDNode>(ResNode)->setMemRefs(MemOp, MemOp + 1);
    return ResNode;
}

/// Atomic opcode table
///
enum AtomicOpc {
    ADD,
    SUB,
    INC,
    DEC,
    OR,
    AND,
    XOR,
    AtomicOpcEnd
};

enum AtomicSz {
    ConstantI8,
    I8,
    SextConstantI16,
    ConstantI16,
    I16,
    SextConstantI32,
    ConstantI32,
    I32,
    SextConstantI64,
    ConstantI64,
    I64,
    AtomicSzEnd
};

static const uint16_t AtomicOpcTbl[AtomicOpcEnd][AtomicSzEnd] = {
//    {
//        Cse523::LOCK_ADD8mi,
//        Cse523::LOCK_ADD8mr,
//        Cse523::LOCK_ADD16mi8,
//        Cse523::LOCK_ADD16mi,
//        Cse523::LOCK_ADD16mr,
//        Cse523::LOCK_ADD32mi8,
//        Cse523::LOCK_ADD32mi,
//        Cse523::LOCK_ADD32mr,
//        Cse523::LOCK_ADD64mi8,
//        Cse523::LOCK_ADD64mi32,
//        Cse523::LOCK_ADD64mr,
//    },
//    {
//        Cse523::LOCK_SUB8mi,
//        Cse523::LOCK_SUB8mr,
//        Cse523::LOCK_SUB16mi8,
//        Cse523::LOCK_SUB16mi,
//        Cse523::LOCK_SUB16mr,
//        Cse523::LOCK_SUB32mi8,
//        Cse523::LOCK_SUB32mi,
//        Cse523::LOCK_SUB32mr,
//        Cse523::LOCK_SUB64mi8,
//        Cse523::LOCK_SUB64mi32,
//        Cse523::LOCK_SUB64mr,
//    },
//    {
//        0,
//        Cse523::LOCK_INC8m,
//        0,
//        0,
//        Cse523::LOCK_INC16m,
//        0,
//        0,
//        Cse523::LOCK_INC32m,
//        0,
//        0,
//        Cse523::LOCK_INC64m,
//    },
//    {
//        0,
//        Cse523::LOCK_DEC8m,
//        0,
//        0,
//        Cse523::LOCK_DEC16m,
//        0,
//        0,
//        Cse523::LOCK_DEC32m,
//        0,
//        0,
//        Cse523::LOCK_DEC64m,
//    },
//    {
//        Cse523::LOCK_OR8mi,
//        Cse523::LOCK_OR8mr,
//        Cse523::LOCK_OR16mi8,
//        Cse523::LOCK_OR16mi,
//        Cse523::LOCK_OR16mr,
//        Cse523::LOCK_OR32mi8,
//        Cse523::LOCK_OR32mi,
//        Cse523::LOCK_OR32mr,
//        Cse523::LOCK_OR64mi8,
//        Cse523::LOCK_OR64mi32,
//        Cse523::LOCK_OR64mr,
//    },
//    {
//        Cse523::LOCK_AND8mi,
//        Cse523::LOCK_AND8mr,
//        Cse523::LOCK_AND16mi8,
//        Cse523::LOCK_AND16mi,
//        Cse523::LOCK_AND16mr,
//        Cse523::LOCK_AND32mi8,
//        Cse523::LOCK_AND32mi,
//        Cse523::LOCK_AND32mr,
//        Cse523::LOCK_AND64mi8,
//        Cse523::LOCK_AND64mi32,
//        Cse523::LOCK_AND64mr,
//    },
//    {
//        Cse523::LOCK_XOR8mi,
//        Cse523::LOCK_XOR8mr,
//        Cse523::LOCK_XOR16mi8,
//        Cse523::LOCK_XOR16mi,
//        Cse523::LOCK_XOR16mr,
//        Cse523::LOCK_XOR32mi8,
//        Cse523::LOCK_XOR32mi,
//        Cse523::LOCK_XOR32mr,
//        Cse523::LOCK_XOR64mi8,
//        Cse523::LOCK_XOR64mi32,
//        Cse523::LOCK_XOR64mr,
//    }
};

// Return the target constant operand for atomic-load-op and do simple
// translations, such as from atomic-load-add to lock-sub. The return value is
// one of the following 3 cases:
// + target-constant, the operand could be supported as a target constant.
// + empty, the operand is not needed any more with the new op selected.
// + non-empty, otherwise.
static SDValue getAtomicLoadArithTargetConstant(SelectionDAG *CurDAG,
        SDLoc dl,
        enum AtomicOpc &Op, MVT NVT,
        SDValue Val) {
    if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Val)) {
        int64_t CNVal = CN->getSExtValue();
        // Quit if not 32-bit imm.
        if ((int32_t)CNVal != CNVal)
            return Val;
        // For atomic-load-add, we could do some optimizations.
        if (Op == ADD) {
            // Translate to INC/DEC if ADD by 1 or -1.
            if ((CNVal == 1) || (CNVal == -1)) {
                Op = (CNVal == 1) ? INC : DEC;
                // No more constant operand after being translated into INC/DEC.
                return SDValue();
            }
            // Translate to SUB if ADD by negative value.
            if (CNVal < 0) {
                Op = SUB;
                CNVal = -CNVal;
            }
        }
        return CurDAG->getTargetConstant(CNVal, NVT);
    }

    // If the value operand is single-used, try to optimize it.
    if (Op == ADD && Val.hasOneUse()) {
        // Translate (atomic-load-add ptr (sub 0 x)) back to (lock-sub x).
        if (Val.getOpcode() == ISD::SUB && Cse523::isZeroNode(Val.getOperand(0))) {
            Op = SUB;
            return Val.getOperand(1);
        }
        // A special case for i16, which needs truncating as, in most cases, it's
        // promoted to i32. We will translate
        // (atomic-load-add (truncate (sub 0 x))) to (lock-sub (EXTRACT_SUBREG x))
        if (Val.getOpcode() == ISD::TRUNCATE && NVT == MVT::i16 &&
                Val.getOperand(0).getOpcode() == ISD::SUB &&
                Cse523::isZeroNode(Val.getOperand(0).getOperand(0))) {
            Op = SUB;
            Val = Val.getOperand(0);
            assert(0);
            //return CurDAG->getTargetExtractSubreg(Cse523::sub_16bit, dl, NVT,
            //        Val.getOperand(1));
        }
    }

    return Val;
}

SDNode *Cse523DAGToDAGISel::SelectAtomicLoadArith(SDNode *Node, MVT NVT) {
    if (Node->hasAnyUseOfValue(0))
        return 0;

    SDLoc dl(Node);

    // Optimize common patterns for __sync_or_and_fetch and similar arith
    // operations where the result is not used. This allows us to use the "lock"
    // version of the arithmetic instruction.
    SDValue Chain = Node->getOperand(0);
    SDValue Ptr = Node->getOperand(1);
    SDValue Val = Node->getOperand(2);
    SDValue Tmp0, Tmp1, Tmp2, Tmp3, Tmp4;
    if (!SelectAddr(Node, Ptr, Tmp0, Tmp1, Tmp2, Tmp3, Tmp4))
        return 0;

    // Which index into the table.
    enum AtomicOpc Op;
    switch (Node->getOpcode()) {
        default:
            return 0;
        case ISD::ATOMIC_LOAD_OR:
            Op = OR;
            break;
        case ISD::ATOMIC_LOAD_AND:
            Op = AND;
            break;
        case ISD::ATOMIC_LOAD_XOR:
            Op = XOR;
            break;
        case ISD::ATOMIC_LOAD_ADD:
            Op = ADD;
            break;
    }

    Val = getAtomicLoadArithTargetConstant(CurDAG, dl, Op, NVT, Val);
    bool isUnOp = !Val.getNode();
    bool isCN = Val.getNode() && (Val.getOpcode() == ISD::TargetConstant);

    llvm_unreachable("Atomic instructions not supported");
    unsigned Opc = 0;
    switch (NVT.SimpleTy) {
        default: return 0;
        case MVT::i8:
                 if (isCN)
                     Opc = AtomicOpcTbl[Op][ConstantI8];
                 else
                     Opc = AtomicOpcTbl[Op][I8];
                 break;
        case MVT::i16:
                 if (isCN) {
                     if (immSext8(Val.getNode()))
                         Opc = AtomicOpcTbl[Op][SextConstantI16];
                     else
                         Opc = AtomicOpcTbl[Op][ConstantI16];
                 } else
                     Opc = AtomicOpcTbl[Op][I16];
                 break;
        case MVT::i32:
                 if (isCN) {
                     if (immSext8(Val.getNode()))
                         Opc = AtomicOpcTbl[Op][SextConstantI32];
                     else
                         Opc = AtomicOpcTbl[Op][ConstantI32];
                 } else
                     Opc = AtomicOpcTbl[Op][I32];
                 break;
        case MVT::i64:
                 Opc = AtomicOpcTbl[Op][I64];
                 if (isCN) {
                     if (immSext8(Val.getNode()))
                         Opc = AtomicOpcTbl[Op][SextConstantI64];
                     else if (i64immSExt32(Val.getNode()))
                         Opc = AtomicOpcTbl[Op][ConstantI64];
                 }
                 break;
    }

    assert(Opc != 0 && "Invalid arith lock transform!");

    SDValue Ret;
    SDValue Undef = SDValue(CurDAG->getMachineNode(TargetOpcode::IMPLICIT_DEF,
                dl, NVT), 0);
    MachineSDNode::mmo_iterator MemOp = MF->allocateMemRefsArray(1);
    MemOp[0] = cast<MemSDNode>(Node)->getMemOperand();
    if (isUnOp) {
        SDValue Ops[] = { Tmp0, Tmp1, Tmp2, Tmp3, Tmp4, Chain };
        Ret = SDValue(CurDAG->getMachineNode(Opc, dl, MVT::Other, Ops), 0);
    } else {
        SDValue Ops[] = { Tmp0, Tmp1, Tmp2, Tmp3, Tmp4, Val, Chain };
        Ret = SDValue(CurDAG->getMachineNode(Opc, dl, MVT::Other, Ops), 0);
    }
    cast<MachineSDNode>(Ret)->setMemRefs(MemOp, MemOp + 1);
    SDValue RetVals[] = { Undef, Ret };
    return CurDAG->getMergeValues(RetVals, 2, dl).getNode();
}

/// HasNoSignedComparisonUses - Test whether the given Cse523ISD::CMP node has
/// any uses which require the SF or OF bits to be accurate.
static bool HasNoSignedComparisonUses(SDNode *N) {
    // Examine each user of the node.
    for (SDNode::use_iterator UI = N->use_begin(),
            UE = N->use_end(); UI != UE; ++UI) {
        // Only examine CopyToReg uses.
        if (UI->getOpcode() != ISD::CopyToReg)
            return false;
        // Only examine CopyToReg uses that copy to EFLAGS.
        if (cast<RegisterSDNode>(UI->getOperand(1))->getReg() !=
                Cse523::EFLAGS)
            return false;
        // Examine each user of the CopyToReg use.
        for (SDNode::use_iterator FlagUI = UI->use_begin(),
                FlagUE = UI->use_end(); FlagUI != FlagUE; ++FlagUI) {
            // Only examine the Flag result.
            if (FlagUI.getUse().getResNo() != 1) continue;
            // Anything unusual: assume conservatively.
            if (!FlagUI->isMachineOpcode()) return false;
            // Examine the opcode of the user.

            switch (FlagUI->getMachineOpcode()) {
                //TODO:
                // These comparisons don't treat the most significant bit specially.
                //case Cse523::SETAr: case Cse523::SETAEr: case Cse523::SETBr: case Cse523::SETBEr:
                //case Cse523::SETEr: case Cse523::SETNEr: case Cse523::SETPr: case Cse523::SETNPr:
                //case Cse523::SETAm: case Cse523::SETAEm: case Cse523::SETBm: case Cse523::SETBEm:
                //case Cse523::SETEm: case Cse523::SETNEm: case Cse523::SETPm: case Cse523::SETNPm:
                //case Cse523::CMOVA16rr: case Cse523::CMOVA16rm:
                //case Cse523::CMOVA32rr: case Cse523::CMOVA32rm:
                //case Cse523::CMOVA64rr: case Cse523::CMOVA64rm:
                //case Cse523::CMOVAE16rr: case Cse523::CMOVAE16rm:
                //case Cse523::CMOVAE32rr: case Cse523::CMOVAE32rm:
                //case Cse523::CMOVAE64rr: case Cse523::CMOVAE64rm:
                //case Cse523::CMOVB16rr: case Cse523::CMOVB16rm:
                //case Cse523::CMOVB32rr: case Cse523::CMOVB32rm:
                //case Cse523::CMOVB64rr: case Cse523::CMOVB64rm:
                //case Cse523::CMOVBE16rr: case Cse523::CMOVBE16rm:
                //case Cse523::CMOVBE32rr: case Cse523::CMOVBE32rm:
                //case Cse523::CMOVBE64rr: case Cse523::CMOVBE64rm:
                //case Cse523::CMOVE16rr: case Cse523::CMOVE16rm:
                //case Cse523::CMOVE32rr: case Cse523::CMOVE32rm:
                //case Cse523::CMOVE64rr: case Cse523::CMOVE64rm:
                //case Cse523::CMOVNE16rr: case Cse523::CMOVNE16rm:
                //case Cse523::CMOVNE32rr: case Cse523::CMOVNE32rm:
                //case Cse523::CMOVNE64rr: case Cse523::CMOVNE64rm:
                //case Cse523::CMOVNP16rr: case Cse523::CMOVNP16rm:
                //case Cse523::CMOVNP32rr: case Cse523::CMOVNP32rm:
                //case Cse523::CMOVNP64rr: case Cse523::CMOVNP64rm:
                //case Cse523::CMOVP16rr: case Cse523::CMOVP16rm:
                //case Cse523::CMOVP32rr: case Cse523::CMOVP32rm:
                //case Cse523::CMOVP64rr: case Cse523::CMOVP64rm:
                case Cse523::JA_4: case Cse523::JAE_4: case Cse523::JB_4: case Cse523::JBE_4:
                case Cse523::JE_4: case Cse523::JNE_4: case Cse523::JP_4: case Cse523::JNP_4:
                    continue;
                    // Anything else: assume conservatively.
                default: return false;
            }
        }
    }
    return true;
}

/// isLoadIncOrDecStore - Check whether or not the chain ending in StoreNode
/// is suitable for doing the {load; increment or decrement; store} to modify
/// transformation.
static bool isLoadIncOrDecStore(StoreSDNode *StoreNode, unsigned Opc,
        SDValue StoredVal, SelectionDAG *CurDAG,
        LoadSDNode* &LoadNode, SDValue &InputChain) {

    // is the value stored the result of a DEC or INC?
    if (!(Opc == Cse523ISD::DEC || Opc == Cse523ISD::INC)) return false;

    // is the stored value result 0 of the load?
    if (StoredVal.getResNo() != 0) return false;

    // are there other uses of the loaded value than the inc or dec?
    if (!StoredVal.getNode()->hasNUsesOfValue(1, 0)) return false;

    // is the store non-extending and non-indexed?
    if (!ISD::isNormalStore(StoreNode) || StoreNode->isNonTemporal())
        return false;

    SDValue Load = StoredVal->getOperand(0);
    // Is the stored value a non-extending and non-indexed load?
    if (!ISD::isNormalLoad(Load.getNode())) return false;

    // Return LoadNode by reference.
    LoadNode = cast<LoadSDNode>(Load);
    // is the size of the value one that we can handle? (i.e. 64, 32, 16, or 8)
    EVT LdVT = LoadNode->getMemoryVT();
    if (LdVT != MVT::i64 && LdVT != MVT::i32 && LdVT != MVT::i16 &&
            LdVT != MVT::i8)
        return false;

    // Is store the only read of the loaded value?
    if (!Load.hasOneUse())
        return false;

    // Is the address of the store the same as the load?
    if (LoadNode->getBasePtr() != StoreNode->getBasePtr() ||
            LoadNode->getOffset() != StoreNode->getOffset())
        return false;

    // Check if the chain is produced by the load or is a TokenFactor with
    // the load output chain as an operand. Return InputChain by reference.
    SDValue Chain = StoreNode->getChain();

    bool ChainCheck = false;
    if (Chain == Load.getValue(1)) {
        ChainCheck = true;
        InputChain = LoadNode->getChain();
    } else if (Chain.getOpcode() == ISD::TokenFactor) {
        SmallVector<SDValue, 4> ChainOps;
        for (unsigned i = 0, e = Chain.getNumOperands(); i != e; ++i) {
            SDValue Op = Chain.getOperand(i);
            if (Op == Load.getValue(1)) {
                ChainCheck = true;
                continue;
            }

            // Make sure using Op as part of the chain would not cause a cycle here.
            // In theory, we could check whether the chain node is a predecessor of
            // the load. But that can be very expensive. Instead visit the uses and
            // make sure they all have smaller node id than the load.
            int LoadId = LoadNode->getNodeId();
            for (SDNode::use_iterator UI = Op.getNode()->use_begin(),
                    UE = UI->use_end(); UI != UE; ++UI) {
                if (UI.getUse().getResNo() != 0)
                    continue;
                if (UI->getNodeId() > LoadId)
                    return false;
            }

            ChainOps.push_back(Op);
        }

        if (ChainCheck)
            // Make a new TokenFactor with all the other input chains except
            // for the load.
            InputChain = CurDAG->getNode(ISD::TokenFactor, SDLoc(Chain),
                    MVT::Other, &ChainOps[0], ChainOps.size());
    }
    if (!ChainCheck)
        return false;

    return true;
}

/// getFusedLdStOpcode - Get the appropriate Cse523 opcode for an in memory
/// increment or decrement. Opc should be Cse523ISD::DEC or Cse523ISD::INC.
static unsigned getFusedLdStOpcode(EVT &LdVT, unsigned Opc) {
    if (Opc == Cse523ISD::DEC) {
        if (LdVT == MVT::i64) return Cse523::DEC64m;
    } else {
        assert(Opc == Cse523ISD::INC && "unrecognized opcode");
        if (LdVT == MVT::i64) return Cse523::INC64m;
    }
    llvm_unreachable("unrecognized size for LdVT");
}

/// SelectGather - Customized ISel for GATHER operations.
///
SDNode *Cse523DAGToDAGISel::SelectGather(SDNode *Node, unsigned Opc) {
    // Operands of Gather: VSrc, Base, VIdx, VMask, Scale
    SDValue Chain = Node->getOperand(0);
    SDValue VSrc = Node->getOperand(2);
    SDValue Base = Node->getOperand(3);
    SDValue VIdx = Node->getOperand(4);
    SDValue VMask = Node->getOperand(5);
    ConstantSDNode *Scale = dyn_cast<ConstantSDNode>(Node->getOperand(6));
    if (!Scale)
        return 0;

    SDVTList VTs = CurDAG->getVTList(VSrc.getValueType(), VSrc.getValueType(),
            MVT::Other);

    // Memory Operands: Base, Scale, Index, Disp, Segment
    SDValue Disp = CurDAG->getTargetConstant(0, MVT::i32);
    SDValue Segment = CurDAG->getRegister(0, MVT::i32);
    const SDValue Ops[] = { VSrc, Base, getI8Imm(Scale->getSExtValue()), VIdx,
        Disp, Segment, VMask, Chain};
    SDNode *ResNode = CurDAG->getMachineNode(Opc, SDLoc(Node), VTs, Ops);
    // Node has 2 outputs: VDst and MVT::Other.
    // ResNode has 3 outputs: VDst, VMask_wb, and MVT::Other.
    // We replace VDst of Node with VDst of ResNode, and Other of Node with Other
    // of ResNode.
    ReplaceUses(SDValue(Node, 0), SDValue(ResNode, 0));
    ReplaceUses(SDValue(Node, 1), SDValue(ResNode, 2));
    return ResNode;
}

SDNode *Cse523DAGToDAGISel::Select(SDNode *Node) {
    MVT NVT = Node->getSimpleValueType(0);
    unsigned Opc, MOpc;
    unsigned Opcode = Node->getOpcode();
    SDLoc dl(Node);

    DEBUG(dbgs() << "Selecting:Opcode[" << Opcode << "] "; Node->dump(CurDAG); dbgs() << '\n');

    if (Node->isMachineOpcode()) {
        DEBUG(dbgs() << "== ";  Node->dump(CurDAG); dbgs() << '\n');
        Node->setNodeId(-1);
        return NULL;   // Already selected.
    }

    switch (Opcode) {
        default: break;
        case ISD::INTRINSIC_W_CHAIN:
                assert(0);
                break;
        case Cse523ISD::GlobalBaseReg:
                return getGlobalBaseReg();


        case Cse523ISD::ATOMOR64_DAG:
        case Cse523ISD::ATOMXOR64_DAG:
        case Cse523ISD::ATOMADD64_DAG:
        case Cse523ISD::ATOMSUB64_DAG:
        case Cse523ISD::ATOMNAND64_DAG:
        case Cse523ISD::ATOMAND64_DAG:
        case Cse523ISD::ATOMMAX64_DAG:
        case Cse523ISD::ATOMMIN64_DAG:
        case Cse523ISD::ATOMUMAX64_DAG:
        case Cse523ISD::ATOMUMIN64_DAG:
        case Cse523ISD::ATOMSWAP64_DAG: {
                                            unsigned Opc;
                                            switch (Opcode) {
                                                default: llvm_unreachable("Impossible opcode");
                                                //case Cse523ISD::ATOMOR64_DAG:   Opc = Cse523::ATOMOR6432;   break;
                                                //case Cse523ISD::ATOMXOR64_DAG:  Opc = Cse523::ATOMXOR6432;  break;
                                                //case Cse523ISD::ATOMADD64_DAG:  Opc = Cse523::ATOMADD6432;  break;
                                                //case Cse523ISD::ATOMSUB64_DAG:  Opc = Cse523::ATOMSUB6432;  break;
                                                //case Cse523ISD::ATOMNAND64_DAG: Opc = Cse523::ATOMNAND6432; break;
                                                //case Cse523ISD::ATOMAND64_DAG:  Opc = Cse523::ATOMAND6432;  break;
                                                //case Cse523ISD::ATOMMAX64_DAG:  Opc = Cse523::ATOMMAX6432;  break;
                                                //case Cse523ISD::ATOMMIN64_DAG:  Opc = Cse523::ATOMMIN6432;  break;
                                                //case Cse523ISD::ATOMUMAX64_DAG: Opc = Cse523::ATOMUMAX6432; break;
                                                //case Cse523ISD::ATOMUMIN64_DAG: Opc = Cse523::ATOMUMIN6432; break;
                                                //case Cse523ISD::ATOMSWAP64_DAG: Opc = Cse523::ATOMSWAP6432; break;
                                            }
                                            SDNode *RetVal = SelectAtomic64(Node, Opc);
                                            if (RetVal)
                                                return RetVal;
                                            break;
                                        }

        case ISD::ATOMIC_LOAD_XOR:
        case ISD::ATOMIC_LOAD_AND:
        case ISD::ATOMIC_LOAD_OR:
        case ISD::ATOMIC_LOAD_ADD: {
                                       SDNode *RetVal = SelectAtomicLoadArith(Node, NVT);
                                       if (RetVal)
                                           return RetVal;
                                       break;
                                   }
        case ISD::AND:
        case ISD::OR:
        case ISD::XOR: {
                           // For operations of the form (x << C1) op C2, check if we can use a smaller
                           // encoding for C2 by transforming it into (x op (C2>>C1)) << C1.
                           SDValue N0 = Node->getOperand(0);
                           SDValue N1 = Node->getOperand(1);

                           if (N0->getOpcode() != ISD::SHL || !N0->hasOneUse())
                               break;

                           // i8 is unshrinkable, i16 should be promoted to i32.
                           if (NVT != MVT::i32 && NVT != MVT::i64)
                               break;

                           ConstantSDNode *Cst = dyn_cast<ConstantSDNode>(N1);
                           ConstantSDNode *ShlCst = dyn_cast<ConstantSDNode>(N0->getOperand(1));
                           if (!Cst || !ShlCst)
                               break;

                           int64_t Val = Cst->getSExtValue();
                           uint64_t ShlVal = ShlCst->getZExtValue();

                           // Make sure that we don't change the operation by removing bits.
                           // This only matters for OR and XOR, AND is unaffected.
                           uint64_t RemovedBitsMask = (1ULL << ShlVal) - 1;
                           if (Opcode != ISD::AND && (Val & RemovedBitsMask) != 0)
                               break;

                           unsigned ShlOp, Op;
                           MVT CstVT = NVT;

                           // Check the minimum bitwidth for the new constant.
                           // TODO: AND32ri is the same as AND64ri32 with zext imm.
                           // TODO: MOV32ri+OR64r is cheaper than MOV64ri64+OR64rr
                           // TODO: Using 16 and 8 bit operations is also possible for or32 & xor32.
                           if (!isInt<8>(Val) && isInt<8>(Val >> ShlVal))
                               CstVT = MVT::i8;
                           else if (!isInt<32>(Val) && isInt<32>(Val >> ShlVal))
                               CstVT = MVT::i32;

                           // Bail if there is no smaller encoding.
                           if (NVT == CstVT)
                               break;

                           switch (NVT.SimpleTy) {
                               default: llvm_unreachable("Unsupported VT!");
                               case MVT::i32:
                                        assert(CstVT == MVT::i8);
                                        //ShlOp = Cse523::SHL32ri;

                                        switch (Opcode) {
                                            default: llvm_unreachable("Impossible opcode");
                                            //case ISD::AND: Op = Cse523::AND32ri8; break;
                                            //case ISD::OR:  Op =  Cse523::OR32ri8; break;
                                            //case ISD::XOR: Op = Cse523::XOR32ri8; break;
                                        }
                                        break;
                               case MVT::i64:
                                        assert(CstVT == MVT::i8 || CstVT == MVT::i32);
                                        ShlOp = Cse523::SHL64ri;

                                        switch (Opcode) {
                                            default: llvm_unreachable("Impossible opcode");
                                            case ISD::AND: Op = Cse523::AND64ri32; break;
                                            case ISD::OR:  Op = Cse523::OR64ri32; break;
                                            case ISD::XOR: Op = Cse523::XOR64ri32; break;
                                        }
                                        break;
                           }

                           // Emit the smaller op and the shift.
                           SDValue NewCst = CurDAG->getTargetConstant(Val >> ShlVal, CstVT);
                           SDNode *New = CurDAG->getMachineNode(Op, dl, NVT, N0->getOperand(0),NewCst);
                           return CurDAG->SelectNodeTo(Node, ShlOp, NVT, SDValue(New, 0),
                                   getI8Imm(ShlVal));
                       }
                       break;
        case Cse523ISD::UMUL: {
                                  SDValue N0 = Node->getOperand(0);
                                  SDValue N1 = Node->getOperand(1);

                                  unsigned LoReg;
                                  switch (NVT.SimpleTy) {
                                      case MVT::i8:
                                      case MVT::i16:
                                      case MVT::i32:
                                      default: llvm_unreachable("Unsupported VT!");

                                      case MVT::i64: LoReg = Cse523::RAX; Opc = Cse523::MUL64r; break;
                                  }

                                  SDValue InFlag = CurDAG->getCopyToReg(CurDAG->getEntryNode(), dl, LoReg,
                                          N0, SDValue()).getValue(1);

                                  SDVTList VTs = CurDAG->getVTList(NVT, NVT, MVT::i32);
                                  SDValue Ops[] = {N1, InFlag};
                                  SDNode *CNode = CurDAG->getMachineNode(Opc, dl, VTs, Ops);

                                  ReplaceUses(SDValue(Node, 0), SDValue(CNode, 0));
                                  ReplaceUses(SDValue(Node, 1), SDValue(CNode, 1));
                                  ReplaceUses(SDValue(Node, 2), SDValue(CNode, 2));
                                  return NULL;
                             }
                             break;

        case ISD::SMUL_LOHI:
        case ISD::UMUL_LOHI: {
                                 SDValue N0 = Node->getOperand(0);
                                 SDValue N1 = Node->getOperand(1);

                                 bool isSigned = Opcode == ISD::SMUL_LOHI;
                                 bool hasBMI2 = Subtarget->hasBMI2();
                                 if (!isSigned) {
                                     switch (NVT.SimpleTy) {
                                         default: llvm_unreachable("Unsupported VT!");
                                         case MVT::i64: Opc = hasBMI2 ? Cse523::MULX64rr : Cse523::MUL64r;
                                                        MOpc = hasBMI2 ? Cse523::MULX64rm : Cse523::MUL64m; break;
                                     }
                                 } else {
                                     switch (NVT.SimpleTy) {
                                         default: llvm_unreachable("Unsupported VT!");
                                         case MVT::i64: Opc = Cse523::IMUL64r; MOpc = Cse523::IMUL64m; break;
                                     }
                                 }

                                 unsigned SrcReg, LoReg, HiReg;
                                 switch (Opc) {
                                     default: llvm_unreachable("Unknown MUL opcode!");

                                     case Cse523::IMUL64r:
                                     case Cse523::MUL64r:
                                              SrcReg = LoReg = Cse523::RAX; HiReg = Cse523::RDX;
                                              break;
                                     case Cse523::MULX64rr:
                                              SrcReg = Cse523::RDX; LoReg = HiReg = 0;
                                              break;
                                 }

                                 SDValue Tmp0, Tmp1, Tmp2, Tmp3, Tmp4;
                                 bool foldedLoad = TryFoldLoad(Node, N1, Tmp0, Tmp1, Tmp2, Tmp3, Tmp4);
                                 // Multiply is commmutative.
                                 if (!foldedLoad) {
                                     foldedLoad = TryFoldLoad(Node, N0, Tmp0, Tmp1, Tmp2, Tmp3, Tmp4);
                                     if (foldedLoad)
                                         std::swap(N0, N1);
                                 }

                                 SDValue InFlag = CurDAG->getCopyToReg(CurDAG->getEntryNode(), dl, SrcReg,
                                         N0, SDValue()).getValue(1);
                                 SDValue ResHi, ResLo;

                                 if (foldedLoad) {
                                     SDValue Chain;
                                     SDValue Ops[] = { Tmp0, Tmp1, Tmp2, Tmp3, Tmp4, N1.getOperand(0),
                                         InFlag };
                                     if (MOpc == Cse523::MULX64rm) {
                                         SDVTList VTs = CurDAG->getVTList(NVT, NVT, MVT::Other, MVT::Glue);
                                         SDNode *CNode = CurDAG->getMachineNode(MOpc, dl, VTs, Ops);
                                         ResHi = SDValue(CNode, 0);
                                         ResLo = SDValue(CNode, 1);
                                         Chain = SDValue(CNode, 2);
                                         InFlag = SDValue(CNode, 3);
                                     } else {
                                         SDVTList VTs = CurDAG->getVTList(MVT::Other, MVT::Glue);
                                         SDNode *CNode = CurDAG->getMachineNode(MOpc, dl, VTs, Ops);
                                         Chain = SDValue(CNode, 0);
                                         InFlag = SDValue(CNode, 1);
                                     }

                                     // Update the chain.
                                     ReplaceUses(N1.getValue(1), Chain);
                                 } else {
                                     SDValue Ops[] = { N1, InFlag };
                                     if (Opc == Cse523::MULX64rr) {
                                         SDVTList VTs = CurDAG->getVTList(NVT, NVT, MVT::Glue);
                                         SDNode *CNode = CurDAG->getMachineNode(Opc, dl, VTs, Ops);
                                         ResHi = SDValue(CNode, 0);
                                         ResLo = SDValue(CNode, 1);
                                         InFlag = SDValue(CNode, 2);
                                     } else {
                                         SDVTList VTs = CurDAG->getVTList(MVT::Glue);
                                         SDNode *CNode = CurDAG->getMachineNode(Opc, dl, VTs, Ops);
                                         InFlag = SDValue(CNode, 0);
                                     }
                                 }

                                 // Copy the low half of the result, if it is needed.
                                 if (!SDValue(Node, 0).use_empty()) {
                                     if (ResLo.getNode() == 0) {
                                         assert(LoReg && "Register for low half is not defined!");
                                         ResLo = CurDAG->getCopyFromReg(CurDAG->getEntryNode(), dl, LoReg, NVT,
                                                 InFlag);
                                         InFlag = ResLo.getValue(2);
                                     }
                                     ReplaceUses(SDValue(Node, 0), ResLo);
                                     DEBUG(dbgs() << "=> "; ResLo.getNode()->dump(CurDAG); dbgs() << '\n');
                                 }
                                 // Copy the high half of the result, if it is needed.
                                 if (!SDValue(Node, 1).use_empty()) {
                                     if (ResHi.getNode() == 0) {
                                         assert(HiReg && "Register for high half is not defined!");
                                         ResHi = CurDAG->getCopyFromReg(CurDAG->getEntryNode(), dl, HiReg, NVT,
                                                 InFlag);
                                         InFlag = ResHi.getValue(2);
                                     }
                                     ReplaceUses(SDValue(Node, 1), ResHi);
                                     DEBUG(dbgs() << "=> "; ResHi.getNode()->dump(CurDAG); dbgs() << '\n');
                                 }

                                 return NULL;
                             }

        case ISD::SDIVREM:
        case ISD::UDIVREM: {
                               SDValue N0 = Node->getOperand(0);
                               SDValue N1 = Node->getOperand(1);

                               bool isSigned = Opcode == ISD::SDIVREM;
                               if (!isSigned) {
                                   switch (NVT.SimpleTy) {
                                       default: llvm_unreachable("Unsupported VT!");
                                       case MVT::i64: Opc = Cse523::DIV64r; MOpc = Cse523::DIV64m; break;
                                   }
                               } else {
                                   switch (NVT.SimpleTy) {
                                       default: llvm_unreachable("Unsupported VT!");
                                       case MVT::i64: Opc = Cse523::IDIV64r; MOpc = Cse523::IDIV64m; break;
                                   }
                               }

                               unsigned LoReg, HiReg, ClrReg;
                               unsigned SExtOpcode;
                               switch (NVT.SimpleTy) {
                                   default: llvm_unreachable("Unsupported VT!");
                                   case MVT::i64:
                                            LoReg = Cse523::RAX; ClrReg = HiReg = Cse523::RDX;
                                            SExtOpcode = Cse523::CQO;
                                            break;
                               }

                               SDValue Tmp0, Tmp1, Tmp2, Tmp3, Tmp4;
                               bool foldedLoad = TryFoldLoad(Node, N1, Tmp0, Tmp1, Tmp2, Tmp3, Tmp4);
                               bool signBitIsZero = CurDAG->SignBitIsZero(N0);

                               SDValue InFlag;
                               InFlag =
                                   CurDAG->getCopyToReg(CurDAG->getEntryNode(), dl,
                                           LoReg, N0, SDValue()).getValue(1);
                               if (isSigned && !signBitIsZero) {
                                   // Sign extend the low part into the high part.
                                   InFlag = SDValue(CurDAG->getMachineNode(SExtOpcode, dl, MVT::Glue, InFlag),0);
                               } else {
                                   // Zero out the high part, effectively zero extending the input.
                                   SDValue ClrNode = SDValue(CurDAG->getMachineNode(Cse523::MOV64r0, dl, NVT), 0);
                                   switch (NVT.SimpleTy) {
                                       case MVT::i64:
                                            // TODO: Needed?
                                            //ClrNode =
                                            //    SDValue(CurDAG->getMachineNode(
                                            //                TargetOpcode::SUBREG_TO_REG, dl, MVT::i64,
                                            //                CurDAG->getTargetConstant(0, MVT::i64), ClrNode,
                                            //                CurDAG->getTargetConstant(Cse523::sub_32bit, MVT::i32)),
                                            //            0);
                                           break;

                                       case MVT::i32:
                                       default:
                                           llvm_unreachable("Unexpected division source");
                                   }

                                   InFlag = CurDAG->getCopyToReg(CurDAG->getEntryNode(), dl, ClrReg,
                                           ClrNode, InFlag).getValue(1);
                               }

                               if (foldedLoad) {
                                   SDValue Ops[] = { Tmp0, Tmp1, Tmp2, Tmp3, Tmp4, N1.getOperand(0),
                                       InFlag };
                                   SDNode *CNode =
                                       CurDAG->getMachineNode(MOpc, dl, MVT::Other, MVT::Glue, Ops);
                                   InFlag = SDValue(CNode, 1);
                                   // Update the chain.
                                   ReplaceUses(N1.getValue(1), SDValue(CNode, 0));
                               } else {
                                   InFlag =
                                       SDValue(CurDAG->getMachineNode(Opc, dl, MVT::Glue, N1, InFlag), 0);
                               }

                               // Copy the division (low) result, if it is needed.
                               if (!SDValue(Node, 0).use_empty()) {
                                   SDValue Result = CurDAG->getCopyFromReg(CurDAG->getEntryNode(), dl,
                                           LoReg, NVT, InFlag);
                                   InFlag = Result.getValue(2);
                                   ReplaceUses(SDValue(Node, 0), Result);
                                   DEBUG(dbgs() << "=> "; Result.getNode()->dump(CurDAG); dbgs() << '\n');
                               }
                               // Copy the remainder (high) result, if it is needed.
                               if (!SDValue(Node, 1).use_empty()) {
                                   SDValue Result = CurDAG->getCopyFromReg(CurDAG->getEntryNode(), dl,
                                           HiReg, NVT, InFlag);
                                   InFlag = Result.getValue(2);
                                   ReplaceUses(SDValue(Node, 1), Result);
                                   DEBUG(dbgs() << "=> "; Result.getNode()->dump(CurDAG); dbgs() << '\n');
                               }
                               return NULL;
                           }

        case Cse523ISD::CMP:
        case Cse523ISD::SUB: {
                                 // Sometimes a SUB is used to perform comparison.
                                 if (Opcode == Cse523ISD::SUB && Node->hasAnyUseOfValue(0))
                                     // This node is not a CMP.
                                     break;
                                 SDValue N0 = Node->getOperand(0);
                                 SDValue N1 = Node->getOperand(1);

                                 // Look for (Cse523cmp (and $op, $imm), 0) and see if we can convert it to
                                 // use a smaller encoding.
                                 if (N0.getOpcode() == ISD::TRUNCATE && N0.hasOneUse() &&
                                         HasNoSignedComparisonUses(Node))
                                     // Look past the truncate if CMP is the only use of it.
                                     N0 = N0.getOperand(0);
                                 if ((N0.getNode()->getOpcode() == ISD::AND ||
                                             (N0.getResNo() == 0 && N0.getNode()->getOpcode() == Cse523ISD::AND)) &&
                                         N0.getNode()->hasOneUse() &&
                                         N0.getValueType() != MVT::i8 &&
                                         Cse523::isZeroNode(N1)) {
                                     ConstantSDNode *C = dyn_cast<ConstantSDNode>(N0.getNode()->getOperand(1));
                                     if (!C) break;

                                     // For example, convert "testl %eax, $8" to "testb %al, $8"
                                     if ((C->getZExtValue() & ~UINT64_C(0xff)) == 0 &&
                                             (!(C->getZExtValue() & 0x80) ||
                                              HasNoSignedComparisonUses(Node))) {
                                         SDValue Imm = CurDAG->getTargetConstant(C->getZExtValue(), MVT::i64);
                                         SDValue Reg = N0.getNode()->getOperand(0);

                                         // Emit a testb.
                                         SDNode *NewNode = CurDAG->getMachineNode(Cse523::TEST64ri32, dl, MVT::i64,
                                                 Reg, Imm);
                                         // Replace SUB|CMP with TEST, since SUB has two outputs while TEST has
                                         // one, do not call ReplaceAllUsesWith.
                                         ReplaceUses(SDValue(Node, (Opcode == Cse523ISD::SUB ? 1 : 0)),
                                                 SDValue(NewNode, 0));
                                         return NULL;
                                     }
                                     
                                     //TODO: Need to enable below code to convert into TEST instructions
                                     break;

                                     // For example, "testl %eax, $2048" to "testb %ah, $8".
                                     if ((C->getZExtValue() & ~UINT64_C(0xff00)) == 0 &&
                                             (!(C->getZExtValue() & 0x8000) ||
                                              HasNoSignedComparisonUses(Node))) {
                                         assert(0);
                                         // Shift the immediate right by 8 bits.
                                         //SDValue ShiftedImm = CurDAG->getTargetConstant(C->getZExtValue() >> 8,
                                         //        MVT::i8);
                                         //SDValue Reg = N0.getNode()->getOperand(0);

                                         //// Put the value in an ABCD register.
                                         //const TargetRegisterClass *TRC;
                                         //switch (N0.getSimpleValueType().SimpleTy) {
                                         //    case MVT::i64: TRC = &Cse523::GR64_ABCDRegClass; break;
                                         //    default: llvm_unreachable("Unsupported TEST operand type!");
                                         //}
                                         //SDValue RC = CurDAG->getTargetConstant(TRC->getID(), MVT::i32);
                                         //Reg = SDValue(CurDAG->getMachineNode(Cse523::COPY_TO_REGCLASS, dl,
                                         //            Reg.getValueType(), Reg, RC), 0);

                                         //// Extract the h-register.
                                         //SDValue Subreg = CurDAG->getTargetExtractSubreg(Cse523::sub_8bit_hi, dl,
                                         //        MVT::i8, Reg);

                                         //// Emit a testb.  The EXTRACT_SUBREG becomes a COPY that can only
                                         //// target GR8_NOREX registers, so make sure the register class is
                                         //// forced.
                                         //SDNode *NewNode = CurDAG->getMachineNode(Cse523::TEST8ri_NOREX, dl,
                                         //        MVT::i32, Subreg, ShiftedImm);
                                         //// Replace SUB|CMP with TEST, since SUB has two outputs while TEST has
                                         //// one, do not call ReplaceAllUsesWith.
                                         //ReplaceUses(SDValue(Node, (Opcode == Cse523ISD::SUB ? 1 : 0)),
                                         //        SDValue(NewNode, 0));
                                         //return NULL;
                                     }

                                     // For example, "testl %eax, $32776" to "testw %ax, $32776".
                                     if ((C->getZExtValue() & ~UINT64_C(0xffff)) == 0 &&
                                             N0.getValueType() != MVT::i16 &&
                                             (!(C->getZExtValue() & 0x8000) ||
                                              HasNoSignedComparisonUses(Node))) {
                                         assert(0);
                                         //SDValue Imm = CurDAG->getTargetConstant(C->getZExtValue(), MVT::i16);
                                         //SDValue Reg = N0.getNode()->getOperand(0);

                                         //// Extract the 16-bit subregister.
                                         //SDValue Subreg = CurDAG->getTargetExtractSubreg(Cse523::sub_16bit, dl,
                                         //        MVT::i16, Reg);

                                         //// Emit a testw.
                                         //SDNode *NewNode = CurDAG->getMachineNode(Cse523::TEST16ri, dl, MVT::i32,
                                         //        Subreg, Imm);
                                         //// Replace SUB|CMP with TEST, since SUB has two outputs while TEST has
                                         //// one, do not call ReplaceAllUsesWith.
                                         //ReplaceUses(SDValue(Node, (Opcode == Cse523ISD::SUB ? 1 : 0)),
                                         //        SDValue(NewNode, 0));
                                         //return NULL;
                                     }

                                     // For example, "testq %rax, $268468232" to "testl %eax, $268468232".
                                     if ((C->getZExtValue() & ~UINT64_C(0xffffffff)) == 0 &&
                                             N0.getValueType() == MVT::i64 &&
                                             (!(C->getZExtValue() & 0x80000000) ||
                                              HasNoSignedComparisonUses(Node))) {
                                         assert(0);
                                         //SDValue Imm = CurDAG->getTargetConstant(C->getZExtValue(), MVT::i32);
                                         //SDValue Reg = N0.getNode()->getOperand(0);

                                         //// Extract the 32-bit subregister.
                                         //SDValue Subreg = CurDAG->getTargetExtractSubreg(Cse523::sub_32bit, dl,
                                         //        MVT::i32, Reg);

                                         //// Emit a testl.
                                         //SDNode *NewNode = CurDAG->getMachineNode(Cse523::TEST32ri, dl, MVT::i32,
                                         //        Subreg, Imm);
                                         //// Replace SUB|CMP with TEST, since SUB has two outputs while TEST has
                                         //// one, do not call ReplaceAllUsesWith.
                                         //ReplaceUses(SDValue(Node, (Opcode == Cse523ISD::SUB ? 1 : 0)),
                                         //        SDValue(NewNode, 0));
                                         //return NULL;
                                     }
                                 }
                                 break;
                             }
        case ISD::STORE: {
                             // Change a chain of {load; incr or dec; store} of the same value into
                             // a simple increment or decrement through memory of that value, if the
                             // uses of the modified value and its address are suitable.
                             // The DEC64m tablegen pattern is currently not able to match the case where
                             // the EFLAGS on the original DEC are used. (This also applies to
                             // {INC,DEC}X{64,32,16,8}.)
                             // We'll need to improve tablegen to allow flags to be transferred from a
                             // node in the pattern to the result node.  probably with a new keyword
                             // for example, we have this
                             // def DEC64m : RI<0xFF, MRM1m, (outs), (ins i64mem:$dst), "dec{q}\t$dst",
                             //  [(store (add (loadi64 addr:$dst), -1), addr:$dst),
                             //   (implicit EFLAGS)]>;
                             // but maybe need something like this
                             // def DEC64m : RI<0xFF, MRM1m, (outs), (ins i64mem:$dst), "dec{q}\t$dst",
                             //  [(store (add (loadi64 addr:$dst), -1), addr:$dst),
                             //   (transferrable EFLAGS)]>;

                             StoreSDNode *StoreNode = cast<StoreSDNode>(Node);
                             SDValue StoredVal = StoreNode->getOperand(1);
                             unsigned Opc = StoredVal->getOpcode();

                             LoadSDNode *LoadNode = 0;
                             SDValue InputChain;
                             if (!isLoadIncOrDecStore(StoreNode, Opc, StoredVal, CurDAG,
                                         LoadNode, InputChain))
                                 break;

                             SDValue Base, Scale, Index, Disp, Segment;
                             if (!SelectAddr(LoadNode, LoadNode->getBasePtr(),
                                         Base, Scale, Index, Disp, Segment))
                                 break;

                             MachineSDNode::mmo_iterator MemOp = MF->allocateMemRefsArray(2);
                             MemOp[0] = StoreNode->getMemOperand();
                             MemOp[1] = LoadNode->getMemOperand();
                             const SDValue Ops[] = { Base, Scale, Index, Disp, Segment, InputChain };
                             EVT LdVT = LoadNode->getMemoryVT();
                             unsigned newOpc = getFusedLdStOpcode(LdVT, Opc);
                             MachineSDNode *Result = CurDAG->getMachineNode(newOpc,
                                     SDLoc(Node),
                                     MVT::i32, MVT::Other, Ops);
                             Result->setMemRefs(MemOp, MemOp + 2);

                             ReplaceUses(SDValue(StoreNode, 0), SDValue(Result, 1));
                             ReplaceUses(SDValue(StoredVal.getNode(), 1), SDValue(Result, 0));

                             return Result;
                         }
                         break;
    }

    SDNode *ResNode = SelectCode(Node);

    DEBUG(dbgs() << "=> ";
            if (ResNode == NULL || ResNode == Node)
            Node->dump(CurDAG);
            else
            ResNode->dump(CurDAG);
            dbgs() << '\n');

    return ResNode;
}

bool Cse523DAGToDAGISel::
SelectInlineAsmMemoryOperand(const SDValue &Op, char ConstraintCode,
        std::vector<SDValue> &OutOps) {
    SDValue Op0, Op1, Op2, Op3, Op4;
    switch (ConstraintCode) {
        case 'o':   // offsetable        ??
        case 'v':   // not offsetable    ??
        default: return true;
        case 'm':   // memory
                 if (!SelectAddr(0, Op, Op0, Op1, Op2, Op3, Op4))
                     return true;
                 break;
    }

    OutOps.push_back(Op0);
    OutOps.push_back(Op1);
    OutOps.push_back(Op2);
    OutOps.push_back(Op3);
    OutOps.push_back(Op4);
    return false;
}

/// createCse523ISelDag - This pass converts a legalized DAG into a
/// Cse523-specific DAG, ready for instruction scheduling.
///
FunctionPass *llvm::createCse523ISelDag(Cse523TargetMachine &TM,
        CodeGenOpt::Level OptLevel) {
    return new Cse523DAGToDAGISel(TM, OptLevel);
}
