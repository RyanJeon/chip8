#include<iostream>

#include "chip8.h"
#include "screen.h"
#include "SDL2/SDL.h"

using namespace std;



int main(int argc, char **argv){
	
	cout << "Running Chip8 Emulator" << endl;

	Chip8 c = Chip8();
	Screen screen = Screen();

	int width = 500;
	int height = 1000;
	
	//screen.createWindow(500, 1000);
		
	
	//Load ROM in the chip8 CPU
	c.load(argv[1]);	
	
	//Start the emulation
	c.emulate();
		

	return 0;
}