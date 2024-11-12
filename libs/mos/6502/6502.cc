#include <6502.hh>

MOS::MOS6502::MOS6502(Mem::Mem<uint16_t, uint8_t> &mem) : Cpu(mem)
{
    this->reset();
}

std::vector<CPU::RegInfo> MOS::MOS6502::get_reg_info()
{
    std::vector<CPU::RegInfo> res = 
    {
        {"PC", 16, this->pc, false, ""},
        {"IR", 8, this->ir, false, current_instruction},
        {"A", 8, this->ac, false, ""},
        {"X", 8, this->x, false, ""},
        {"Y", 8, this->y, false, ""},
        {"SP", 8, this->sp, false, ""},
        {"SR", 8, this->sr.v, true, "N V _ B D I Z C"}
    };
    return res;
}

void MOS::MOS6502::reset()
{
    this->pc = mem[_RES_VECTOR] | mem[_RES_VECTOR + 1] << 8;
    ac = 0;
    x = 0;
    y = 0;
    sp = 0xFF;
    sr.v = 0b00110101;
}

void MOS::MOS6502::step()
{
    this->fetch();
    this->current_mode = this->decode();
    this->execute();
}

void MOS::MOS6502::irq()
{
    if (!sr.reg.i)
        this->pc = mem[_IRQ_VECTOR] | mem[_IRQ_VECTOR + 1] << 8;
}

void MOS::MOS6502::nmi()
{
    this->pc = mem[_NMI_VECTOR] | mem[_NMI_VECTOR + 1] << 8;
}

void MOS::MOS6502::clock()
{
    if (cycles == 0)
    {
        step();
        return;
    }
    cycles--;
}

void MOS::MOS6502::fetch()
{
    this->ir = mem[this->pc++];
}

MOS::AddressingMode MOS::MOS6502::decode()
{
    uint8_t low = this-> ir & 0x03;
    uint8_t admode = this->ir >> 2 & 0x07;
    uint8_t high = this->ir >> 5;
    AddressingMode mode;
    switch (admode)
    {
    case 0:
        if (ir == 0x20)
            return ABSOLUTE;
        if (low == 0 && high <= 3)
            return IMPLIED;
        if (low == 1 || low == 3)
            return X_INDIRECT;
        return IMMEDIATE;
    case 1:
        return ZEROPAGE;
    case 2:
        if (low == 0)
            return IMPLIED;
        if (low == 1 || low == 3)
            return IMMEDIATE;
        if (low == 2 && high <= 3)
            return ACCUMULATOR;
        return IMPLIED;
    case 3:
        if (ir == 0x6C)
            return INDIRECT;
        return ABSOLUTE;
    case 4:
        if (low == 0)
            return RELATIVE;
        return INDIRECT_Y;
    case 5:
        if (ir == 0x96 || ir == 0x97 || ir == 0xB6 || ir == 0xB7)
            return ZEROPAGE_Y;
        return ZEROPAGE_X;
    case 6:
        if (low == 0 || low == 2)
            return IMPLIED;
        return ABSOLUTE_Y;
    case 7:
        if (ir == 0x9E || ir == 0x9F || ir == 0xBE || ir == 0xBF)
            return ABSOLUTE_Y;
        return ABSOLUTE_X;
    }
    return IMPLIED;
}

