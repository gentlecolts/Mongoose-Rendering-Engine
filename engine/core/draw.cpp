/**
this file implements all functionality of the engine class that pertain to rendering the scene
*/
#include "core.h"
#include <ctime>
using namespace MG;

/*note that in the future this may do a lot more than just a simple time check
especially if engine.update() ever becomes async
*/
bool engine::isTimeToUpdate(){
	return (std::clock()-updateTimestamp)*targetFPS>CLOCKS_PER_SEC;//delta_clock/clockpersec>1/fps
}

#define FIRST_JOIN_FORK 1

void engine::update(){
	/*TODO: consider re-organizing this so that last frame's gpu tasks happen first, then this frame's cpu tasks
	reson being that...
	if user is manually calling update:
		after calling update, all non-render specific data for the current frame would be ready to go for them after update is called,
		allowing for any further adjustments to take place before rendering is performed
	for engine's own main loop:
		might make wait timing better, since gpu and cpu tasks are completely independent parts

	also on the TODO: make engine::isTimeToUpdate() return true when both the gpu task is done AND the timer says it's time to update, in case the gpu side takes too long

	also TODO: regardless of any of the above, maybe split processing and rendering into separate function calls
		this'd help organiztion and make it easier for server code to completely skip the render task
	*/
	updateTimestamp=std::clock();//done at the beginning of update so that processing time is included in the dt

	///check events if they're not running in a separate thread
	if(isEventSync){
		pollEvents();
	}

	//TODO: gui updates go here?
	//TODO: determine if gui update should be separate from regular update, currently leaning towards yes

	///step physics(?)
	///TODO
	///render scene(?)
	///TODO
	///update window
	//firstupdate
	for(int i=0;i<scene.objects.size();i++){
		scene.objects[i]->firstUpdate();
	}
	//staticUpdate and forkedUpdate
	threadPool pool(&engine::updateTasks,-1,true,this);
	#if FIRST_JOIN_FORK
	for(int i=0;i<scene.objects.size();i++){
		scene.objects[i]->joinedUpdate();
	}
	#endif

	//draw the scene
	mainCamera.render();

	showHud=false;

	//win.update();
	//if the scene or hud are invisible, no reason to waste effort rendering to them
	if(showScene){render.update();}
	if(showHud){hud.update();}

	//clear the current window, then copy the scene and hud layers to it
	SDL_RenderClear(win.r);
	if(showScene){SDL_RenderCopy(win.r,render.tex,NULL,NULL);}
	if(showHud){SDL_RenderCopy(win.r,hud.tex,NULL,NULL);}
	SDL_RenderPresent(win.r);
}

void engine::initWindow(int width,int height,int flags){
	win.initWindow(width,height,flags);
	hud.initSurface(&win);
	render.initSurface(&win);
}

void engine::setTitle(const char* title){
	SDL_SetWindowTitle(win.win,title);
}

void engine::updateTasks(int id,int numThreads){
	#if !FIRST_JOIN_FORK
	if(numThreads==1){//if there is only one thread, then we need to handle both update calls in this thread
		for(int i=0;i<scene.objects.size();i++){
			scene.objects[i]->staticUpdate();
			scene.objects[i]->forkedUpdate();
		}
	//}else if(numThreads>objects.size()){//if we for some reason have more threads than objects
	}else{
		if(id){//id>0
			/*note that if the number of async threads is greater than the number of objects, then some instances of start and stop will have the same value and not loop
			thus there will be no duplicate calls anyways TODO: make sure to test this just to be safe*/
			const int//remember, one of these threads is running the static updates
				start=(scene.objects.size()*(id-1))/(numThreads-1),
				stop=(scene.objects.size()*id)/(numThreads-1);
			for(int i=start;i<stop;i++){
				scene.objects[i]->forkedUpdate();
			}
		}else{
			for(int i=0;i<scene.objects.size();i++){
				scene.objects[i]->joinedUpdate();
			}
		}
	}
	#else
    /*note that if the number of async threads is greater than the number of objects, then some instances of start and stop will have the same value and not loop
    thus there will be no duplicate calls anyways*/
	const int
        start=(scene.objects.size()*(id-1))/(numThreads-1),
        stop=(scene.objects.size()*id)/(numThreads-1);
    for(int i=start;i<stop;i++){
        scene.objects[i]->forkedUpdate();
    }
    #endif
}
