#pragma once

#include <array>
#include <cstdint>
#include <random>

// Constants defining the CHIP-8 specifications
constexpr unsigned int KEY_COUNT        = 16;    // Number of keys in the CHIP-8 keypad
constexpr unsigned int MEMORY_SIZE      = 4096;  // Size of the CHIP-8 memory
constexpr unsigned int REGISTER_COUNT   = 16;    // Number of registers in the CHIP-8
constexpr unsigned int STACK_LEVELS     = 16;    // Number of stack levels in the CHIP-8
constexpr unsigned int VIDEO_HEIGHT     = 32;    // Height of the CHIP-8 display
constexpr unsigned int VIDEO_WIDTH      = 64;    // Width of the CHIP-8 display

class Chip8
{
public:
    Chip8();
    
    /**
     * Loads a ROM file into the CHIP-8 memory.
     * @param filename The path to the ROM file.
     */
    void LoadROM(const std::string& filename);
    
    /**
     * Executes one cycle of the CHIP-8 CPU.
     */
    void Cycle();

    // CHIP-8 keypad state
    std::array<uint8_t, KEY_COUNT> keypad{};
    
    // CHIP-8 video memory (display)
    std::array<uint32_t, VIDEO_WIDTH * VIDEO_HEIGHT> video{};

private:
    // Function tables for opcode handling
    void Table0();
    void Table8();
    void TableE();
    void TableF();

    // Opcode implementations
    void OP_NULL();    // Do nothing
    void OP_00E0();    // Clear the display
    void OP_00EE();    // Return from a subroutine
    void OP_1nnn();    // Jump to address nnn
    void OP_2nnn();    // Call subroutine at nnn
    void OP_3xkk();    // Skip next instruction if Vx == kk
    void OP_4xkk();    // Skip next instruction if Vx != kk
    void OP_5xy0();    // Skip next instruction if Vx == Vy
    void OP_6xkk();    // Set Vx = kk
    void OP_7xkk();    // Set Vx = Vx + kk
    void OP_8xy0();    // Set Vx = Vy
    void OP_8xy1();    // Set Vx = Vx OR Vy
    void OP_8xy2();    // Set Vx = Vx AND Vy
    void OP_8xy3();    // Set Vx = Vx XOR Vy
    void OP_8xy4();    // Set Vx = Vx + Vy, set VF = carry
    void OP_8xy5();    // Set Vx = Vx - Vy, set VF = NOT borrow
    void OP_8xy6();    // Set Vx = Vx SHR 1
    void OP_8xy7();    // Set Vx = Vy - Vx, set VF = NOT borrow
    void OP_8xyE();    // Set Vx = Vx SHL 1
    void OP_9xy0();    // Skip next instruction if Vx != Vy
    void OP_Annn();    // Set I = nnn
    void OP_Bnnn();    // Jump to location nnn + V0
    void OP_Cxkk();    // Set Vx = random byte AND kk
    void OP_Dxyn();    // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
    void OP_Ex9E();    // Skip next instruction if key with the value of Vx is pressed
    void OP_ExA1();    // Skip next instruction if key with the value of Vx is not pressed
    void OP_Fx07();    // Set Vx = delay timer value
    void OP_Fx0A();    // Wait for a key press, store the value of the key in Vx
    void OP_Fx15();    // Set delay timer = Vx
    void OP_Fx18();    // Set sound timer = Vx
    void OP_Fx1E();    // Set I = I + Vx
    void OP_Fx29();    // Set I = location of sprite for digit Vx
    void OP_Fx33();    // Store BCD representation of Vx in memory locations I, I+1, and I+2
    void OP_Fx55();    // Store registers V0 through Vx in memory starting at location I
    void OP_Fx65();    // Read registers V0 through Vx from memory starting at location I

    // CHIP-8 memory
    std::array<uint8_t, MEMORY_SIZE> memory{};
    
    // CHIP-8 registers
    std::array<uint8_t, REGISTER_COUNT> registers{};
    
    // Delay timer
    uint8_t delayTimer{};
    
    // Sound timer
    uint8_t soundTimer{};
    
    // Stack pointer
    uint8_t sp{};

    // CHIP-8 stack
    std::array<uint16_t, STACK_LEVELS> stack{};
    
    // Index register
    uint16_t index{};
    
    // Program counter
    uint16_t pc{};
    
    // Current opcode
    uint16_t opcode{};

    // Random number generator
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;

    // Function pointers for opcode handling
    using Chip8Func = void (Chip8::*)();
    std::array<Chip8Func, 0xF  + 1> table ;
    std::array<Chip8Func, 0xE  + 1> table0;
    std::array<Chip8Func, 0xE  + 1> table8;
    std::array<Chip8Func, 0xE  + 1> tableE;
    std::array<Chip8Func, 0x65 + 1> tableF;
};