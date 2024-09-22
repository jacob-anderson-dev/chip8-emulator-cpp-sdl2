#include "../include/Platform.hpp"
#include <SDL.h>

// Constructor: Initializes the SDL window, OpenGL context, and textures.
Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
    // Initialize SDL with video support
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return;
    }

    // Create the SDL Window
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight,
        SDL_WINDOW_RESIZABLE
    );

    // Check if window creation was successful
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return;
    }

    // Create SDL Renderer for the window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    // Check if renderer creation was successful
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return;
    }

    // Create the SDL texture for the framebuffer
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,  // Equivalent to GL_RGBA
        SDL_TEXTUREACCESS_STREAMING, // Allows manual pixel updates
        textureWidth, textureHeight
    );

    // Check if texture creation was successful
    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return;
    }
}

// Destructor: Cleans up and releases SDL resources.
Platform::~Platform()
{
    // Clean up SDL resources
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

// Updates the texture with new pixel data.
void Platform::Update(void const* buffer, int pitch)
{
    // Update the texture with the provided buffer
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);

    // Clear the screen
    SDL_RenderClear(renderer);

    // Copy the updated texture to the rendering target (the window)
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    // Present the updated frame on the window
    SDL_RenderPresent(renderer);
}

// Processes input events and updates the state of the keys.
bool Platform::ProcessInput(uint8_t* keys)
{
    bool quit = false;
    SDL_Event event;

    // Poll for events and handle them
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                // Handle quit event
                quit = true;
                break;

            case SDL_KEYDOWN:
            {
                // Handle key down events
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: quit = true; break;
                    case SDLK_x: keys[0] = 1; break;
                    case SDLK_1: keys[1] = 1; break;
                    case SDLK_2: keys[2] = 1; break;
                    case SDLK_3: keys[3] = 1; break;
                    case SDLK_q: keys[4] = 1; break;
                    case SDLK_w: keys[5] = 1; break;
                    case SDLK_e: keys[6] = 1; break;
                    case SDLK_a: keys[7] = 1; break;
                    case SDLK_s: keys[8] = 1; break;
                    case SDLK_d: keys[9] = 1; break;
                    case SDLK_z: keys[0xA] = 1; break;
                    case SDLK_c: keys[0xB] = 1; break;
                    case SDLK_4: keys[0xC] = 1; break;
                    case SDLK_r: keys[0xD] = 1; break;
                    case SDLK_f: keys[0xE] = 1; break;
                    case SDLK_v: keys[0xF] = 1; break;
                }
            }
            break;

            case SDL_KEYUP:
            {
                // Handle key up events
                switch (event.key.keysym.sym)
                {
                    case SDLK_x: keys[0] = 0; break;
                    case SDLK_1: keys[1] = 0; break;
                    case SDLK_2: keys[2] = 0; break;
                    case SDLK_3: keys[3] = 0; break;
                    case SDLK_q: keys[4] = 0; break;
                    case SDLK_w: keys[5] = 0; break;
                    case SDLK_e: keys[6] = 0; break;
                    case SDLK_a: keys[7] = 0; break;
                    case SDLK_s: keys[8] = 0; break;
                    case SDLK_d: keys[9] = 0; break;
                    case SDLK_z: keys[0xA] = 0; break;
                    case SDLK_c: keys[0xB] = 0; break;
                    case SDLK_4: keys[0xC] = 0; break;
                    case SDLK_r: keys[0xD] = 0; break;
                    case SDLK_f: keys[0xE] = 0; break;
                    case SDLK_v: keys[0xF] = 0; break;
                }
            }
            break;
        }
    }

    return quit;
}