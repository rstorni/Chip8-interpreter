#include "chip-8.h"
#include "gameWindow.h"
#include <chrono>
#include <iostream>

int main(int argc, char** argv)
{	
	//If given an invalid argument count exit
	if(argc != 4)
	{
		return -1;
	}

	//load in arguments as variables
	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char const* fileName = argv[3];
	
	//Create Game window
	GameWindow Window(fileName, DISPLAY_WIDTH * videoScale, DISPLAY_HIGHT * videoScale, DISPLAY_WIDTH, DISPLAY_HIGHT);
	
	//create CHIP-8
	Chip8 Chip8_Emulator;
	Chip8_Emulator.loadROM(fileName);
	
	int videoPitch = sizeof(Chip8_Emulator.display[0]) * DISPLAY_WIDTH;

	//set condition variable to false and set now to be the time of the first cycle	
	auto lastcycle = std::chrono::high_resolution_clock::now();
	bool quit = false;
	
	//emulation loop
	while(!quit)
	{
		//if signaled to quit exit
		quit = Window.processInput(Chip8_Emulator.keypad);
	
		//get the current time and find the difference since we set the fist cycle	
		auto currentTime = sdt::chrono::high_resolution_clock::now();
		float delta_t = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastcycle).count();
		
		//if the change in time is greater that the delay set then run a cycle	
		if(delta_t > cycleDelay)
		{
			lastcycle = currentTime;
			
			Chip8_Emulator.cycle();
			
			window.Update(Chip8_Emulator.display, videoPitch);
		}		
	}	
	
	return 0; 
}
