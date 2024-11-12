#include <6560.hh>
#include <thread>

#define NTSC_WIDTH 176
#define NTSC_HEIGHT 184

void MOS::MOS6560::print_char(Mem::Mem<uint16_t, uint8_t> *mem, uint16_t index)
{
    uint8_t r = char_size ? 16 : 8;
    uint8_t current_column = index % columns;
    uint8_t current_row = index / columns;
    for (uint8_t i = 0; i < r; i++)
    {
        uint16_t ch = mem->get_using_intc_mapping(char_adr + i + mem->get_using_intc_mapping(video_adr + index, this) * 8, this);
        for (uint8_t j = 0; j < r; j++)
            framebuffer[((j + current_column * r) + NTSC_WIDTH * (i + (current_row * 8)))] = (ch & (0x80 >> j)) ? fgcolor : bgcolor;
    }
}

void MOS::MOS6560::update(uint8_t *regs, Mem::Mem<uint16_t, uint8_t> *mem)
{
    columns = regs[2] & 0x7F;
    rows = (regs[3] >> 1) & 0x3F;
    char_size = regs[3] & 0x01;     // 0 = 8 x 8 chars, 1 = 16 x 8 chars
    video_adr = (regs[5] & 0xF0) << 6 | (regs[2] & 0x80) << 2;
    char_adr = regs[5] & 0x0F << 10;
    fgcolor_code = regs[15] & 0x07;
    bgcolor_code = regs[15] >> 4;

    fgcolor = color_palette[fgcolor_code];
    bgcolor = color_palette[bgcolor_code];

    for (uint16_t i = 0; i < rows * columns; i++)
    {
        print_char(mem, i);
    }

    Video::write_video(framebuffer);
}

MOS::MOS6560::MOS6560()
{
    Video::request_video(NTSC_WIDTH, NTSC_HEIGHT, "MOS6560", 4);
    framebuffer = new uint32_t[NTSC_WIDTH * NTSC_HEIGHT];
}

void MOS::MOS6560::on_init(uint16_t from, uint16_t to, Mem::Mem<uint16_t, uint8_t> *mem)
{
    mem->write_silent(from, 0x05);
    mem->write_silent(from + 1, 0x19);
    mem->write_silent(from + 2, 0x16);
    mem->write_silent(from + 3, 0x2E);
    mem->write_silent(from + 5, 0xF0);
    mem->write_silent(from + 6, 0x00);
    mem->write_silent(from + 7, 0x00);
    mem->write_silent(from + 8, 0xFF);
    mem->write_silent(from + 9, 0xFF);
    mem->write_silent(from + 10, 0x00);
    mem->write_silent(from + 11, 0x00);
    mem->write_silent(from + 12, 0x00);
    mem->write_silent(from + 13, 0x00);
    mem->write_silent(from + 14, 0x00);
    mem->write_silent(from + 15, 0x1B);
}

void MOS::MOS6560::on_post_init(uint16_t from, uint16_t to, Mem::Mem<uint16_t, uint8_t> *mem)
{
    Video::create_buffer(NTSC_WIDTH, NTSC_HEIGHT);
    Video::start([](void* a)
    {
        auto m6560 = (MOS6560*)((void**)a)[0];
        m6560->update((uint8_t*)((void**)a)[1], (Mem::Mem<uint16_t, uint8_t>*)((void**)a)[2]);
    }, new void*[]{this, mem->get_raw_data() + from, mem});
}