#ifndef SURFACE_H_INCLUDED
#define SURFACE_H_INCLUDED
#include <stdint.h>
#include <SDL2/SDL.h>

namespace MG{
	struct surface{
		SDL_Window *win;
		SDL_Renderer *r;
		SDL_Texture *tex;
		uint32_t *pixels;
		int w,h;

		void initScreen(int width,int height,int flags=SDL_WINDOW_SHOWN);
		void update();
	};
}

#endif // SURFACE_H_INCLUDED
