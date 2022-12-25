#include "chip-8.h"
#include <chrono>
#include <random>
#include <fstream>

const unsigned int FONT_SET_SIZE = 80;
const unsigned int FONT_START_ADDRESS = 0x050;
const unsigned int ROM_START_ADDRESS = 0x200;

uint8_t fontSet[FONT_SET_SIZE] =  
	{
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


//:randGen(std::chrono:system_clock::now().time_since_epoch().count())
Chip8::Chip8()
{
	// sets the pc to the starting address location
	pc = ROM_START_ADDRESS;
	
	// Loads the font set into the RAM
	for(unsigned int i = 0; i < FONT_SET_SIZE; i++)
	{
		memory[FONT_START_ADDRESS + i] = fontSet[i];
	}

	
	
}

//loads binary file data into the correct spot in memory
void Chip8::loadROM(char const* filename)
{
	//creates an ifstream object tied to the given filename
	//also specifies that the file is binary
	std::ifstream rom_file(filename, std::ios_base::binary);
		
	if(rom_file.is_open())
	{
		//gets the length of the file and reads in data from file to a buffer;
		rom_file.seekg(0, rom_file.end);
		int length = rom_file.tellg();
		rom_file.seekg(0, rom_file.beg);
		char* buffer = new char[length];
		rom_file.read(buffer, length);
		rom_file.close();
			
		for(long i = 0; i < length; i++)
		{
			Chip8::memory[ROM_START_ADDRESS + i] = buffer[i];
		}
			
		delete[] buffer;
	}
}

//Clear Screen
void Chip8::op_00E0()
{
	//TO DO
}

//RETURN
void Chip8::op_00EE()
{
	--stack_pointer;
	pc = stack[stack_pointer];
}

//JUMP
void Chip8::op_1nnn()
{
	pc = opcodes & 0x0FFFU; 
}

//CALL
void Chip8::op_2nnn()
{	
	uint16_t address = opcodes & 0x0FFFU;
	stack[stack_pointer] = pc;
	++stack_pointer;
	pc = address;
	
}

void Chip8::op_3xkk()
{
	uint8_t compared_value = opcodes & 0x00FFU;
	uint8_t Vx = (opcodes & 0x0F00U)>> 8U;
	
	if(compared_value == regesters[Vx])
	{
		pc += 2;
	}	
}

void Chip8::op_4xkk()
{
	uint8_t compared_value = opcodes & 0x00FFU;
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	
	if(compared_value != regesters[Vx])
	{
		pc += 2;
	}
}

void Chip8::op_5xy0()
{
	uint8_t Vy = (opcodes & 0x00F0) >> 4U;
	uint8_t Vx = (opcodes & 0x0F00) >> 8U;
	
	if(regesters[Vx] == regesters[Vy])
	{
		pc += 2;
	}
}

void Chip8::op_6xkk()
{
	uint8_t load_value = opcodes & 0x00FFU;
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	regesters[Vx] = load_value;
}

void Chip8::op_7xkk()
{
	uint8_t byte = opcodes & 0x00FFU;
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	regesters[Vx] += byte;
}

void Chip8::op_8xy0()
{
	uint8_t Vy = (opcodes & 0x00F0U) >> 4U;
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	regesters[Vx] = regesters[Vy];
}

void Chip8::op_8xy1()
{
	uint8_t Vy = (opcodes & 0x00F0U) >> 4U;
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	
	regesters[Vx] = (regesters[Vx] | regesters[Vy]);
}

void Chip8::op_8xy2()
{
	uint8_t Vy = (opcodes & 0x00F0U) >> 4U;
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	
	regesters[Vx] = (regesters[Vx] & regesters[Vy]);
}

void Chip8::op_8xy3()
{
	uint8_t Vy = (opcodes & 0x00F0U) >> 4U;
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	regesters[Vx] = (regesters[Vx] ^ regesters[Vy]);
}

void Chip8::op_8xy4()
{
	uint8_t Vy = (opcodes & 0x00F0U) >> 4U;
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;

	uint8_t sum = regesters[Vx] + regesters[Vy];
	if(sum > 255U)
	{
		regesters[NUM_REGESTERS] = 1;
	}
	else
	{
		regesters[NUM_REGESTERS] = 0;
	}
	
	regesters[Vx] = sum & 0x00FFU;
}

void Chip8::op_8xy5()
{
	uint8_t Vy = (opcodes & 0x00F0U) >> 4U;
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;

	uint8_t difference = regesters[Vx] - regesters[Vy];
	if(regesters[Vx] > regesters[Vy])
	{
		regesters[NUM_REGESTERS] = 1;
	}
	else
	{
		regesters[NUM_REGESTERS] = 0;
	}
	
	regesters[Vx] = difference;
}

void Chip8::op_8xy6()
{	
	//if the last bit of value stored in Vx is 1 set Vf to 1 else set to 0
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	regesters[NUM_REGESTERS] = regesters[Vx] & 0x1U;
	
	//divide regester Vx by 2
	regesters[Vx] = regesters[Vx] >> 1U;
}

void Chip8::op_8xy7()
{
	
}

void Chip8::op_8xyE()
{
	//TO DO
}

void Chip8::op_9xy0()
{
	//TO DO
}

void Chip8::op_Annn()
{
	//TO DO
}

void Chip8::op_Bnnn()
{
	//TO DO
}

void Chip8::op_Cxkk()
{
	//To DO
}

void Chip8::op_Dxyn()
{
	//TO DO
}

void Chip8::op_Ex9E()
{
	//TO DO
}

void Chip8::op_ExA1()
{
	//TO DO
}

void Chip8::op_Fx07()
{
	//To DO
}

void Chip8::op_Fx0A()
{
	//To DO
}

void Chip8::op_Fx15()
{
	//To DO
}

void Chip8::op_Fx18()
{
	//To DO
}

void Chip8::op_Fx1E()
{
	//To DO
}

void Chip8::op_Fx29()
{
	//To DO
}

void Chip8::op_Fx33()
{
	//TO DO 
}

void Chip8::op_Fx55()
{
	//To DO
}

void Chip8::op_Fx65()
{
	//To DO
}