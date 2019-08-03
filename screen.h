#include <stdint.h>
#include "SDL2/SDL.h"


class Screen {
	private:
		SDL_Window *win;	//Main window
		SDL_Renderer *ren;	//Window renderer
	
	public:
		Screen();
		~Screen();
		
		void init();		
		void createWindow(const int width, const int height);
		void createRenderer(const int index);
};	
