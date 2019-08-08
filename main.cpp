#include<iostream>

#include "chip8.h"
#include "screen.h"
#include "SDL2/SDL.h"

using namespace std;



int main(int argc, char **argv){
	
	cout << "Running Chip8 Emulator" << endl;

	Chip8 c = Chip8();
	Screen screen = Screen();

	int width = 1024;
	int height = 512;
	
	screen.createWindow(width, height);
	screen.createRenderer(-1);		
	
	//Load ROM in the chip8 CPU
	c.load(argv[1], &screen);	
	
	//Start the emulation
	c.emulate();
		

	return 0;
}