void MOS::MOS6502::execute()
{
    uint8_t low = this-> ir & 0x03;
    uint8_t admode = this->ir >> 2 & 0x07;
    uint8_t high = this->ir >> 5;

    if (low == 1)
    {
        if (this->ir == 0x89)
        {
            nop();
            return;
        }
        switch (high)
        {
        case 0:
            ora();
            return;
        case 1:
            _and();
            return;
        case 2:
            eor();
            return;
        case 3:
            adc();
            return;
        case 4:
            sta();
            return;
        case 5:
            lda();
            return;
        case 6:
            cmp();
            return;
        case 7:
            sbc();
            return;
        }
    }
    if (low == 2)
    {
        if (admode == 4)
        {
            hlt();
            return;
        }
        switch (ir)
        {
            case 0x02:
            case 0x22:
            case 0x42:
            case 0x62:
                hlt();
                return;
            case 0x1A:
            case 0x3A:
            case 0x5A:
            case 0x7A:
            case 0x82:
            case 0xC2:
            case 0xDA:
            case 0xE2:
            case 0xFA:
                nop();
                return;
            case 0x9A:
                txs();
                return;
            case 0x8A:
                txa();
                return;
            case 0xAA:
                tax();
                return;
            case 0xBA:
                tsx();
                return;
            case 0xCA:
                dex();
                return;
        }

        switch (high)
        {
        case 0:
            asl();
            return;
        case 1:
            rol();
            return;
        case 2:
            lsr();
            return;
        case 3:
            ror();
            return;
        case 4:
            stx();
            return;
        case 5:
            ldx();
            return;
        case 6:
            dec();
            return;
        case 7:
            inc();
            return;
        }
    }

    switch (this->ir)
    {
    case 0x00:
        brk();
        return;
    case 0x04:
    case 0x14:
    case 0x1C:
    case 0x34:
    case 0x3C:
    case 0x44:
    case 0x54:
    case 0x5C:
    case 0x64:
    case 0x74:
    case 0x7C:
    case 0x80:
    case 0xD4:
    case 0xDC:
    case 0xF4:
    case 0xFC:
        nop();
        return;
    case 0x08:
        php();
        return;
    case 0x10:
        bpl();
        return;
    case 0x18:
        clc();
        return;
    case 0x20:
        jsr();
        return;
    case 0x24:
    case 0x2c:
        bit();
        return;
    case 0x28:
        plp();
        return;
    case 0x30:
        bmi();
        return;
    case 0x38:
        sec();
        return;
    case 0x40:
        rti();
        return;
    case 0x48:
        pha();
        return;
    case 0x4c:
    case 0x6c:
        jmp();
        return;
    case 0x50:
        bvc();
        return;
    case 0x58:
        cli();
        return;
    case 0x60:
        rts();
        return;
    case 0x68:
        pla();
        return;
    case 0x70:
        bvs();
        return;
    case 0x78:
        sei();
        return;
    case 0x84:
    case 0x8C:
    case 0x94:
        sty();
        return;
    case 0x88:
        dey();
        return;
    case 0x90:
        bcc();
        return;
    case 0x98:
        tya();
    case 0xA0:
    case 0xA4:
    case 0xAC:
    case 0xB4:
    case 0xBC:
        ldy();
        return;
    case 0xA8:
        tay();
        return;
    case 0xB0:
        bcs();
        return;
    case 0xB8:
        clv();
        return;
    case 0xC0:
    case 0xC4:
    case 0xCC:
        cpy();
        return;
    case 0xC8:
        iny();
        return;
    case 0xD0:
        bne();
        return;
    case 0xD8:
        cld();
        return;
    case 0xE0:
    case 0xE4:
    case 0xEC:
        cpx();
        return;
    case 0xE8:
        inx();
        return;
    case 0xF0:
        beq();
        return;
    case 0xF8:
        sed();
        return;
    }
}

void MOS::MOS6502::hlt()
{
}

uint8_t MOS::MOS6502::getop()
{
    switch (this->current_mode)
    {
    case ACCUMULATOR:
        return this->ac;
    case ABSOLUTE:
    {
        uint16_t addr = mem[pc++] | mem[pc++] << 8;
        return mem[addr];
    }
    case ABSOLUTE_X:
    {
        uint16_t addr = mem[pc++] | mem[pc++] << 8;
        return mem[addr + this->x];
    }
    case ABSOLUTE_Y:
    {
        uint16_t addr = mem[pc++] | mem[pc++] << 8;
        return mem[addr + this->y];
    }
    case IMMEDIATE:
        return mem[pc++];
    case X_INDIRECT:
    {
        uint8_t addr = mem[pc++] + x;
        return mem[mem[addr] | mem[addr + 1] << 8];
    }
    case INDIRECT_Y:
    {
        uint8_t addr = mem[pc++];
        return mem[(mem[addr] | mem[addr + 1] << 8) + y];
    }
    case ZEROPAGE:
        return mem[mem[pc++]];
    case ZEROPAGE_X:
    {
        uint8_t addr = mem[pc++] + x;
        return mem[addr];
    }
    case ZEROPAGE_Y:
    {
        uint8_t addr = mem[pc++] + y;
        return mem[addr];
    }
    }
    return 0;
}

