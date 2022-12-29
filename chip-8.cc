#include "chip-8.h"
#include <chrono>
#include <random>
#include <fstream>
#include <functional>

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



Chip8::Chip8()
	:rng(std::chrono::system_clock::now().time_since_epoch().count())
{
	random_byte = std::uniform_int_distribution<uint8_t>(0, 255U);	

	// initializes variables
	pc = ROM_START_ADDRESS;
	index_regester = 0;
	stack_pointer = 0;
	sound_timer = 0;
	delay_timer = 0;
	opcodes = 0;
	
	//initialize function Tables
	FunctionTable[0x0] = &Chip8::table0;
	FunctionTable[0x1] = &Chip8::op_1nnn; 
	FunctionTable[0x2] = &Chip8::op_2nnn;
	FunctionTable[0x3] = &Chip8::op_3xkk;
	FunctionTable[0x4] = &Chip8::op_4xkk;
	FunctionTable[0x5] = &Chip8::op_5xy0;
	FunctionTable[0x6] = &Chip8::op_6xkk;
	FunctionTable[0x7] = &Chip8::op_7xkk;
	FunctionTable[0x8] = &Chip8::table8;
	FunctionTable[0x9] = &Chip8::op_9xy0;
	FunctionTable[0xA] = &Chip8::op_Annn;
	FunctionTable[0xB] = &Chip8::op_Bnnn;
	FunctionTable[0xC] = &Chip8::op_Cxkk;
	FunctionTable[0xD] = &Chip8::op_Dxyn;
	FunctionTable[0xE] = &Chip8::tableE;
	FunctionTable[0xF] = &Chip8::tableF;
	
	Table0[0x0] = &Chip8::op_00E0;
	Table0[0xE] = &Chip8::op_00EE;

	Table8[0x0] = &Chip8::op_8xy0;
	Table8[0x1] = &Chip8::op_8xy1;
	Table8[0x2] = &Chip8::op_8xy2;
	Table8[0x3] = &Chip8::op_8xy3;
	Table8[0x4] = &Chip8::op_8xy4;
	Table8[0x5] = &Chip8::op_8xy5;
	Table8[0x6] = &Chip8::op_8xy6;
	Table8[0x7] = &Chip8::op_8xy7;
	Table8[0xE] = &Chip8::op_8xyE;

	TableE[0x1] = &Chip8::op_ExA1;
	TableE[0xE] = &Chip8::op_Ex9E;

	TableF[0x07] = &Chip8::op_Fx07;
	TableF[0x0A] = &Chip8::op_Fx0A;
	TableF[0x15] = &Chip8::op_Fx15;
	TableF[0x18] = &Chip8::op_Fx18;
	TableF[0x1E] = &Chip8::op_Fx1E;
	TableF[0x29] = &Chip8::op_Fx29;
	TableF[0x33] = &Chip8::op_Fx33;
	TableF[0x55] = &Chip8::op_Fx55;
	TableF[0x65] = &Chip8::op_Fx65;
	
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

void Chip8::cycle()
{
	//we left shift by 8 to make a 16byte adress (adds 8 zeros to the right of the starting value)
	opcodes = (memory[ROM_START_ADDRESS] << 8U) | memory[ROM_START_ADDRESS + 1];
	pc += 2;
	
	//this syntax is disgusting but essentialy we are dereferencing the memory address that contains the function we want to call
	//then calling it from the chip8 object through this	
	(this->*(FunctionTable[(opcodes & 0xF000U) >> 12U]))();		
	
	if(delay_timer > 0)
	{	 
		delay_timer -= 1;
	}	
	if(sound_timer > 0)
	{
		sound_timer -= 1;	
	}

}

void Chip8::table0()
{
	(this->*(Table0[opcodes & 0x000FU]))();
}

void Chip8::table8()
{
	(this->*(Table8[opcodes & 0x000FU]))();	
}

void Chip8::tableE()
{
	(this->*(TableE[opcodes & 0x000FU]))();
}

void Chip8::tableF()
{
	(this->*(TableF[opcodes * 0x00FFU]))();
}

//CLS clears the display
void Chip8::op_00E0()
{
	std::memset(display, 0, sizeof(display));
}

//RET returns from a subroutine
void Chip8::op_00EE()
{
	--stack_pointer;
	pc = stack[stack_pointer];
}

//JP jump to location nnn
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
		regesters[0xF] = 1;
	}
	else
	{
		regesters[0xF] = 0;
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
		regesters[0xF] = 1;
	}
	else
	{
		regesters[0xF] = 0;
	}
	
	regesters[Vx] = difference;
}

void Chip8::op_8xy6()
{	
	//if the last bit of value stored in Vx is 1 set Vf to 1 else set to 0
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	regesters[0xF] = regesters[Vx] & 0x1U;
	
	//divide regester Vx by 2
	regesters[Vx] = regesters[Vx] >> 1U;
}

void Chip8::op_8xy7()
{
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	uint8_t Vy = (opcodes & 0x00F0U) >> 4U;

	if(regesters[Vy] > regesters[Vx])
	{
		regesters[0xF] = 1;
	}	
	else
	{
		regesters[0xF] = 0;
	}

	regesters[Vx] = regesters[Vy] - regesters[Vx]; 	
}

void Chip8::op_8xyE()
{
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	uint8_t Vy = (opcodes & 0x00F0U) >> 4U;
	
	regesters[0xF] = (regesters[Vx] & 0x80U) >> 7U;
	
	regesters[Vx] = regesters[Vx] << 1U;
}

void Chip8::op_9xy0()
{
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	uint8_t Vy = (opcodes & 0x00F0U) >> 4U;
	
	if(regesters[Vx] != regesters[Vy])
	{
		pc += 2;
	}

}

void Chip8::op_Annn()
{
	uint16_t address = opcodes & 0x0FFFU;
	index_regester = address;
}

void Chip8::op_Bnnn()
{
	uint16_t address = opcodes & 0x0FFFU;
	pc = address + regesters[0];
}

void Chip8::op_Cxkk()
{
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	uint8_t byte = opcodes & 0x00FFU;
	
	regesters[Vx] = random_byte(rng) & byte;	
}

void Chip8::op_Dxyn()
{
	//TO DO	
}

void Chip8::op_Ex9E()
{
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	uint8_t key = regesters[Vx];

	if(keypad[key])
	{
		pc += 2;
	}
}

void Chip8::op_ExA1()
{
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	uint8_t key = regesters[Vx];
	
	if(!keypad[key])
	{
		pc += 2;
	}
}

void Chip8::op_Fx07()
{
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	regesters[Vx] = delay_timer;
}

void Chip8::op_Fx0A()
{
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	
	for(int i = 0x0U; i <= 0xF; i++)
	{
		if(keypad[i])
		{
			regesters[Vx] = i;
			return;	
		}	
	}

	pc -= 2;	
}

void Chip8::op_Fx15()
{
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	delay_timer = regesters[Vx];

}

void Chip8::op_Fx18()
{
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	sound_timer = regesters[Vx];

}

void Chip8::op_Fx1E()
{
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	index_regester = index_regester + regesters[Vx];
}

//LD F, Vx Set I equal to the location of sprite for digit Vx
void Chip8::op_Fx29()
{
	uint8_t Vx = (opcodes & 0x0F00U) >> 8U;
	uint8_t digit = regesters[Vx];
	index_regester = FONT_START_ADDRESS + (digit * 5);
}

void Chip8::op_Fx33()
{
	// TO DO	
}

void Chip8::op_Fx55()
{
	//To DO
}

void Chip8::op_Fx65()
{
	//To DO
}
