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

int main(int argc,char** argv){
	MG::engine e;
 	//e.setEventHandler(&checkClose);//moved this to be the default event handler
 	e.initWindow(1280,720);
	e.setTitle("Mongoose Rendering Engine Demo");

	e.setEventAsync(true);
	//e.setEventAsync(false);

	while(1){
		e.update();
	}
	return 0;
}
