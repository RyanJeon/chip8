
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
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
	long romSize = ftell(rom); //Get the current position
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

void Chip8::emulateCycle(){
	
	opcode = memory[pc] << 8 | memory[pc + 1];


}



