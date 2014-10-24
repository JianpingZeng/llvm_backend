static void print8ByteRegister(const MCInst *MI, unsigned OpNo,
        raw_ostream &O) {
    const MCOperand &Op = MI->getOperand(OpNo);
    assert(Op.isReg() && "unknown register operand for 8 bit regs");

    O << "%";
    switch(Op.getReg()) {
        case  7: O << "al"; break;
        case  9: O << "bl"; break;
        case 10: O << "cl"; break;
        case 12: O << "dl"; break;
        case 11: O << "dil"; break;
        case 15: O << "sil"; break;
        case 16: O << "spl"; break;
        case 42: O << "r8b"; break;
        case 43: O << "r9b"; break;
        case 44: O << "r10b"; break;
        case 45: O << "r11b"; break;
        case 46: O << "r12b"; break;
        case 47: O << "r13b"; break;
        case 48: O << "r14b"; break;
        case 49: O << "r15b"; break;

        case 13: // RIP
        default: 
            assert(0 && "Invalid register number for 8 bit register");  
    }
}

static void print16ByteRegister(const MCInst *MI, unsigned OpNo,
        raw_ostream &O) {
    const MCOperand &Op = MI->getOperand(OpNo);
    assert(Op.isReg() && "unknown register operand for 16 bit regs");

    O << "%";
    switch(Op.getReg()) {
        case  7: O << "ax"; break;
        case  9: O << "bx"; break;
        case 10: O << "cx"; break;
        case 12: O << "dx"; break;
        case 11: O << "di"; break;
        case 15: O << "si"; break;
        case 16: O << "sp"; break;
        case 42: O << "r8w"; break;
        case 43: O << "r9w"; break;
        case 44: O << "r10w"; break;
        case 45: O << "r11w"; break;
        case 46: O << "r12w"; break;
        case 47: O << "r13w"; break;
        case 48: O << "r14w"; break;
        case 49: O << "r15w"; break;

        case 13: // RIP
        default: 
            assert(0 && "Invalid register number for 8 bit register");  
    }
}

static void print32ByteRegister(const MCInst *MI, unsigned OpNo,
        raw_ostream &O) {
    const MCOperand &Op = MI->getOperand(OpNo);
    assert(Op.isReg() && "unknown register operand for 32 bit regs");

    O << "%";
    switch(Op.getReg()) {
        case  7: O << "eax"; break;
        case  9: O << "ebx"; break;
        case 10: O << "ecx"; break;
        case 12: O << "edx"; break;
        case 11: O << "edi"; break;
        case 13: O << "eip"; break;
        case 15: O << "esi"; break;
        case 16: O << "esp"; break;
        case 42: O << "r8d"; break;
        case 43: O << "r9d"; break;
        case 44: O << "r10d"; break;
        case 45: O << "r11d"; break;
        case 46: O << "r12d"; break;
        case 47: O << "r13d"; break;
        case 48: O << "r14d"; break;
        case 49: O << "r15d"; break;

        default: 
            assert(0 && "Invalid register number for 8 bit register");  
    }
}

