#include <video.hh>
#include <iostream>
#include <thread>

#ifdef _WIN32
	#include <windows.h>
#endif

SDL_Texture *IO::Video::fbuf = nullptr;
int IO::Video::fbuf_width = 0;
int IO::Video::fbuf_height = 0;

void IO::Video::handle_events()
{
    SDL_Event event;
    if (SDL_PollEvent(&event))
        switch (event.type)
        {
        case SDL_QUIT:
            SDL_Quit();
            break;
        }
}

void IO::Video::start(void (*callback)(void *), void *args)
{
    while (true)
    {
        handle_events();
        SDL_RenderClear(Ctx::renderer);
        callback(args);
        SDL_RenderCopy(Ctx::renderer, Video::fbuf, nullptr, nullptr);
        SDL_RenderPresent(Ctx::renderer);
    }
}

void IO::Video::request_video(int width, int height, const char *title, float scale)
{
    #ifdef _WIN32
        SetProcessDPIAware();
    #endif

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");

    if (SDL_Init(SDL_INIT_VIDEO))
        std::cerr << "Couldn't initialize video" << std::endl;

    Ctx::window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * scale, height * scale, 0);
    if (!Ctx::window)
        std::cerr << "Couldn't create window" << std::endl;

    Ctx::renderer = SDL_CreateRenderer(Ctx::window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!Ctx::renderer)
        std::cerr << "Couldn't create renderer" << std::endl;

    SDL_RenderSetScale(Ctx::renderer, scale, scale);
}

void IO::Video::create_buffer(int width, int height)
{
    Video::fbuf_width = width;
    Video::fbuf_height = height;
    Video::fbuf = SDL_CreateTexture(Ctx::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
}

void IO::Video::write_video(uint32_t *pixels)
{
    SDL_UpdateTexture(Video::fbuf, NULL, pixels, Video::fbuf_width * sizeof (uint32_t));
}
