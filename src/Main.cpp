#include "../include/Chip8.hpp"
#include "../include/Platform.hpp"
#include <chrono>
#include <iostream>
#include <cstdlib>

/**
 * @brief Entry point for the CHIP-8 emulator.
 * 
 * This function initializes the emulator, processes command-line arguments,
 * sets up the platform window and CHIP-8 instance, and runs the main emulation loop.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return int Returns EXIT_SUCCESS on successful execution, or EXIT_FAILURE on error.
 */
int main(int argc, char** argv)
{
    // Ensure correct usage
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        return EXIT_FAILURE;
    }

    // Parse command-line arguments
    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    const std::string romFilename = argv[3];

    // Create platform window and chip8 instance
    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);
    Chip8 chip8;
    chip8.LoadROM(romFilename);

    // Determine pitch for rendering the video buffer
    const int videoPitch = static_cast<int>(sizeof(chip8.video[0]) * VIDEO_WIDTH);

    // Initialize timing variables
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    // Main loop
    while (!quit)
    {
        // Process user input
        quit = platform.ProcessInput(chip8.keypad.data());

        // Calculate time difference between cycles
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        // Execute the next cycle if the delay has passed
        if (dt > static_cast<float>(cycleDelay))
        {
            lastCycleTime = currentTime;

            // Emulate one cycle of CHIP-8
            chip8.Cycle();

            // Update the display with the latest video buffer
            platform.Update(chip8.video.data(), videoPitch);
        }
    }

    return 0;
}