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

	const int
		pxcount=renderTarget->w*renderTarget->h,//how many pixels
		raycount=pxcount,//number of rays, for now this is equal to number of pixels
		numthreads=threadPool::getMaxThreads(),
		bounces=0;//TODO: parameterize this
	color *raw=new color[pxcount];

	/*the order of this array is to be treated as it were [threadcount][bouncenum][pixels/threadcount]
	the reason for this is that this this means that sequential pixels and bounces will be adjacent to eachother in memory
	and will be split evenly between threads
	also sequential pixels will be adjacent in memory
	this layout is designed for optimizing caching, something which would not be achieved by the 2d array it's initialized as
	TODO: in the future, this will not be the case, as rays will be emitted from a light source
	will probably look like: drawScene.initRays(bounces,this)
	*/
	ray **rays=new ray*[numthreads];
	threadPool raypool(&camera::initRays,numthreads,true,this,rays,raycount,bounces);

	/*passing numthreads should be equivalent to passing -1
	however numthreads is passed anyways for consistency in case there is some bizarre edge case in which std::thread::hardware_concurrency() can change*/
	threadPool drawPool(&camera::renderLoop,numthreads,true,this,raw,raycount,pxcount,renderScene);
	//TODO: add one more function for rasterizing the ray array?
	threadPool postPool(&camera::doPost,numthreads,true,this,raw,renderTarget);

	delete[] raw;
	for(int i=0;i<numthreads;i++){
		delete[] rays[i];
	}
	delete[] rays;
}

/**
* @param **rays - an array pointers to ray arrays with size [threadcount]
* @param raycount - total number of rays in the scene
* @param bounces - how many times do the rays bounce
*
* initializes all members of rays to be arrays of size:
* ((bounces+1)*count)/numthreads, aka the number of bounces + the initial cast, split between each thread
* where each array is laid out as if it were a 2d array [bouncenum][pixels/threadcount]

* the first "row" is, for now, initialized from the camera's out rays
* TODO: should instead initialize from the scene's light sources
* TODO: does this need to be called every frame?  probably, but maybe not once rays come from lights instead
*/
void camera::initRays(int id,int numthreads,ray **rays,int raycount,int bounces){
	rays[id]=new ray[((bounces+1)*raycount)/numthreads];//1 is added to bounces to provide space for the initial set of rays

	//make each ray come out of the camera
	int reali;//the absolute ith ray
	double px,py;
	const int offset=(raycount*id)/numthreads;//size of each chunk divided by the bounces
	for(int i=0;i<raycount/numthreads;i++){
		reali=i+offset;//from (raycount*id)/numthreads to (raycount*(id+1))/numthreads -1
		//-1 to 1
		px=2*(double)(reali%target->w)/target->w-1;
		py=2*(double)(reali/target->w)/target->h-1;

		rays[id][i].from=this->position;
		rays[id][i].to=px*vec3d(this->axes.x)+py*vec3d(this->axes.y)+vec3d(this->axes.z);//TODO: determine if using +z or -z for right vs left hand coords
		rays[id][i].to.normalize();
	}
}

void camera::renderLoop(int id,int numthreads,color *raw,int raycount,int pxcount,sceneContainer *usingScene){
	//this will run a test pattern, TODO: this can be removed once ray bounces are correct
	#if 1
	const int start=(id*pxcount)/numthreads,stop=((id+1)*pxcount)/numthreads;
	const int patternWid=2;
	for(int i=start;i<stop;i++){
		raw[i].r=(float(id))/(numthreads-1);
		raw[i].g=(float(i%patternWid))/(patternWid-1);
		raw[i].b=0;
	}
	#else
	#endif
}
void camera::doPost(int id,int numthreads,color *raw,surface *target){
	//TODO: this assumes that the target and input have the same
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
