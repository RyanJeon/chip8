
#include <stdlib.h>
#include <chrono> 
#include <thread>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>
#include "time.h"

#include "chip8.h"

using namespace std;

unsigned char chip8_fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};


Chip8::Chip8(){
	init();
}

Chip8::~Chip8(){}


void Chip8::init() {
	cout << "Initializing the CPU" << endl;
	pc	= 0x200;	//512 
	opcode	= 0;
	ind	= 0;
	sp	= 0;
	printf("PC: %X\n", pc);

    // Clear the display
    for (int i = 0; i < 2048; ++i) {
        graphics[i] = 0;
    }

    // Clear the stack, keypad, and V registers
    for (int i = 0; i < 16; ++i) {
        stack[i]    = 0;
        //key[i]      = 0;
        v[i]        = 0;
    }

    // Clear memory
    for (int i = 0; i < 4096; ++i) {
        memory[i] = 0;
    }

    delayTimer = 0;
    soundTimer = 0;

    // Load font set into memory
    for (int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }

     srand (time(NULL));

}


void Chip8::load(const char *path, Screen *s) {
	screen = s;
	if(screen == NULL) {
		cerr << "Screen failed to initialize" << endl;
	}
	printf("Loading file\n");
	
	FILE* rom = fopen(path, "rb");
	if (rom == NULL) {
		cerr << "Failed to open the ROM file" << endl;
	}



	//Get size of the rom
	fseek(rom, 0, SEEK_END);    //All theway to the end
	romSize = ftell(rom); //Get the current position
	rewind(rom); //Reset the stream
	
	
	//Memory allocation to store opcodes from rom
	char* romBuffer = (char*) malloc(sizeof(char) * romSize);
	
	fread(romBuffer, sizeof(char), (size_t)romSize, rom);
	
	
	if((4096-512) > romSize){	
		for(int i = 0; i < romSize; i++){
			memory[512 + i] = (uint8_t)romBuffer[i];
		}
	}
	else{
		std::cerr << "ROM too large" << endl;
		exit(3);
	}
	
	fclose(rom);
	free(romBuffer);	

}

