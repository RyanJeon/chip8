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
	win = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN );
	if (win == nullptr){
		cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
		SDL_Quit();
	}
}

void Screen::createRenderer(const int index){
	printf("Creating Renderer\n");

	ren = SDL_CreateRenderer(win, index, 0);
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
		SDL_Quit();
	}
	
	SDL_RenderSetLogicalSize(ren, 1024, 512);	
	
	printf("Creating Texture\n");
	texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
}

void Screen::renderSprite(uint32_t sprite[]){
	    if(ren == NULL) cerr << "Renderer not defined" << endl;
	    if(texture == NULL) cerr << "Texture not defined" << endl;

	    printf("Rendering Sprite\n");
	    // Update SDL texture
            SDL_UpdateTexture(texture, NULL, sprite, 64 * sizeof(Uint32));
            
	    // Clear screen and render
            SDL_RenderClear(ren);
            SDL_RenderCopy(ren, texture, NULL, NULL);
            SDL_RenderPresent(ren);
}

void Screen::eventLoop(){

	SDL_Event e;
	while(SDL_PollEvent(&e)){
		if (e.key.keysym.sym == SDLK_ESCAPE) exit(0);		

	}
	cout << "Event Loop End" << endl;
}