uint16_t MOS::MOS6502::getaddr()
{
    switch (this->current_mode)
    {
    case ACCUMULATOR:
        return this->ac;
    case ABSOLUTE:
        return mem[pc++] | mem[pc++] << 8;
    case ABSOLUTE_X:
        return (mem[pc++] | mem[pc++] << 8) + x;
    case ABSOLUTE_Y:
        return (mem[pc++] | mem[pc++] << 8) + y;
    case X_INDIRECT:
    {
        uint8_t addr = mem[pc++] + x;
        return mem[addr] | mem[addr + 1] << 8;
    }
    case INDIRECT_Y:
    {
        uint8_t addr = mem[pc++];
        return (mem[addr] | mem[addr + 1] << 8) + y;
    }
    case INDIRECT:
    {
        uint16_t addr = mem[pc++] | mem[pc++] << 8;
        return mem[addr] | mem[addr + 1] << 8;
    }
    case RELATIVE:
        return pc + (int8_t)mem[pc++];
    case ZEROPAGE:
        return mem[pc++];
    case ZEROPAGE_X:
        return mem[pc++] + x;
    case ZEROPAGE_Y:
        return mem[pc++] + y;
    }
    return 0;
}

void MOS::MOS6502::adc()
{
    current_instruction = "ADC";
    auto op = getop();
    bool msa = ac & 0x80;
    bool mso = (op + sr.reg.c) & 0x80;
    ac += op + sr.reg.c;
    sr.reg.n = ac & 0x80 ? 1 : 0;
    sr.reg.c = mso && msa; 
    sr.reg.v = (!msa && !mso && (ac & 0x80)) || (msa && mso && !(ac & 0x80));
    sr.reg.z = ac == 0;
}

void MOS::MOS6502::_and()
{
    current_instruction = "AND";
    auto op = getop();
    ac &= op;
    sr.reg.n = ac & 0x80 ? 1 : 0;
    sr.reg.z = ac == 0;
}

void MOS::MOS6502::asl()
{
    current_instruction = "ASL";
    auto tpc = pc;
    auto r = getop();
    sr.reg.c = r >> 7;
    r <<= 1;
    sr.reg.n = r & 0x80 ? 1 : 0;
    sr.reg.z = r == 0;
    if (current_mode == ACCUMULATOR)
    {
        ac = r;
        return;
    }
    pc = tpc;
    mem.write(getaddr(), r);
}

void MOS::MOS6502::bcc()
{
    current_instruction = "BCC";
    auto adr = getaddr();
    if (!sr.reg.c) 
        pc = adr;
}
   
void MOS::MOS6502::bcs()
{
    current_instruction = "BCS";
    auto adr = getaddr();
    if (sr.reg.c) 
        pc = adr;
}

void MOS::MOS6502::beq()
{
    current_instruction = "BEQ";
    auto adr = getaddr();
    if (sr.reg.z) 
        pc = adr;
}

void MOS::MOS6502::bit()
{
    current_instruction = "BIT";
    auto op = getop();
    sr.reg.n = op >> 7;
    sr.reg.v = (op & 0x40) >> 6;
    uint8_t res = ac & op;
    sr.reg.z = res == 0;
}

void MOS::MOS6502::bmi()
{
    current_instruction = "BMI";
    auto adr = getaddr();
    if (sr.reg.n) 
        pc = adr;
}

void MOS::MOS6502::bne()
{
    current_instruction = "BNE";
    auto adr = getaddr();
    if (!sr.reg.z) 
        pc = adr;
}

void MOS::MOS6502::bpl()
{
    current_instruction = "BPL";
    auto adr = getaddr();
    if (!sr.reg.n) 
        pc = adr;
}

void MOS::MOS6502::brk()
{
    current_instruction = "BRK";
    sr.reg.i = 1;
    mem.write(sp-- + 0x0100, (pc + 2) >> 8);
    mem.write(sp-- + 0x0100, (pc + 2) & 0xF);
    sr.reg.b = 1;
    mem.write(sp-- + 0x0100, sr.v);
    sr.reg.b = 0;
    this->pc = mem[_IRQ_VECTOR] | mem[_IRQ_VECTOR + 1] << 8;
}

void MOS::MOS6502::bvc()
{
    current_instruction = "BVC";
    auto adr = getaddr();
    if (!sr.reg.v) 
        pc = adr;
}

void MOS::MOS6502::bvs()
{
    current_instruction = "BVS";
    auto adr = getaddr();
    if (sr.reg.v) 
        pc = adr;
}

void MOS::MOS6502::clc()
{
    current_instruction = "CLC";
    sr.reg.c = 0;
}

void MOS::MOS6502::cld()
{
    current_instruction = "CLD";
    sr.reg.d = 0;
}

void MOS::MOS6502::cli()
{
    current_instruction = "CLI";
    sr.reg.i = 0;
}