//Helper to execute one operation
void Chip8::execute(){
	//Opcodes are 2 bytes!
	opcode = memory[pc] << 8 | memory[pc + 1];
	
	stringstream stream;
	stream << std::hex << opcode;
	std::string result( stream.str() );
	cout << result << endl;
	
	//Check the first 4 bits 0 - F?
	switch(opcode & 0xF000 ){
		//0x0xxx
		case 0x0000:
			switch(opcode & 0x00FF){
				//00E0 : CLS clear screan
				case 0x00E0:
					for(int pixel = 0 ; pixel < 2048; ++pixel) {
						graphics[pixel] = 0;
					}
					draw = true;
					pc += 2;
					break;
				//Return from subroutine
				case 0x00EE:
					//pop rax
					//jmp rax
					sp--; //Decrease stack pointer
					pc = stack[sp]; //Return address
					pc += 2;
					// cout << "0x00EE" << endl;
					break;
			}
			break;
		//0x1xxx: jump to 0x0nnn
		case 0x1000:
			pc = opcode & 0x0FFF;
			break;
		//0x2xxx: subroutine at 0x0nnn
		case 0x2000:
			stack[sp] = pc; //return address
			sp++;
			pc = opcode & 0x0FFF;
			break;
		//0x3xkk
		case 0x3000:
			//If Vx == kk skip the next operation
			if(v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
				pc += 4;
			}
			else{
				pc += 2;
			}
			break;
		//0x4xkk 
		case 0x4000:
			//If Vx != kk skip the next operation
			if(v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
				pc += 4;	
			}
			else{
				pc += 2;
			}
			break;
		//0x5xy0: Vx == Vy then skip the instruction
		case 0x5000:
			if(v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0 >> 4)]){
				pc += 4;
			}
			else{
				pc += 2;
			}
			break;
		//0x6xkk: set Vx = kk
		case 0x6000:
			v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
			pc += 2;
			break;
		//0x7xkk: set Vx = Vx + kk
		case 0x7000:
			v[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
			pc += 2;
			cout << "7xkkk" << endl;
			break;
		//0x8xyn 
		case 0x8000:
			//Check the last 4 bits for instruction type
			switch( opcode & 0x000F ){
				//0x8xy0: Vx = Vy
				case 0x0000:
					v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
				//0x8xy1: Vx |= Vy
				case 0x0001:
					v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
				//0x8xy2: Vx &= Vy
				case 0x0002:
					v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
				//0x8xy3: Vx ^= Vy
				case 0x0003:
					v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];
					pc += 2; 
					break;
				//0x8xy4: Vx += Vy CHECK
				case 0x0004:
					//Carry over happens when the value goes over 0xFF (Each register
					//is 8-bit) so, if Vy + Vx > 0xFF, carry over happens.
					//   Vy > 0xFF - V
					
					v[(opcode & 0x0F00) >> 8] += v[(opcode & 0x00F0) >> 4];
					if(v[(opcode & 0x00F0) >> 4] > (0xFF - v[(opcode & 0x0F00) >> 8])){
						v[0xF] = 1;
					}
					else{
						v[0xF] = 0;
					}
					pc += 2;
					break;
				//0x8xy5: Vx -= Vy
				case 0x0005:
					//Check if borrow happens. Borrow happens when Vx -= Vy becomes 0
					//if Vy > Vx
					if(v[(opcode & 0x00F0) >> 4] > v[(opcode & 0x0F00) >> 8]){
						//Borrow
						v[0xF] = 0;
					}
					else{
						v[0xF] = 1;
					}
					v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
				//0x8xy6: Vx >> 1
				//Carry over if the least significant bit is 1. 
				case 0x0006:
					// if(v[(opcode & 0x0F00) >> 8] & 0x1) v[0xF] = 0x1;
					v[0xF] = v[(opcode & 0x0F00) >> 8] & 0x1;
					v[(opcode & 0x0F00) >> 8] >>= 1;
					pc += 2;
					//cout << "8xy6" << endl;
					break;
				//0x8xy7: Vx = Vy - Vx
				case 0x0007:	
					if(v[(opcode & 0x00F0) >> 4] < v[(opcode & 0x0F00) >> 8]){
						v[0xF] = 0;
					}
					else{
						v[0xF] = 1;
					}
					v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] - v[(opcode & 0x0F00) >> 8];	
					pc += 2;
					break;
				//0x8xyE: Vx << 1
				case 0x000E:
					//Check most significant bit, carry if it is 1.
					//if(v[(opcode & 0x0F00) >> 8] >> 7 == 0x1) v[0xF] = 0x1;
					v[0xF] = v[(opcode & 0x0F00) >> 8] >> 7;
					v[(opcode & 0x0F00) >> 8] <<= 1;
					pc += 2;
					//cout << "8xyE" << endl;
					break;
				default:
					cerr << "(ERROR) UNKNOWN OPCODE: " << result << endl;
					exit(3);
					break;
			}
			break;
		//0x9xy0: skip next instruction if Vx != Vy
		case 0x9000:
			if(v[(opcode & 0x0F00) >> 8] != v[(opcode & 0x00F0) >> 4] ){
				pc += 4;
			}
			else{
				pc += 2;
			}
			break;
		//0xAnnn : ind set to nnn
		case 0xA000:
			ind = opcode & 0x0FFF;
			pc += 2;
			break;
		//0xBnnn : jump to location nnn + V[0]
		case 0xB000:
			pc = (opcode & 0x0FFF) + v[0];
			break;
		//0xCxkk : Random byte & kk
		case 0xC000:
			//masking?
			v[(opcode * 0x0F00) >> 8] = (rand() % (0xFF + 1))  & (opcode & 0x00FF);
			pc += 2; 
			break;
		//0xDxyh x = x coordinate, y = y coordinate, h = height
		case 0xD000:
		{
			//X and Y coordinate for the pixels
			unsigned short x = v[(opcode & 0x0F00) >> 8];
			unsigned short y = v[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			
			v[0xF] = 0;
			unsigned short pixel;
			//Loop start for y line
			for(int yLine = 0; yLine < height; yLine++){
				//Width of 8
				pixel = memory[ind + yLine];
				//Read 8 bits of pixel data. 
				//0x80 = 1000 0000
				//pixel & 0x80 will give you the first bit of pixel on that row
				//to get the next pixel 0x80 >> 1 = 0100 0000 so on.... 
				for(int xLine = 0; xLine < 8; xLine++){
					if( ((0x80 >> xLine) & pixel) != 0 ) {
						if(graphics[(xLine + x + ((yLine + y) * 64))] == 1) v[0xF] = 1;
						//Give unique id to each pixel
						//chip8 uses 64x32 pixel display
						//every row has 64 pixels, to give 
						//unique IDs, (y + yLine) * 64
						graphics[xLine + x + ((y + yLine) * 64) ] ^= 1;
					// cout << "Print Graphic in pixel" << pixel << endl; 
					}
				}		
			}
			draw = true;
			pc += 2;
			// cout << "Draw Set Up Complete" << endl;
			break;
		}
		//0xExxx
		case 0xE000:
			cout << "E000" << endl;
			break;
		//0xFxxx
		case 0xF000:
			switch(opcode & 0x00FF){
				case 0x0007:
					v[(opcode & 0x0F00) >> 8] = delayTimer;
					pc += 2;
					break;
				case 0x000A:
				{
					bool keyPressed = false;
					
					cout << "FxxA" << endl;
				}
					break;
				case 0x0018:
					soundTimer = v[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;
				case 0x001E:
					if(ind + v[(opcode & 0x0F00) >> 8] > 0xFFF){
						v[0xF] = 1;
					}
					else{
						v[0xF] = 0;
					}
					ind += v[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;
				case 0x0029:
					ind = v[(opcode * 0x0F00) >> 8] * 0x5;
					pc += 2;
					break;
				case 0x0033:
	                		memory[ind]     = v[(opcode & 0x0F00) >> 8] / 100;
        	            		memory[ind + 1] = (v[(opcode & 0x0F00) >> 8] / 10) % 10;
                	    		memory[ind + 2] = v[(opcode & 0x0F00) >> 8] % 10;
                   	 		pc += 2;
					break;
				case 0x0055:
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i){
                        			memory[ind + i] = v[i];
					}
                    			// On the original interpreter, when the
                    			// operation is done, I = I + X + 1.
                    			ind += ((opcode & 0x0F00) >> 8) + 1;
                   			pc += 2;
					break;
				case 0x0065:
					for(int i = 0; i <= ((opcode & 0x0F00) >> 8); i++){
						v[i] = memory[ind + i];
					}
					ind += ((opcode & 0x0F00) >> 8) + 1;
					pc += 2;
			//		cout << "0xFx365 Executed" << endl;
					break;
				 default:
                    			printf ("Unknown opcode [0xF000]: 0x%X\n", opcode);
			}	
		break;
		default:
			printf("Unknown opcode: 0x%X\n", opcode);			
			exit(3);
	}
	cout << result << " executed" << endl;
}

//Emulate the full ROM
void Chip8::emulate(){
	cout << "Starting the emulation process.." << endl;
	cout << "ROM Size: " << romSize << endl;	
	
	uint32_t pixels[2048]; //temp pixel buffer
	uint32_t ctr = 0;
	//Execute all instructions in ROM	
	while(true){
		execute();
		
		//Run the event loop on the screen
		screen->eventLoop();
		cout << "Event Loop Done " << ctr << endl;
		//Do we need to draw? :)
		if (draw) {
			cout << "We Need To Draw: " << ctr << endl;
			//yes we do!
			cout << "DRAW START " << ctr << endl;
            		for (int i = 0; i < 2048; ++i) {
                		uint8_t pixel = graphics[i];
          	      		pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
            			// cout << pixels[i];
			}
			cout << "DRAWING TIME: " << ctr << endl;
			screen->renderSprite(pixels);	
			draw = false;
		}
		std::this_thread::sleep_for(std::chrono::microseconds(1200));
		cout << "One loop done " << ctr << endl;
		ctr++;
	}
}

