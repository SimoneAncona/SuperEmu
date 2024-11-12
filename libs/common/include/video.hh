#ifndef _VIDEO
#define _VIDEO
#include <SDL2/SDL.h>
#include <ctx.hh>

namespace IO
{
    class Video
    {
    private:
        static bool initialized;
        static SDL_Texture* fbuf;
        static int fbuf_width;
        static int fbuf_height;
        static void handle_events(void);
    public:
        static void start(void (*callback)(void*), void*);
        static void request_video(int, int, const char*, float);
        static void create_buffer(int, int);
        static void write_video(uint32_t*);
    };
}

#endif