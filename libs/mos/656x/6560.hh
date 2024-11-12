#ifndef _MOS6560
#define _MOS6560
#include <stdint.h>
#include <mmap.hh>
#include <video.hh>

namespace MOS
{
    using namespace IO;
    class MOS6560 : public Mem::Extension<uint16_t, uint8_t>
    {
    private:
        uint32_t color_palette[16] = {
            0x00000000,  // black
            0xFFFFFF00,  // white
            0x8D3E3700,  // red
            0x72C1C800,  // cyan
            0x80348B00,  // purple
            0x55A04900,  // green
            0x40318D00,  // blue
            0xAAB95D00,  // yellow
            0x8B542900,  // orange
            0xD59F7400,  // light orange
            0xB8696200,  // pink
            0x87D6DD00,  // light cyan
            0xAA5FB600,  // light purple
            0x94E08900,  // light green
            0x8071CC00,  // light blue
            0xFFFFB200   // light yellow
        };
        void update(uint8_t*, Mem::Mem<uint16_t, uint8_t>*);
        uint32_t *framebuffer;

        uint8_t columns;
        uint8_t rows;
        bool char_size;
        uint16_t video_adr;
        uint16_t char_adr;
        uint8_t fgcolor_code;
        uint8_t bgcolor_code;

        uint32_t fgcolor;
        uint32_t bgcolor;
        void print_char(Mem::Mem<uint16_t, uint8_t>*, uint16_t);
    public:
        MOS6560();
        void on_init(uint16_t, uint16_t, Mem::Mem<uint16_t, uint8_t>*);
        void on_post_init(uint16_t, uint16_t, Mem::Mem<uint16_t, uint8_t>*);
    };
}

#endif