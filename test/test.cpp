#include<iostream>
#include<assert.h>
#include "../chip8.h"
#include "../screen.h"

using namespace std;

//Opcode Tests
string testPath = "./";

Chip8 initCPU(char path[]){
	Chip8 c = Chip8();
	Screen screen = Screen();

	int width = 1024;
	int height = 512;

	c.load(path, &screen);

	return c;
}


//MOVE ADD TEST 0x6xkk & 0x7xkk
void TEST_0(){
	
	//Test_0 : 6122 7147
	Chip8 chip = initCPU("Test_0");	
	
	//6122 : MOV v1, 0x22 <- 34 in decimal
	chip.execute();
	assert(chip.registerValue(1) == 0x22);

	//7147 : ADD v1, 0x47 <- 71 in decimal 
	chip.execute();
	//total should be 105 in decimal, or 0x69
	assert(chip.registerValue(1) == 0x69);

}

int main(int argc, char **argv){

	TEST_0();

	return 0;
}
