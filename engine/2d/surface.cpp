#include "surface.h"
using namespace MG;

void surface::initScreen(int width,int height,int flags){
	win=SDL_CreateWindow("",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,flags);
	r=SDL_CreateRenderer(win,-1,0);
	tex=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,width,height);
	pixels=new uint32_t[width*height];
	w=width;
	h=height;
}

void surface::update(){
	SDL_UpdateTexture(tex,NULL,pixels,w*sizeof(Uint32));
	SDL_RenderClear(r);
	SDL_RenderCopy(r,tex,NULL,NULL);
	SDL_RenderPresent(r);
}
