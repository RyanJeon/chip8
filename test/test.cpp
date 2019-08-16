#include<iostream>

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


//0x0xxx test
void TEST_0(){
	
	Chip8 chip = initCPU("Test_0");	
	chip.execute();
}

int main(int argc, char **argv){

	TEST_0();

	return 0;
}
