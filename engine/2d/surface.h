#ifndef SURFACE_H_INCLUDED
#define SURFACE_H_INCLUDED
#include <stdint.h>
#include <SDL2/SDL.h>

namespace MG{
	/*
	struct surface{
		SDL_Texture *tex;
		uint32_t *pixels;

		void initSurface(int width,int height);
		void update();
	};
	//*/

	struct window{
		SDL_Window *win;
		SDL_Renderer *r;
		/*
		surface s;
		/*/
		SDL_Texture *tex;
		uint32_t *pixels;
		//*/

		int w,h;

		void initWindow(int width,int height,int flags=SDL_WINDOW_SHOWN);
		//void blit(surface &s);
		void update();
	};
}

#endif // SURFACE_H_INCLUDED