void MOS::MOS6502::clv()
{
    current_instruction = "CLV";
    sr.reg.v = 0;
}

void MOS::MOS6502::cmp()
{
    current_instruction = "CMP";
    auto op = getop();
    uint8_t res = ac - op;
    sr.reg.n = res & 0x80 ? 1 : 0;
    sr.reg.c = ac >= op;
    sr.reg.z = res == 0;
}

void MOS::MOS6502::cpx()
{
    current_instruction = "CPX";
    auto op = getop();
    uint8_t res = x - op;
    sr.reg.n = res & 0x80 ? 1 : 0;
    sr.reg.c = x >= op;
    sr.reg.z = res == 0;
}

void MOS::MOS6502::cpy()
{
    current_instruction = "CPY";
    auto op = getop();
    uint8_t res = y - op;
    sr.reg.n = res & 0x80 ? 1 : 0;
    sr.reg.c = y >= op;
    sr.reg.z = res == 0;
}

void MOS::MOS6502::dec()
{
    current_instruction = "DEC";
    auto adr = getaddr();
    uint8_t res = mem[adr] - 1;
    mem.write(adr, res);
    sr.reg.n = res & 0x80 ? 1 : 0;
    sr.reg.z = res == 0;
}

void MOS::MOS6502::dex()
{
    current_instruction = "DEX";
    x--;
    sr.reg.n = x & 0x80 ? 1 : 0;
    sr.reg.z = x == 0;
}

void MOS::MOS6502::dey()
{
    current_instruction = "DEY";
    y--;
    sr.reg.n = y & 0x80 ? 1 : 0;
    sr.reg.z = y == 0;
}

void MOS::MOS6502::eor()
{
    current_instruction = "EOR";
    uint8_t op = getop();
    ac ^= op;
    sr.reg.n = ac & 0x80 ? 1 : 0;
    sr.reg.z = ac == 0;
}

void MOS::MOS6502::inc()
{
    current_instruction = "INC";
    uint16_t adr = getaddr();
    uint8_t res = mem[adr] + 1;
    mem.write(adr, res);
    sr.reg.n = res & 0x80 ? 1 : 0;
    sr.reg.z = res == 0;
}

void MOS::MOS6502::inx()
{
    current_instruction = "INX";
    x++;
    sr.reg.n = x & 0x80 ? 1 : 0;
    sr.reg.z = x == 0;
}

void MOS::MOS6502::iny()
{
    current_instruction = "INY";
    y++;
    sr.reg.n = y & 0x80 ? 1 : 0;
    sr.reg.z = y == 0;
}

void MOS::MOS6502::jmp()
{
    current_instruction= "JMP";
    pc = getaddr();
}

void MOS::MOS6502::jsr()
{
    current_instruction = "JSR";
    auto adr = getaddr();
    mem.write(sp-- + 0x0100, pc >> 8);
    mem.write(sp-- + 0x0100, pc & 0xFF);
    pc = adr;
}

void MOS::MOS6502::lda()
{
    current_instruction = "LDA";
    ac = getop();
    sr.reg.n = ac & 0x80 ? 1 : 0;
    sr.reg.z = ac == 0;
}

void MOS::MOS6502::ldx()
{
    current_instruction = "LDX";
    x = getop();
    sr.reg.n = x & 0x80 ? 1 : 0;
    sr.reg.z = x == 0;
}

void MOS::MOS6502::ldy()
{
    current_instruction = "LDY";
    y = getop();
    sr.reg.n = y & 0x80 ? 1 : 0;
    sr.reg.z = y == 0;
}

void MOS::MOS6502::lsr()
{
    current_instruction = "LSR";
    auto tpc = pc;
    auto r = getop();
    sr.reg.c = r & 0x01;
    r >>= 1;
    sr.reg.n = 0;
    sr.reg.z = r == 0;
    if (current_mode == ACCUMULATOR)
    {
        ac = r;
        return;
    }
    pc = tpc;
    mem.write(getaddr(), r);
}

void MOS::MOS6502::nop()
{
    current_instruction = "NOP";
    getop();
}

void MOS::MOS6502::ora()
{
    current_instruction = "ORA";
    uint8_t op = getop();
    ac |= op;
    sr.reg.n = ac & 0x80 ? 1 : 0;
    sr.reg.z = ac == 0;
}

void MOS::MOS6502::pha()
{
    current_instruction = "PHA";
    mem.write(sp-- + 0x0100, ac);
}

