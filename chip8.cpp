
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>
#include "time.h"

#include "chip8.h"

using namespace std;

Chip8::Chip8(){
	init();
}

Chip8::~Chip8(){}


void Chip8::init() {
	pc	= 0x200;	//512 
	opcode	= 0;
	ind	= 0;
	sp	= 0;
}


void Chip8::load(const char *path) {
	
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
	
	
	
	for(int i = 0; i < romSize; i++){
		memory[512 + i] = (uint8_t)romBuffer[i];
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
	// cout << result << endl;
	
	//Check the first 4 bits 0 - F?
	switch(opcode & 0xF000 ){
		//0x0xxx
		case 0x0000:
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
			if(v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0)]){
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
				//0x8xy4: Vx += Vy
				case 0x0004:
					//Carry over happens when the value goes over 0xFF (Each register
					//is 8-bit) so, if Vy + Vx > 0xFF, carry over happens.
					//   Vy > 0xFF - Vx
					if(v[(opcode & 0x00F0) >> 4] > (0xFF - v[(opcode & 0x0F00) >> 8])){
						v[0xF] = 1;
					}
					else{
						v[0xF] = 0;
					}
					v[(opcode & 0x0F00) >> 8] += v[(opcode & 0x00F0) >> 4];
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
					if(v[(opcode & 0x0F00) >> 8] & 0x1) v[0xF] = 0x1;
					v[(opcode & 0x0F00) >> 8] >>= 1;
					pc += 2;
					break;
				//0x8xy7: Vx = Vy - Vx
				case 0x0007:	
					if(v[(opcode & 0x00F0) >> 4] > v[(opcode & 0x0F00) >> 8]){
						v[0xF] = 1;
					}
					else{
						v[0xF] = 0;
					}
					break;
				//0x8xyE: Vx << 1
				case 0x000E:
					//Check most significant bit, carry if it is 1.
					if(v[(opcode & 0x0F00) >> 8] >> 7 == 0x1) v[0xF] = 0x1;
					v[(opcode & 0x0F00) >> 8] <<= 1;
					break;
				default:
					cerr << "(ERROR) UNKNOWN OPCODE: " << result << endl;
					break;
			}
			break;
		//0x9xxx
		case 0x9000:
			break;
		//0xAxxx
		case 0xA000:
			break;
		//0xBxxx
		case 0xB000:
			break;
		//0xCxxx
		case 0xC000:
			break;
		//0xDxyh x = x coordinate, y = y coordinate, h = height
		case 0xD000:
		{
			//X and Y coordinate for the pixels
			uint16_t x = v[(opcode & 0x0F00 >> 8)];
			uint16_t y = v[(opcode & 0x00F0 >> 4)];
			uint16_t height = opcode & 0x000F;
			

			uint16_t pixel;
			//Loop start for y line
			for(int yLine = 0; yLine < height; yLine++){
				//Width of 8
				pixel = memory[ind + yLine]; 
				for(int xLine = 0; x < 8; xLine++){
					cout << "Print Graphic in pixel" << pixel << endl; 
				}		
			}
			break;
		}
		//0xExxx
		case 0xE000:
			break;
		//0xFxxx
		case 0xF000:
			cout << result << endl;
			break;
	}
	
	//Increment program counter
	pc += 2;
}

//Emulate the full ROM
void Chip8::emulate(){
	cout << "Starting the emulation process.." << endl;
	cout << "ROM Size: " << romSize << endl;	
	
	//Execute all instructions in ROM	
	while(pc - 0x200 < romSize){
		execute();
	}
}

