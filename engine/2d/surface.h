#ifndef SURFACE_H_INCLUDED
#define SURFACE_H_INCLUDED
#include <SDL2/SDL.h>
#include <stdint.h>
namespace MG{
	//*
	struct window;
	struct surface{
		SDL_Texture *tex=0;
		uint32_t *pixels=0;
		int w,h;

		void initSurface(const window *win);
		void initSurface(int width,int height,const window *win);
		void update();
	};
	//*/

	struct window{
		SDL_Window *win=0;
		SDL_Renderer *r=0;
		//*
		surface s;
		/*/
		SDL_Texture *tex;
		uint32_t *pixels;
		//*/

		int w,h;

		void initWindow(int width,int height,int flags=SDL_WINDOW_SHOWN);
		//void blit(surface &surf);
		void update();
	};
}

#endif // SURFACE_H_INCLUDED
