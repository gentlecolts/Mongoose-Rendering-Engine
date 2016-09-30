#include "camera.h"
#include "../core/threadpool.h"
using namespace MG;

camera::camera(surface* renderTarget,sceneContainer* sceneToDraw){
	target=renderTarget;
	scene=sceneToDraw;

	position=vec3d(0,0,0);
}

void camera::render(surface *drawTo,sceneContainer *drawScene){
	/*TODO: this is just a simplified test to get things working, the final version will look more like the following
	-generate 2d array of light points
	--width=number of rays in scene, height=num bounces+1
	-(async) fill the first "row" with the inital rays from light sources
	-while threads are adding new points for the light sources(?), add first row's points to spacial partition as they are made available
	-loop for n bounces (will likely be stored in scene):
	--(async) fill the first "row" with the inital rays from light sources
	--while threads are adding new points for the light sources(?), add first row's points to spacial partition as they are made available
	-render final state of the ray points to camera as float[3]
	-apply post processing to the float[3], convert to ints, and apply to the given surface
	*/
	surface *renderTarget=drawTo?drawTo:target;//if a render target was supplied, use that instead of the stored one
	sceneContainer *renderScene=drawScene?drawScene:scene;//if a scene was supplied, use that instead of the stored one

	float target[renderTarget->w*renderTarget->h];


	threadPool drawPool(&camera::renderLoop,-1,true,this,target,renderScene);
	threadPool postPool(&camera::doPost()
}
void camera::renderLoop(int id,int numthreads,float *drawTarget,sceneContainer *usingScene){
}
