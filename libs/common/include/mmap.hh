#ifndef _MMAP
#define _MMAP

#ifndef _MAX_MEM16
#define _MAX_MEM16 0x10000
#endif
#include <stdint.h>
#include <ext.hh>
#include <type_traits>
#include <vector>

namespace Mem
{
    template<typename Address, typename Data>
    class Extension;

    template<typename Address, typename Data>
    struct Device
    {
        Extension<Address, Data>* dev;
        uint16_t from_addr;
        uint16_t to_addr;
    };

    /**
     * External devices may see the memory mapped differently from the CPU
     * for different reasons
     */
    template<typename Address, typename Data>
    struct DeviceInterconnection
    {
        Extension<Address, Data> *dev;      // the device "dev"
        Address from_addr;                  // see the address from address
        Address to_addr;                    // to address
        Address (*map_function)(Address a); // add an offset in order to see the memory like the CPU would see
    };

    template<typename Address, typename Data>
    class Mem
    {
    private:
        uint8_t* mem;
        std::vector<Device<Address, Data>*> &mapping;
        std::vector<DeviceInterconnection<Address, Data>*> &connections;
    public:
        Mem(
            std::vector<Device<Address, Data>*> &mapping, 
            std::vector<DeviceInterconnection<Address, Data>*> &connections
        ) : mapping(mapping), connections(connections)
        {
            this->mem = new uint8_t[_MAX_MEM16];
            for (std::size_t i = 0; i < _MAX_MEM16; i++) mem[i] = 0;
            for (auto node : mapping)
                node->dev->on_init(node->from_addr, node->to_addr, this);
        }

        void start_post_init()
        {
            for (auto node : mapping)
                node->dev->on_post_init(node->from_addr, node->to_addr, this);
        }

        ~Mem()
        {
            delete mem;
        }

        void write(Address addr, Data data)
        {
            write_silent(addr, data);
            for (auto node : mapping)
            {
                if (node->from_addr <= addr && node->to_addr >= addr)
                    node->dev->on_write(addr, data, this);
            }
        } 

        void write_silent(Address adr, Data data)
        {
            mem[adr] = data;
        } 

        /**
         * Get using normal mapping
         */
        uint8_t operator[](Address addr)
        {
            for (auto node : mapping)
            {
                if (node->from_addr <= addr && node->to_addr >= addr)
                    return node->dev->on_read(addr, this);
            }
            return mem[addr];
        }

        /**
         * Get using interconnection mapping
         */
        uint8_t get_using_intc_mapping(Address addr, Extension<Address, Data> *dev)
        {
            for (auto connection : connections)
            {
                if (connection->dev == dev && connection->from_addr <= addr && connection->to_addr >= addr)
                    return (*this)[connection->map_function(addr)];
            }
            return (*this)[addr];
        }

        uint8_t *get_raw_data()
        {
            return mem;
        }
    };
}

#endif // _MMAP