#pragma once

#include <cstdint>
#include <SDL.h>

// The Platform class encapsulates the SDL window, OpenGL context, and rendering functionalities.
class Platform
{
	// Granting Imgui class access to private members of Platform.
	friend class Imgui;

public:
	// Constructor: Initializes the SDL window, OpenGL context, and textures.
	// Parameters:
	// - title: The title of the SDL window.
	// - windowWidth: The width of the SDL window.
	// - windowHeight: The height of the SDL window.
	// - textureWidth: The width of the texture used for rendering.
	// - textureHeight: The height of the texture used for rendering.
	Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);

	// Destructor: Cleans up and releases SDL resources.
	~Platform();

	// Updates the texture with new pixel data.
	// Parameters:
	// - buffer: Pointer to the pixel data.
	// - pitch: The number of bytes in a row of pixel data.
	void Update(void const* buffer, int pitch);

	// Processes input events and updates the state of the keys.
	// Parameters:
	// - keys: Pointer to an array representing the state of the keys.
	// Returns:
	// - true if the application should continue running, false if it should quit.
	bool ProcessInput(uint8_t* keys);

private:
	// Pointer to the SDL window.
	SDL_Window* window = nullptr;

	// Handle to the OpenGL context.
	SDL_GLContext gl_context = nullptr;

	// Pointer to the SDL texture used as a framebuffer.
    SDL_Texture* framebuffer_texture = nullptr;

	// Pointer to the SDL renderer.
	SDL_Renderer* renderer = nullptr;

	// Pointer to the SDL texture used for rendering.
	SDL_Texture* texture = nullptr;
};