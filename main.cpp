/**
TODO:
use SDL2 -try to hide it as much as possible, any external libraries should be treated as though they could be dropped any time
use c++11 threading

look into BVH
*/
#include <SDL2/SDL.h>
#include "engine/engine.h"
#include "engine/math/quat.h"
#include "engine/math/vec3d.h"
#include <cstdlib>
using namespace std;

SDL_Event evt;
void checkClose(){
	if(SDL_PollEvent(&evt)){
		switch(evt.type){
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
		}
	}
}

int main(int argc,char** argv){
	MG::engine e;

	e.screen.initScreen(1280,720);
	while(1){
		e.screen.update();
		checkClose();
	}
	return 0;
}
