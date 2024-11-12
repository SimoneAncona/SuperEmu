#ifndef _ROM
#define _ROM
#include <ext.hh>
#include <mmap.hh>
#include <fstream>

namespace Generic 
{
    template <typename Address, typename Data>
    class ROM : public Mem::Extension<Address, Data>
    {
    private:
        const std::string filename;
    public:
        ROM(const std::string &filename) : filename(filename)
        {
        }
        
        void on_init(Address from, Address to, Mem::Mem<Address, Data> *mem)
        {
            std::ifstream f(filename, std::ios::binary);
            auto buf = mem->get_raw_data();
            f.read(&((char*)buf)[from], to - from);
            f.close();
        }
    };
}

#endif