#ifndef CHIP8_H
#define CHIP8_H
#include <cstdint>
#include <random>

//CONSTANTS
const unsigned int MEMORY_SIZE = 4096;
const unsigned int NUM_REGESTERS = 16;
const unsigned int STACK_SIZE = 16;
const unsigned int NUM_KEYS = 16;
const unsigned int DISPLAY_HIGHT = 32;
const unsigned int DISPLAY_WIDTH = 64;

class Chip8{
public:
	
	Chip8(); //constructor
	void loadROM(char const* filename);
	void cycle();
	
	uint8_t keypad[NUM_KEYS];
	uint32_t display[DISPLAY_HIGHT * DISPLAY_WIDTH];

private:
	
	void table0();

	void table8();

	void tableE();

	void tableF();	
	
	//Does Nothing
	void op_null();
	
	//jump to machine code routine at nnn (obsolite)
	void op_0nnn();
	
	//CLS clears the display
	void op_00E0();	

	//RET returns from a subroutine
	void op_00EE();
	
	//JP jump to location nnn
	void op_1nnn();
	
	//CALL call subroutine at nnn
	void op_2nnn();

	//SE Vx skip the next instruction if Vx == kk
	void op_3xkk();

	//SNE Vx skip next instruction if Vx != kk
	void op_4xkk();

	//SE Vx, Vy skip the next instruction if Vx == Vy
	void op_5xy0();

	//LD Vx load the value kk into regester Vx
	void op_6xkk();
	
	//ADD Vx adds the value of kk to the val at regester Vx and then stores it at Vx
	void op_7xkk(); 

	//LD Vx, Vy Load value of regester Vy into regester Vx
	void op_8xy0();
	
	//OR Vx, Vy Set Vx equal to the result from preforming OR on Vx Vy (Vx | Vy)
	void op_8xy1();
	
	//AND Vx, Vy set Vx equal to the result from preforming AND on Vx Vy (Vx & Vy)
	void op_8xy2();

	//XOR Vx, Vy set Vx equal to the result from preforming XOR on Vx Vy (Vx ^ Vy)
	void op_8xy3();

	//ADD Vx, Vy set Vx equal to the sum of Vx and Vy also set the VF regester to carry
	void op_8xy4();

	//SUB Vx, Vy set Vx equal to the difference of Vx and Vy set VF to NOT borrow
	void op_8xy5();
	
	//SHR Vx {, Vy} set Vx to Vx SHR 1
	void op_8xy6();

	//SUBN Vx, Vy set Vx equal to the difference of Vy and Vx set VF to NOT borrow
	void op_8xy7();
	
	//SHL Vx {, Vy} set Vx equal to Vx SHL 1
	void op_8xyE();
	
	//SNE Vx,Vy Skip next instruction if Vx != Vy
	void op_9xy0();

	//LD I, addr Load the value nnn into regester I
	void op_Annn();
	
	//JP V0, addr Jump to the location of nnn + V0
	void op_Bnnn();

	//RND Vx, set Vx to the result from a random byte AND kk
	void op_Cxkk();

	//DRW Vx, Vy display n-byte sprite starting at memory location I at (Vx,Vy),Set VF = Collison
	void op_Dxyn();
	
	//SKP Vx Skip the next instruction if the key with value of Vx is pressed
	void op_Ex9E();

	//SKNP Vx Skip the next instruction if the key with value Vx is not pressed
	void op_ExA1();

	//LD Vx, DT Set Vx to the value of the delay timer
	void op_Fx07();

	//LD Vx, K Wait for a key to be pressed and store the value of the key in Vx
	void op_Fx0A();

	//LD DT,Vx Set the delay timer equal to Vx
	void op_Fx15();

	//LD St, Vx Set the sound timer to equal Vx
	void op_Fx18();

	//ADD I, Vx Set I equal to the sum of I and Vx
	void op_Fx1E();
	
	//LD F, Vx Set I equal to the location of sprite for digit Vx
	void op_Fx29();
	
	//Ld B, Vx Store BCD reprsentation of Vx in Memory locations I, I+1 and I+2
	void op_Fx33();

	//LD [I], Vx Store regesters V0 - Vx in memory starting at location I
	void op_Fx55();
	
	//LD Vx, [I] Read regesters V0 - Vx from memory starting at location I
	void op_Fx65();


	uint8_t memory[MEMORY_SIZE];
	uint8_t regesters[NUM_REGESTERS];	
	uint16_t pc;
	uint16_t index_regester;
	uint16_t stack[STACK_SIZE];
	uint8_t stack_pointer;
	uint8_t sound_timer;
	uint8_t delay_timer;
	uint16_t opcodes;


	//define random generator
	std::default_random_engine rng;
	std::uniform_int_distribution<uint8_t> random_byte;

	//define tables
	typedef void(Chip8::*Chip8Function)();
	
	Chip8Function FunctionTable[0xF + 1];
	Chip8Function Table0[0xE + 1];
	Chip8Function Table8[0xE + 1];
	Chip8Function TableE[0xE + 1];
	Chip8Function TableF[0x65 + 1];	
};


#endif
