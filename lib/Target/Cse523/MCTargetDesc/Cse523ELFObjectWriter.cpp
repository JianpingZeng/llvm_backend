//===-- Cse523ELFObjectWriter.cpp - Cse523 ELF Writer -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Cse523FixupKinds.h"
#include "MCTargetDesc/Cse523MCExpr.h"
#include "MCTargetDesc/Cse523MCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
    class Cse523ELFObjectWriter : public MCELFObjectTargetWriter {
        public:
            Cse523ELFObjectWriter(bool Is64Bit, uint8_t OSABI)
                : MCELFObjectTargetWriter(Is64Bit, OSABI,
                        ELF::EM_CSE523,
                        /*HasRelocationAddend*/ true) {}

            virtual ~Cse523ELFObjectWriter() {}
        protected:
            virtual unsigned GetRelocType(const MCValue &Target, const MCFixup &Fixup,
                    bool IsPCRel, bool IsRelocWithSymbol,
                    int64_t Addend) const;

            virtual const MCSymbol *ExplicitRelSym(const MCAssembler &Asm,
                    const MCValue &Target,
                    const MCFragment &F,
                    const MCFixup &Fixup,
                    bool IsPCRel) const;
    };
}


unsigned Cse523ELFObjectWriter::GetRelocType(const MCValue &Target,
        const MCFixup &Fixup,
        bool IsPCRel,
        bool IsRelocWithSymbol,
        int64_t Addend) const {

    if (const Cse523MCExpr *SExpr = dyn_cast<Cse523MCExpr>(Fixup.getValue())) {
        if (SExpr->getKind() == Cse523MCExpr::VK_Cse523_R_DISP32)
            return ELF::R_CSE523_DISP32;
    }

    if (IsPCRel) {
        switch((unsigned)Fixup.getKind()) {
            default:
                llvm_unreachable("Unimplemented fixup -> relocation");
            case FK_Data_1:                  return ELF::R_CSE523_DISP8;
            case FK_Data_2:                  return ELF::R_CSE523_DISP16;
            case FK_Data_4:                  return ELF::R_CSE523_DISP32;
            case FK_Data_8:                  return ELF::R_CSE523_DISP64;
            case Cse523::fixup_cse523_call30:  return ELF::R_CSE523_WDISP30;
            case Cse523::fixup_cse523_br22:    return ELF::R_CSE523_WDISP22;
            case Cse523::fixup_cse523_br19:    return ELF::R_CSE523_WDISP19;
            case Cse523::fixup_cse523_pc22:    return ELF::R_CSE523_PC22;
            case Cse523::fixup_cse523_pc10:    return ELF::R_CSE523_PC10;
            case Cse523::fixup_cse523_wplt30:  return ELF::R_CSE523_WPLT30;
        }
    }

    switch((unsigned)Fixup.getKind()) {
        default:
            llvm_unreachable("Unimplemented fixup -> relocation");
        case FK_Data_1:                return ELF::R_CSE523_8;
        case FK_Data_2:                return ((Fixup.getOffset() % 2)
                                               ? ELF::R_CSE523_UA16
                                               : ELF::R_CSE523_16);
        case FK_Data_4:                return ((Fixup.getOffset() % 4)
                                               ? ELF::R_CSE523_UA32
                                               : ELF::R_CSE523_32);
        case FK_Data_8:                return ((Fixup.getOffset() % 8)
                                               ? ELF::R_CSE523_UA64
                                               : ELF::R_CSE523_64);
        case Cse523::fixup_cse523_hi22:  return ELF::R_CSE523_HI22;
        case Cse523::fixup_cse523_lo10:  return ELF::R_CSE523_LO10;
        case Cse523::fixup_cse523_h44:   return ELF::R_CSE523_H44;
        case Cse523::fixup_cse523_m44:   return ELF::R_CSE523_M44;
        case Cse523::fixup_cse523_l44:   return ELF::R_CSE523_L44;
        case Cse523::fixup_cse523_hh:    return ELF::R_CSE523_HH22;
        case Cse523::fixup_cse523_hm:    return ELF::R_CSE523_HM10;
        case Cse523::fixup_cse523_got22: return ELF::R_CSE523_GOT22;
        case Cse523::fixup_cse523_got10: return ELF::R_CSE523_GOT10;
        case Cse523::fixup_cse523_tls_gd_hi22:   return ELF::R_CSE523_TLS_GD_HI22;
        case Cse523::fixup_cse523_tls_gd_lo10:   return ELF::R_CSE523_TLS_GD_LO10;
        case Cse523::fixup_cse523_tls_gd_add:    return ELF::R_CSE523_TLS_GD_ADD;
        case Cse523::fixup_cse523_tls_gd_call:   return ELF::R_CSE523_TLS_GD_CALL;
        case Cse523::fixup_cse523_tls_ldm_hi22:  return ELF::R_CSE523_TLS_LDM_HI22;
        case Cse523::fixup_cse523_tls_ldm_lo10:  return ELF::R_CSE523_TLS_LDM_LO10;
        case Cse523::fixup_cse523_tls_ldm_add:   return ELF::R_CSE523_TLS_LDM_ADD;
        case Cse523::fixup_cse523_tls_ldm_call:  return ELF::R_CSE523_TLS_LDM_CALL;
        case Cse523::fixup_cse523_tls_ldo_hix22: return ELF::R_CSE523_TLS_LDO_HIX22;
        case Cse523::fixup_cse523_tls_ldo_lox10: return ELF::R_CSE523_TLS_LDO_LOX10;
        case Cse523::fixup_cse523_tls_ldo_add:   return ELF::R_CSE523_TLS_LDO_ADD;
        case Cse523::fixup_cse523_tls_ie_hi22:   return ELF::R_CSE523_TLS_IE_HI22;
        case Cse523::fixup_cse523_tls_ie_lo10:   return ELF::R_CSE523_TLS_IE_LO10;
        case Cse523::fixup_cse523_tls_ie_ld:     return ELF::R_CSE523_TLS_IE_LD;
        case Cse523::fixup_cse523_tls_ie_ldx:    return ELF::R_CSE523_TLS_IE_LDX;
        case Cse523::fixup_cse523_tls_ie_add:    return ELF::R_CSE523_TLS_IE_ADD;
        case Cse523::fixup_cse523_tls_le_hix22:  return ELF::R_CSE523_TLS_LE_HIX22;
        case Cse523::fixup_cse523_tls_le_lox10:  return ELF::R_CSE523_TLS_LE_LOX10;
    }

    return ELF::R_CSE523_NONE;
}

const MCSymbol *Cse523ELFObjectWriter::ExplicitRelSym(const MCAssembler &Asm,
        const MCValue &Target,
        const MCFragment &F,
        const MCFixup &Fixup,
        bool IsPCRel) const {

    if (!Target.getSymA())
        return NULL;
    switch((unsigned)Fixup.getKind()) {
        default: break;
        case Cse523::fixup_cse523_got22:
        case Cse523::fixup_cse523_got10:
                 return &Target.getSymA()->getSymbol().AliasedSymbol();
    }
    return NULL;
}

MCObjectWriter *llvm::createCse523ELFObjectWriter(raw_ostream &OS,
        bool Is64Bit,
        uint8_t OSABI) {
    MCELFObjectTargetWriter *MOTW = new Cse523ELFObjectWriter(Is64Bit, OSABI);
    return createELFObjectWriter(MOTW, OS,  /*IsLittleEndian=*/true);
}
