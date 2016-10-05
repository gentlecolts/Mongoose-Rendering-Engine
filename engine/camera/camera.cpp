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

	const int count=renderTarget->w*renderTarget->h;
	color *raw=new color[count];

	threadPool drawPool(&camera::renderLoop,-1,true,this,raw,renderTarget->w*renderTarget->h,renderScene);
	threadPool postPool(&camera::doPost,-1,true,this,raw,renderTarget);

	delete[] raw;
}
void camera::renderLoop(int id,int numthreads,color *raw,int count,sceneContainer *usingScene){
	const int start=(id*count)/numthreads,stop=((id+1)*count)/numthreads;
	const int patternWid=2;
	for(int i=start;i<stop;i++){
		raw[i].r=(float(id))/(numthreads-1);
		raw[i].g=(float(i%patternWid))/(patternWid-1);
		raw[i].b=0;
	}
}
void camera::doPost(int id,int numthreads,color *raw,surface *target){
	const int count=target->w*target->h;
	const int start=(id*count)/numthreads,stop=((id+1)*count)/numthreads;
	uint32_t r,g,b;
	for(int i=start;i<stop;i++){
		r=255*raw[i].r;
		g=255*raw[i].g;
		b=255*raw[i].b;
		target->pixels[i]=0xff000000|(r<<16)|(g<<8)|b;
	}
}
