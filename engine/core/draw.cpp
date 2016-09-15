/**
this file implements all functionality of the engine class that pertain to rendering the scene
*/
#include "core.h"
using namespace MG;

void engine::update(){
	///check events if they're not running in a separate thread
	if(isEventSync){
		pollEvents();
	}
	///step physics(?)
	///TODO
	///render scene(?)
	///TODO
	///update window
	//win.update();
	if(showScene){scene.update();}
	if(showHud){hud.update();}

	SDL_RenderClear(win.r);
	if(showScene){SDL_RenderCopy(win.r,scene.tex,NULL,NULL);}
	if(showHud){SDL_RenderCopy(win.r,hud.tex,NULL,NULL);}
	SDL_RenderPresent(win.r);
}

void engine::initWindow(int width,int height,int flags){
	win.initWindow(width,height,flags);
	hud.initSurface(&win);
	scene.initSurface(&win);
}

void engine::setTitle(const char* title){
	SDL_SetWindowTitle(win.win,title);
}
