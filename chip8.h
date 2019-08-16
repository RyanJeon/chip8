#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include "screen.h"

class Chip8 {
	private:
		uint16_t pc;		//Program Counter
		uint16_t sp;		//Stack Pointer
		uint16_t ind;		//Index Register
		uint16_t opcode;	//Current Opcode		
		long romSize;	//ROM size	
		
		uint16_t delayTimer;
		uint16_t soundTimer;

		uint16_t v[16];		//Registers (v0 v1 ... vF)
		uint16_t stack[16];	//Stack
		uint16_t memory[4096];	//Memory (4kb)
		
		Screen *screen;		

	
	public:
		Chip8();
		~Chip8();
		
		bool draw;		//Draw flag
		uint8_t  graphics[64 * 32];              // Graphics buffer (64 x 32) display
		uint8_t  key[16];		
		void init();				//Initialize CPU
		void execute();
		void emulate();
		void load(const char *path, Screen *s);

		//Test Helper Methods READ ONLY
		uint16_t registerValue(uint16_t reg);

};

#endif
