/**
this file implements all functionality of the engine class that pertain to handling and reporting events
*/

#include "core.h"
#include "SDL2/SDL.h"
using namespace MG;

//TODO: consider adding a sleep to this?
void engine::threadedPolling(){
	while(1){pollEvents();}
}

bool engine::setEventAsync(bool b){
	if(b!=isEventSync){//no need to do anything if the state of threaded or not is unchanged
		return true;
	}

	//TODO: revise this so that, instead of deleting the thread, it terminates itself when the bool flips, deleting threads can be unsafe
	if(b){//create event thread
		evtThread=new std::thread(&engine::threadedPolling,this);
		evtThread->detach();
	}else{//terminate event thread if it exists
		delete evtThread;
	}

	//if we've reached this point, then everything went well and we can return
	isEventSync=!b;
	return true;//everything went well
}

void engine::quit(){
	SDL_Quit();
	exit(0);
}

SDL_Event e;
void engine::pollEvents(){
#define loopall(x) for(int i=0;i<events.size();i++){events[i]->x;} break;
	while(SDL_PollEvent(&e)){
		switch(e.type){
		case SDL_QUIT:
			loopall(quit())
		case SDL_KEYDOWN:
			loopall(keyEvent(true,e.key.keysym.sym))
		case SDL_KEYUP:
			loopall(keyEvent(false,e.key.keysym.sym))
		case SDL_MOUSEMOTION:
			loopall(mouseMoved(e.motion.x,e.motion.y,e.motion.xrel,e.motion.yrel))
		case SDL_MOUSEBUTTONDOWN:
			loopall(mouseButton(true,e.button.button))
		case SDL_MOUSEBUTTONUP:
			loopall(mouseButton(false,e.button.button))
		}
	}
#undef loopall
}

int engine::registerEvent(event *evt){
	events.push_back(evt);
	return events.size()-1;
}
void engine::removeEvent(int index){
	events.erase(events.begin()+index);
}
