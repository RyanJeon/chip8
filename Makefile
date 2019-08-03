build:
	g++ main.cpp chip8.cpp

run:
	g++ main.cpp chip8.cpp -o chip8.out
	./chip8.out clock.ch8
