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
	//firstupdate
	for(int i=0;i<objects.size();i++){
		objects[i]->firstUpdate();
	}
	//staticUpdate and threadedUpdate
	threadPool pool(&engine::updateTasks,-1,true,this);

	//win.update();
	//if the scene or hud are invisible, no reason to waste effort rendering to them
	if(showScene){scene.update();}
	if(showHud){hud.update();}

	//clear the current window, then copy the scene and hud layers to it
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

void engine::updateTasks(int id,int numThreads){
	if(numThreads==1){//if there is only one thread, then we need to handle both update calls in this thread
		for(int i=0;i<objects.size();i++){
			objects[i]->staticUpdate();
			objects[i]->threadedUpdate();
		}
	//}else if(numThreads>objects.size()){//if we for some reason have more threads than objects
	}else{
		if(id){//id>0
			/*note that if the number of async threads is greater than the number of objects, then some instances of start and stop will have the same value and not loop
			thus there will be no duplicate calls anyways TODO: make sure to test this just to be safe*/
			const int//remember, one of these threads is running the static updates
				start=(objects.size()*(id-1))/(numThreads-1),
				stop=(objects.size()*id)/(numThreads-1);
			for(int i=start;i<stop;i++){
				objects[i]->threadedUpdate();
			}
		}else{
			for(int i=0;i<objects.size();i++){
				objects[i]->staticUpdate();
			}
		}
	}
}
