#ifndef _MOS6502
#define _MOS6502
#include <stdint.h>
#include <mmap.hh>
#include <cpu.hh>
#include <vector>

#ifndef _MAX_MEM
#define _MAX_MEM 0x10000
#endif

#define _NMI_VECTOR 0xFFFA
#define _RES_VECTOR 0xFFFC
#define _IRQ_VECTOR 0xFFFE

namespace MOS
{
    struct StatusRegister
    {
        uint8_t c : 1;  // carry
        uint8_t z : 1;  // zero
        uint8_t i : 1;  // interrupt
        uint8_t d : 1;  // decimal
        uint8_t b : 1;  // break
        uint8_t _ : 1;  // ignored
        uint8_t v : 1;  // overflow
        uint8_t n : 1;  // negative
    };

    union SRValue
    {
        StatusRegister reg;
        uint8_t v;
    };

    enum AddressingMode
    {
        ACCUMULATOR,
        ABSOLUTE,
        ABSOLUTE_X,
        ABSOLUTE_Y,
        IMMEDIATE,
        IMPLIED,
        INDIRECT,
        X_INDIRECT,
        INDIRECT_Y,
        RELATIVE,
        ZEROPAGE,
        ZEROPAGE_X,
        ZEROPAGE_Y,
    };

    class MOS6502 : public CPU::Cpu<uint16_t, uint8_t>
    {
    private:
        uint8_t ir;             // Current instruction  
        uint16_t pc;            // Program Counter
        uint8_t ac;             // Accumulator
        uint8_t x;              // X register
        uint8_t y;              // Y register
        uint8_t sp;             // Stack pointer
        SRValue sr;             // Status register

        std::string current_instruction;

        AddressingMode current_mode;

        void adc();     // add with carry
        void _and();    // and a
        void asl();     // shift left
        void bcc();     // branch on carry clear    
        void bcs();     // branch on carry set
        void beq();     // branch on zero set
        void bit();     // bit test
        void bmi();     // branch on minus
        void bne();     // branch on zero clear
        void bpl();     // branch on plus
        void brk();     // break
        void bvc();     // branch on overflow clear
        void bvs();     // branch on overflow set
        void clc();     // clear carry
        void cld();     // clear decimal
        void cli();     // clear interrupt
        void clv();     // clear overflow
        void cmp();     // compare a
        void cpx();     // compare x
        void cpy();     // compare y
        void dec();     // decrement
        void dex();     // decrement x
        void dey();     // decrement y
        void eor();     // xor a
        void inc();     // increment
        void inx();     // increment x
        void iny();     // increment y
        void jmp();     // jump
        void jsr();     // jump subroutine
        void lda();     // load a
        void ldx();     // load x
        void ldy();     // load y
        void lsr();     // shift right
        void nop();     // no operation
        void ora();     // or a
        void pha();     // push a
        void php();     // push sr
        void pla();     // pull a
        void plp();     // pull sr
        void rol();     // rotate left
        void ror();     // rotate right
        void rti();     // return from interrupt
        void rts();     // return from subroutine
        void sbc();     // subtract with carry
        void sec();     // set carry
        void sed();     // set decimal
        void sei();     // set interrupt
        void sta();     // store a
        void stx();     // store x
        void sty();     // store y
        void tax();     // transfer a to x
        void tay();     // transfer a to y
        void tsx();     // transfer sp to x
        void txa();     // transfer x to a
        void txs();     // transfer x to sp
        void tya();     // transfer y to a

        void hlt();     // halt
        uint8_t getop();
        uint16_t getaddr();

        void fetch();
        AddressingMode decode();
        void execute();
        int cycles = 0;

    public:
        MOS6502(Mem::Mem<uint16_t, uint8_t> &mem);
        void reset();   // Reset
        void step();    // Perform one instruction
        void irq();     // Interrupt
        void nmi();     // NMI
        void clock();
        std::vector<CPU::RegInfo> get_reg_info();
    };
}

#endif // _MOS6502