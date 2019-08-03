

#include <stdint.h>

class Chip8 {
	private:
		uint16_t pc;		//Program Counter
		uint16_t sp;		//Stack Pointer
		uint16_t ind;		//Index Register
		uint16_t opcode;	//Current Opcode		

		uint16_t v[16];		//Registers (v0 v1 ... vF)
		uint16_t stack[16];	//Stack
		uint16_t memory[4096];	//Memory (4kb)
			
	
	public:
		Chip8();
		~Chip8();
		
		void init();				//Initialize CPU
		void emulateCycle();			
		void load(const char *path);
};
