#include "../include/Chip8.hpp"
#include <fstream>
#include <random>
#include <algorithm>

constexpr unsigned int FONTSET_SIZE = 80;
constexpr unsigned int FONTSET_START_ADDRESS = 0x50;
constexpr unsigned int START_ADDRESS = 0x200;

std::array<uint8_t, FONTSET_SIZE> fontset = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

/**
 * @brief Constructs a new Chip8 object and initializes its state.
 */
Chip8::Chip8() 
	: randGen(std::random_device{}())
{
	// Initialize PC
	pc = START_ADDRESS;

	// Load fonts into memory
	std::copy(fontset.begin(), fontset.end(), memory.begin() + FONTSET_START_ADDRESS);

	// Initialize RNG
	randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

	// Set up function pointer table
	table[0x0] = &Chip8::Table0;
	table[0x1] = &Chip8::OP_1nnn;
	table[0x2] = &Chip8::OP_2nnn;
	table[0x3] = &Chip8::OP_3xkk;
	table[0x4] = &Chip8::OP_4xkk;
	table[0x5] = &Chip8::OP_5xy0;
	table[0x6] = &Chip8::OP_6xkk;
	table[0x7] = &Chip8::OP_7xkk;
	table[0x8] = &Chip8::Table8;
	table[0x9] = &Chip8::OP_9xy0;
	table[0xA] = &Chip8::OP_Annn;
	table[0xB] = &Chip8::OP_Bnnn;
	table[0xC] = &Chip8::OP_Cxkk;
	table[0xD] = &Chip8::OP_Dxyn;
	table[0xE] = &Chip8::TableE;
	table[0xF] = &Chip8::TableF;

	table0.fill(&Chip8::OP_NULL);
	table8.fill(&Chip8::OP_NULL);
	tableE.fill(&Chip8::OP_NULL);

	table0[0x0] = &Chip8::OP_00E0;
	table0[0xE] = &Chip8::OP_00EE;

	table8[0x0] = &Chip8::OP_8xy0;
	table8[0x1] = &Chip8::OP_8xy1;
	table8[0x2] = &Chip8::OP_8xy2;
	table8[0x3] = &Chip8::OP_8xy3;
	table8[0x4] = &Chip8::OP_8xy4;
	table8[0x5] = &Chip8::OP_8xy5;
	table8[0x6] = &Chip8::OP_8xy6;
	table8[0x7] = &Chip8::OP_8xy7;
	table8[0xE] = &Chip8::OP_8xyE;

	tableE[0x1] = &Chip8::OP_ExA1;
	tableE[0xE] = &Chip8::OP_Ex9E;

	tableF.fill(&Chip8::OP_NULL);

	tableF[0x07] = &Chip8::OP_Fx07;
	tableF[0x0A] = &Chip8::OP_Fx0A;
	tableF[0x15] = &Chip8::OP_Fx15;
	tableF[0x18] = &Chip8::OP_Fx18;
	tableF[0x1E] = &Chip8::OP_Fx1E;
	tableF[0x29] = &Chip8::OP_Fx29;
	tableF[0x33] = &Chip8::OP_Fx33;
	tableF[0x55] = &Chip8::OP_Fx55;
	tableF[0x65] = &Chip8::OP_Fx65;
}

/**
 * @brief Loads a ROM file into the CHIP-8 memory.
 * 
 * @param filename The path to the ROM file.
 */
void Chip8::LoadROM(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<char> buffer(size);
		if (file.read(buffer.data(), size))
		{
			std::copy(buffer.begin(), buffer.end(), memory.begin() + START_ADDRESS);
		}
	}
}

/**
 * @brief Executes one cycle of the CHIP-8 CPU.
 */
void Chip8::Cycle()
{
	// Fetch
	opcode = (memory[pc] << 8u) | memory[pc + 1];

	// Increment the PC before we execute anything
	pc += 2;

	// Decode and Execute
	(this->*table[(opcode & 0xF000u) >> 12u])();

	// Decrement the delay timer if it's been set
	if (delayTimer > 0) --delayTimer;

	// Decrement the sound timer if it's been set
	if (soundTimer > 0) --soundTimer;
}

/**
 * @brief Handles opcodes starting with 0x0.
 */
void Chip8::Table0() { (this->*table0[opcode & 0x000Fu])(); }

/**
 * @brief Handles opcodes starting with 0x8.
 */
void Chip8::Table8() { (this->*table8[opcode & 0x000Fu])(); }

