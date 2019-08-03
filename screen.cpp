#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include "SDL2/SDL.h"

#include "screen.h"

using namespace std;


Screen::Screen(){
	init();
}
Screen::~Screen(){}

void Screen::init(){
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		cerr << "SDL_Init Error: " << SDL_GetError() << endl;
	}
}


void Screen::createWindow(const int width, const int height){	
	printf("Creating window\n");

	//win = SDL_CreateWindow("Chip8 Emulator", 0, 0, width, height, SDL_WINDOW_SHOWN);
	win = SDL_CreateWindow("Chip8 Emulator", 0, 0, width, height, SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (win == nullptr){
		cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
		SDL_Quit();
	}
}

void Screen::createRenderer(const int index){
	printf("Creating Renderer\n");

	ren = SDL_CreateRenderer(win, index, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
		SDL_Quit();
	}

}

