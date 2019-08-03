#include<iostream>

#include "chip8.h"
#include "SDL2/SDL.h"


using namespace std;



int main(int argc, char **argv){
	
	cout << "Running Chip8 Emulator" << endl;
	//Graphic
	//setupGraphics
	//SetUpInput

	Chip8 c = Chip8();
	
	c.load(argv[1]);	

	return 0;
}