/**
 * @brief Handles opcodes starting with 0xE.
 */
void Chip8::TableE() { (this->*tableE[opcode & 0x000Fu])(); }

/**
 * @brief Handles opcodes starting with 0xF.
 */
void Chip8::TableF() { (this->*tableF[opcode & 0x00FFu])(); }

/**
 * @brief No operation (NOP).
 */
void Chip8::OP_NULL() {}

/**
 * @brief Clears the display.
 */
void Chip8::OP_00E0() { video.fill(0); }

/**
 * @brief Returns from a subroutine.
 */
void Chip8::OP_00EE() { --sp; pc = stack[sp]; }

/**
 * @brief Jumps to address nnn.
 */
void Chip8::OP_1nnn() { pc = opcode & 0x0FFFu; }

/**
 * @brief Calls subroutine at nnn.
 */
void Chip8::OP_2nnn() { stack[sp++] = pc; pc = opcode & 0x0FFFu; }

/**
 * @brief Skips the next instruction if Vx equals kk.
 */
void Chip8::OP_3xkk() { if (registers[(opcode & 0x0F00u) >> 8u] == (opcode & 0x00FFu)) pc += 2; }

/**
 * @brief Skips the next instruction if Vx does not equal kk.
 */
void Chip8::OP_4xkk() { if (registers[(opcode & 0x0F00u) >> 8u] != (opcode & 0x00FFu)) pc += 2; }

/**
 * @brief Skips the next instruction if Vx equals Vy.
 */
void Chip8::OP_5xy0() { if (registers[(opcode & 0x0F00u) >> 8u] == registers[(opcode & 0x00F0u) >> 4u]) pc += 2; }

/**
 * @brief Sets Vx to kk.
 */
void Chip8::OP_6xkk() { registers[(opcode & 0x0F00u) >> 8u] = opcode & 0x00FFu; }

/**
 * @brief Adds kk to Vx.
 */
void Chip8::OP_7xkk() { registers[(opcode & 0x0F00u) >> 8u] += opcode & 0x00FFu; }

/**
 * @brief Sets Vx to the value of Vy.
 */
void Chip8::OP_8xy0() { registers[(opcode & 0x0F00u) >> 8u] = registers[(opcode & 0x00F0u) >> 4u]; }

/**
 * @brief Sets Vx to Vx OR Vy.
 */
void Chip8::OP_8xy1() { registers[(opcode & 0x0F00u) >> 8u] |= registers[(opcode & 0x00F0u) >> 4u]; }

/**
 * @brief Sets Vx to Vx AND Vy.
 */
void Chip8::OP_8xy2() { registers[(opcode & 0x0F00u) >> 8u] &= registers[(opcode & 0x00F0u) >> 4u]; }

/**
 * @brief Sets Vx to Vx XOR Vy.
 */
void Chip8::OP_8xy3() { registers[(opcode & 0x0F00u) >> 8u] ^= registers[(opcode & 0x00F0u) >> 4u]; }

/**
 * @brief Adds Vy to Vx. VF is set to 1 when there's a carry, and to 0 when there isn't.
 */
void Chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint16_t sum = registers[Vx] + registers[Vy];

	registers[0xF] = sum > 255U ? 1 : 0;
	registers[Vx] = sum & 0xFFu;
}

/**
 * @brief Subtracts Vy from Vx. VF is set to 0 when there's a borrow, and 1 when there isn't.
 */
void Chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[0xF] = registers[Vx] > registers[Vy] ? 1 : 0;
	registers[Vx] -= registers[Vy];
}

/**
 * @brief Stores the least significant bit of Vx in VF and then shifts Vx to the right by 1.
 */
void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[0xF] = registers[Vx] & 0x1u;
	registers[Vx] >>= 1;
}

/**
 * @brief Sets Vx to Vy minus Vx. VF is set to 0 when there's a borrow, and 1 when there isn't.
 */
void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[0xF] = registers[Vy] > registers[Vx] ? 1 : 0;
	registers[Vx] = registers[Vy] - registers[Vx];
}

/**
 * @brief Stores the most significant bit of Vx in VF and then shifts Vx to the left by 1.
 */
void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
	registers[Vx] <<= 1;
}

/**
 * @brief Skips the next instruction if Vx does not equal Vy.
 */
void Chip8::OP_9xy0() { if (registers[(opcode & 0x0F00u) >> 8u] != registers[(opcode & 0x00F0u) >> 4u]) pc += 2; }

