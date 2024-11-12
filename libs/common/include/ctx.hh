#ifndef _CTX
#define _CTX

#include <SDL2/SDL.h>

namespace IO
{
    struct Ctx
    {
        static SDL_Window *window;
        static SDL_Renderer *renderer;
    };
}

#endif