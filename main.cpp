#include<iostream>

#include "SDL2/SDL.h"

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
	
	screen.createWindow(500, 1000);
		

	c.load(argv[1]);	
	
	cout << SDLK_1 << endl;	

	return 0;
}