void MOS::MOS6502::php()
{
    current_instruction = "PHP";
    sr.reg.b = 1;
    mem.write(sp-- + 0x0100, sr.v);
    sr.reg.b = 0;
}

void MOS::MOS6502::pla()
{
    current_instruction = "PLA";
    ac = mem[++sp + 0x0100];
    sr.reg.n = ac & 0x80 ? 1 : 0;
    sr.reg.z = ac == 0;
}

void MOS::MOS6502::plp()
{
    current_instruction = "PLP";
    sr.v = mem[++sp + 0x0100];
}

void MOS::MOS6502::rol()
{
    current_instruction = "ROL";
    auto tpc = pc;
    auto r = getop();
    uint8_t t = r >> 7;
    r <<= 1;
    r |= sr.reg.c;
    sr.reg.c = t;
    sr.reg.n = r & 0x80 ? 1 : 0;
    sr.reg.z = r == 0;
    if (current_mode == ACCUMULATOR)
    {
        ac = r;
        return;
    }
    pc = tpc;
    mem.write(getaddr(), r);
}

void MOS::MOS6502::ror()
{
    current_instruction = "ROR";
    auto tpc = pc;
    auto r = getop();
    uint8_t t = r & 0x1;
    r >>= 1;
    r |= (sr.reg.c << 7);
    sr.reg.c = t;
    sr.reg.n = r & 0x80 ? 1 : 0;
    sr.reg.z = r == 0;
    if (current_mode == ACCUMULATOR)
    {
        ac = r;
        return;
    }
    pc = tpc;
    mem.write(getaddr(), r);
}

void MOS::MOS6502::rti()
{
    current_instruction = "RII";
    sr.v = mem[++sp + 0x0100];
    sr.reg.b = 0;
    pc = mem[++sp + 0x0100] | mem[++sp + 0x0100] << 8;
}

void MOS::MOS6502::rts()
{
    current_instruction = "RTS";
    pc = mem[++sp + 0x0100] | mem[++sp + 0x0100] << 8;
}

void MOS::MOS6502::sbc()
{
    current_instruction = "SBC";
    auto op = getop();
    op = (~op + 1) - !sr.reg.c;
    bool msa = ac & 0x80;
    bool mso = op & 0x80;
    ac += op;
    sr.reg.n = ac & 0x80 ? 1 : 0;
    sr.reg.c = mso && msa; 
    sr.reg.v = (msa && mso && (ac & 0x80)) || (!msa && !mso && !(ac & 0x80));
    sr.reg.z = ac == 0;
}

void MOS::MOS6502::sec()
{
    current_instruction = "SEC";
    sr.reg.c = 1;
}

void MOS::MOS6502::sed()
{
    current_instruction = "SED";
    sr.reg.d = 1;
}

void MOS::MOS6502::sei()
{
    current_instruction = "SEI";
    sr.reg.i = 1;
}

void MOS::MOS6502::sta()
{
    current_instruction = "STA";
    auto adr = getaddr();
    mem.write(adr, ac);
}

void MOS::MOS6502::stx()
{
    current_instruction = "STX";
    auto adr = getaddr();
    mem.write(adr, x);
}

void MOS::MOS6502::sty()
{
    current_instruction = "STY";
    auto adr = getaddr();
    mem.write(adr, y);
}

void MOS::MOS6502::tax()
{
    current_instruction = "TAX";
    x = ac;
    sr.reg.n = x & 0x80 ? 1 : 0;
    sr.reg.z = x == 0;
}

void MOS::MOS6502::tay()
{
    current_instruction = "TAY";
    y = ac;
    sr.reg.n = y & 0x80 ? 1 : 0;
    sr.reg.z = y == 0;
}

void MOS::MOS6502::tsx()
{
    current_instruction = "TSX";
    x = sp;
    sr.reg.n = x & 0x80 ? 1 : 0;
    sr.reg.z = x == 0;
}

void MOS::MOS6502::txa()
{
    current_instruction = "TXA";
    ac = x;
    sr.reg.n = ac & 0x80 ? 1 : 0;
    sr.reg.z = ac == 0;
}

void MOS::MOS6502::txs()
{
    current_instruction = "TXS";
    sp = x;
}

void MOS::MOS6502::tya()
{
    current_instruction = "TYA";
    ac = y;
    sr.reg.n = ac & 0x80 ? 1 : 0;
    sr.reg.z = ac == 0;
}