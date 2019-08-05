#include <stdint.h>
#include "SDL2/SDL.h"

#ifndef screen_h
#define screen_h
class Screen {
	private:
		SDL_Window *win;	//Main window
		SDL_Renderer *ren;	//Window renderer
		SDL_Texture *texture;	//Screen texture	
	public:
		Screen();
		~Screen();
		
		void init();		
		void createWindow(const int width, const int height);
		void createRenderer(const int index);
		void renderSprite(uint32_t sprite[]);
};
#endif
