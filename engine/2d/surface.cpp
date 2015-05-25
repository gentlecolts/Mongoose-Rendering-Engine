#include "surface.h"
using namespace MG;

void surface::initSurface(const window *win){
	w=win->w;
	h=win->h;
	tex=SDL_CreateTexture(win->r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,w,h);
	pixels=new uint32_t[w*h];
}
void surface::initSurface(int width,int height,const window *win){
	w=width;
	h=height;
	tex=SDL_CreateTexture(win->r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,w,h);
	pixels=new uint32_t[w*h];
}
void surface::update(){
	SDL_UpdateTexture(tex,NULL,pixels,w*sizeof(Uint32));
}
//*/

void window::initWindow(int width,int height,int flags){
	w=width;
	h=height;

	win=SDL_CreateWindow("",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,flags);
	r=SDL_CreateRenderer(win,-1,0);

	//tex=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,width,height);
	//pixels=new uint32_t[width*height];
	s.initSurface(this);
}

//void window::blit(surface &surf){}

/**note that this will remain unused as long as sdl2 is the active library
the reason being that it prevents blitting from taking effect and there's really no better way around this
*/
void window::update(){
	s.update();

	SDL_RenderClear(r);
	//SDL_RenderCopy(r,tex,NULL,NULL);
	SDL_RenderCopy(r,s.tex,NULL,NULL);
	SDL_RenderPresent(r);
}
