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
        case 11: // rdi
        case 13: // rip
        case 15: // rsi
        case 16: // rsp
        case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: // r8-r15
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
        case 13: // RIP
        case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: // r8-r15
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
        case 15: O << "esi"; break;
        case 16: O << "esp"; break;
        case 13: // RIP
        case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: // r8-r15
        default: 
            assert(0 && "Invalid register number for 8 bit register");  
    }
}

