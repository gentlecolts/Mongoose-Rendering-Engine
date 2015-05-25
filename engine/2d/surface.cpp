#include "surface.h"
using namespace MG;

/*
void surface::initSurface(int width,int height){
}
void surface::update(){
	SDL_UpdateTexture(tex,NULL,pixels,w*sizeof(Uint32));
}
//*/

void window::initWindow(int width,int height,int flags){
	win=SDL_CreateWindow("",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,flags);
	r=SDL_CreateRenderer(win,-1,0);

	///TODO: figure out how this whole works so that surfaces can be made independent of the window object
	tex=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,width,height);
	pixels=new uint32_t[width*height];
	w=width;
	h=height;
}

void window::update(){
	SDL_UpdateTexture(tex,NULL,pixels,w*sizeof(Uint32));///This is to be removed and put in the surface's version of update
	SDL_RenderClear(r);
	SDL_RenderCopy(r,tex,NULL,NULL);
	//SDL_RenderCopy(r,s.tex,NULL,NULL);
	SDL_RenderPresent(r);
}
