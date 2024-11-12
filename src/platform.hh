#include <mmap.hh>
#include <cpu.hh>
#include <6502.hh>
#include <6560.hh>
#include <rom.hh>
#include <vector>
#include <thread>
#include <iostream>
#include <bitset>
#include <iomanip>

// template<typename Address, typename Data>
// class Platform
// {
// private:
//     Mem::Mem<Address, Data> mem;
//     const CPU::CPU &cpu;
// public:
//     Platform(const CPU::CPU &cpu) : cpu(cpu)
// };

void loop(MOS::MOS6502 *cpu)
{
    std::cout << "\033[2J";
    while (true)
    {
        auto info = cpu->get_reg_info();
        /*std::cout << "\033[0;0H";
        for (auto &reg : info)
        {
            if (reg.print_binary)
                std::cout << reg.name << "\t" << std::bitset<8>(reg.value) << (reg.note == "" ? "" : " (" + reg.note + ")") << std::endl; 
            else
                std::cout << reg.name << "\t" << std::uppercase << std::setw(reg.size / 4) << std::setfill('0') << std::hex << reg.value << (reg.note == "" ? "" : " (" + reg.note + ")") << std::endl; 
        }
        std::cout << "###################" << std::endl;*/
        cpu->step();
    }
}

void vic20()
{
    auto *kernal = new Mem::Device<uint16_t, uint8_t>();
    kernal->from_addr = 0xE000;
    kernal->to_addr = 0xFFFF;
    kernal->dev = new Generic::ROM<uint16_t, uint8_t>("rom\\commodore vic20\\kernal.rom");

    auto *basic = new Mem::Device<uint16_t, uint8_t>();
    basic->from_addr = 0xC000;
    basic->to_addr = 0xDFFF;
    basic->dev = new Generic::ROM<uint16_t, uint8_t>("rom\\commodore vic20\\basic.rom");

    auto *chars = new Mem::Device<uint16_t, uint8_t>();
    chars->from_addr = 0x8000;
    chars->to_addr = 0x8FFF;
    chars->dev = new Generic::ROM<uint16_t, uint8_t>("rom\\commodore vic20\\characters.rom");

    auto *mos6560 = new Mem::Device<uint16_t, uint8_t>();
    mos6560->from_addr = 0x9000;
    mos6560->to_addr = 0x900F;
    mos6560->dev = new MOS::MOS6560();

    /*
    The MOS6560 was directly connected to the character ROM, so event tho
    it was in 0x8000 address (staring), dev MOS6560 would have used 0x0000 as starting address
    */

    auto *mos6560_charrom = new Mem::DeviceInterconnection<uint16_t, uint8_t>();
    mos6560_charrom->dev = mos6560->dev;
    mos6560_charrom->from_addr = 0x0000;
    mos6560_charrom->to_addr = 0xFFFF;
    mos6560_charrom->map_function = [](uint16_t adr) -> uint16_t {
        if (adr & 0x2000)
            return adr - 0x2000;
        return adr + 0x8000;
    };

    std::vector<Mem::Device<uint16_t, uint8_t>*> mmap = {kernal, basic, chars, mos6560};
    std::vector<Mem::DeviceInterconnection<uint16_t, uint8_t>*> interconnections = {mos6560_charrom};

    Mem::Mem<uint16_t, uint8_t> mem(mmap, interconnections);
    MOS::MOS6502 *m6502 = new MOS::MOS6502(mem);
    std::thread t(loop, m6502);
    m6502->start();
    t.join();
}