/**
 * @brief Sets the index register to nnn.
 */
void Chip8::OP_Annn() { index = opcode & 0x0FFFu; }

/**
 * @brief Jumps to the address nnn plus V0.
 */
void Chip8::OP_Bnnn() { pc = registers[0] + (opcode & 0x0FFFu); }

/**
 * @brief Sets Vx to a random byte AND kk.
 */
void Chip8::OP_Cxkk() { registers[(opcode & 0x0F00u) >> 8u] = randByte(randGen) & (opcode & 0x00FFu); }

/**
 * @brief Draws a sprite at coordinate (Vx, Vy) with a width of 8 pixels and a height of n pixels.
 * Each row of 8 pixels is read as bit-coded starting from memory location I.
 * VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn't happen.
 */
void Chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;
	registers[0xF] = 0;

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[index + row];
		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t& screenPixel = video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];
			if (spritePixel && screenPixel == 0xFFFFFFFF) registers[0xF] = 1;
			if (spritePixel) screenPixel ^= 0xFFFFFFFF;
		}
	}
}

/**
 * @brief Skips the next instruction if the key stored in Vx is pressed.
 */
void Chip8::OP_Ex9E() { if (keypad[registers[(opcode & 0x0F00u) >> 8u]]) pc += 2; }

/**
 * @brief Skips the next instruction if the key stored in Vx is not pressed.
 */
void Chip8::OP_ExA1() { if (!keypad[registers[(opcode & 0x0F00u) >> 8u]]) pc += 2; }

/**
 * @brief Sets Vx to the value of the delay timer.
 */
void Chip8::OP_Fx07() { registers[(opcode & 0x0F00u) >> 8u] = delayTimer; }

/**
 * @brief A blocking operation that waits for a key press, then stores the value of the key in Vx.
 */
void Chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i < 16; ++i)
	{
		if (keypad[i])
		{
			registers[Vx] = i;
			return;
		}
	}
	pc -= 2;
}

/**
 * @brief Sets the delay timer to Vx.
 */
void Chip8::OP_Fx15() { delayTimer = registers[(opcode & 0x0F00u) >> 8u]; }

/**
 * @brief Sets the sound timer to Vx.
 */
void Chip8::OP_Fx18() { soundTimer = registers[(opcode & 0x0F00u) >> 8u]; }

/**
 * @brief Adds Vx to I. VF is not affected.
 */
void Chip8::OP_Fx1E() { index += registers[(opcode & 0x0F00u) >> 8u]; }

/**
 * @brief Sets I to the location of the sprite for the digit Vx.
 */
void Chip8::OP_Fx29() { index = FONTSET_START_ADDRESS + (5 * registers[(opcode & 0x0F00u) >> 8u]); }

/**
 * @brief Stores the binary-coded decimal representation of the value in register Vx at memory locations I, I+1, and I+2.
 * 
 * This function extracts the value from the register Vx, which is determined by the lower 12 bits of the opcode.
 * It then calculates the hundreds, tens, and units digits of the value and stores them in consecutive memory locations
 * starting from the address stored in the index register.
 * 
 * Opcode: Fx33
 */
void Chip8::OP_Fx33()
{
    uint8_t Vx = registers[(opcode & 0x0F00u) >> 8u];
    memory[index + 2] = Vx % 10;          // Store the units digit
    memory[index + 1] = (Vx / 10) % 10;   // Store the tens digit
    memory[index] = (Vx / 100) % 10;      // Store the hundreds digit
}

/**
 * @brief Stores registers V0 through Vx in memory starting at location I.
 * 
 * This function copies the values from the registers V0 through Vx into consecutive memory locations
 * starting from the address stored in the index register. The register Vx is determined by the lower 12 bits of the opcode.
 * 
 * Opcode: Fx55
 */
void Chip8::OP_Fx55() 
{ 
    std::copy(registers.begin(), registers.begin() + ((opcode & 0x0F00u) >> 8u) + 1, memory.begin() + index); 
}

/**
 * @brief Fills registers V0 through Vx with values from memory starting at location I.
 * 
 * This function copies values from consecutive memory locations starting from the address stored in the index register
 * into the registers V0 through Vx. The register Vx is determined by the lower 12 bits of the opcode.
 * 
 * Opcode: Fx65
 */
void Chip8::OP_Fx65() 
{ 
    std::copy(memory.begin() + index, memory.begin() + index + ((opcode & 0x0F00u) >> 8u) + 1, registers.begin()); 
}