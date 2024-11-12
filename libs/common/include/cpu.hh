#ifndef _CPU
#define _CPU

#include <vector>
#include <string>

namespace CPU
{
    struct RegInfo
    {
        std::string name;
        uint8_t size;   // size in bits
        uint64_t value;
        bool print_binary;  // if the register consists of flags (like status register) it is better to print in binary
        std::string note;
    };

    template <typename Address, typename Data>
    class Cpu
    {
    protected:
        Mem::Mem<uint16_t, uint8_t> &mem;
    public:
        Cpu(Mem::Mem<Address, Data>& mem) : mem(mem) {}
        ~Cpu() {}
        virtual void reset() = 0;
        virtual void step() = 0;
        virtual void clock() = 0;
        virtual void start()
        {
            mem.start_post_init();
        }
        virtual std::vector<RegInfo> get_reg_info() = 0;
    };
}

#endif