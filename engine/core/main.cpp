/**
this file implements all functionality of the engine class that pertain to core functionality, such as initialization
*/

#include "core.h"
using namespace MG;

engine::engine():event(0){
	init();
	registerEvent(this);
	//index and parent for event superclass
	index=0;
	parent=this;
}
engine::~engine(){
}

void checkClose(MG::event *evt){
	/*
	if(SDL_PollEvent(evt)){
		switch(evt->type){
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
		}
	}
	*/
}
void engine::init(){
}
