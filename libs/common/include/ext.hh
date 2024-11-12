#ifndef _EXT
#define _EXT
#include <stdint.h>
#include <mmap.hh>

namespace Mem
{
    template<typename A, typename D>
    class Mem;

    template<typename Address, typename Data>
    class Extension 
    {
    public:
        virtual ~Extension() {}

        virtual Data on_read(Address addr, Mem<Address, Data> *mem)
        {
            return mem->get_raw_data()[addr];
        }

        virtual void on_write(Address addr, Data data, Mem<Address, Data> *mem)
        {
            mem->write_silent(addr, data);
        }

        virtual void on_init(Address, Address, Mem<Address, Data>*)
        {
        }

        virtual void on_post_init(Address, Address, Mem<Address, Data>*)
        {
        }
    };
}

#endif
