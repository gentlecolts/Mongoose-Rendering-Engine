#include "camera.h"
#include "../core/threadpool.h"
using namespace MG;

camera::camera(surface* renderTarget,sceneContainer* sceneToDraw){
	target=renderTarget;
	scene=sceneToDraw;

	position=vec3d(0,0,0);

	//axes.y=double(renderTarget->
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
		bounces=1;//TODO: parameterize this
	//this and rays are allocated on heap because they may be too big for the stack
	color *raw=new color[pxcount];

	/*the order of this array is to be treated as it were [threadcount][bouncenum][raycount/threadcount]
	the reason for this is that this this means that sequential rays and bounces will be adjacent to eachother in memory
	and will be split evenly between threads
	this layout is designed for optimizing caching
	*/
	ray **rays=new ray*[numthreads];
	threadPool raypool(&camera::initRays,numthreads,true,this,rays,raycount,bounces);

	/*passing numthreads should be equivalent to passing -1
	however numthreads is passed anyways for consistency in case there is some bizarre edge case in which std::thread::hardware_concurrency() can change
	*/
	threadPool drawPool(&camera::renderLoop,numthreads,true,this,rays,raw,raycount,renderTarget,bounces,renderScene);
	//TODO: add one more function for rasterizing the ray array?
	threadPool postPool(&camera::doPost,numthreads,true,this,raw,renderTarget);

	delete[] raw;
	for(int i=0;i<numthreads;i++){
		delete[] rays[i];
	}
	delete[] rays;

	printf("render done\n");
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
	const int offset=(raycount*id)/numthreads;//size of each chunk divided by the bounces
	for(int i=0;i<raycount/numthreads;i++){
		const int reali=i+offset;//from (raycount*id)/numthreads to (raycount*(id+1))/numthreads -1
		//-1 to 1
		//saving the double cast to the very latest it can be (marginally) reduces rounding error
		double
			px=(double)(2*(reali%target->w)-target->w)/target->w,
			py=(double)(2*(reali/target->w)-target->h)/target->h;

		rays[id][i].from=this->position;
		rays[id][i].dir=px*vec3d(this->axes.x)+py*vec3d(this->axes.y)+vec3d(this->axes.z);//TODO: determine if using +z or -z for right vs left hand coords
		rays[id][i].dir.normalize();
	}
}

void camera::renderLoop(int id,int numthreads,ray **rays,color *raw,int raycount,surface *renderTarget,int bounces,sceneContainer *usingScene){
	//this will run a test pattern, TODO: this can be removed once ray bounces are correct
	#if 0
	const int start=(id*pxcount)/numthreads,stop=((id+1)*pxcount)/numthreads;
	const int patternWid=2;
	for(int i=start;i<stop;i++){
		raw[i].r=(float(id))/(numthreads-1);
		raw[i].g=(float(i%patternWid))/(patternWid-1);
		raw[i].b=0;
	}
	#else
	//compute all rays in the scene
	const int rcount=raycount/numthreads;
	for(int i=0;i<bounces;i++){
		usingScene->render(rcount,&(rays[id][i*rcount]),&(rays[id][(i+1)*rcount]));
	}

	const vec3d xcomp(axes.x),ycomp(axes.y),norm(axes.z),origin=position+norm;

	//convert rays to raw image
	//for(int i=0;i<((bounces+1)*raycount)/numthreads;i++){//this loop is for actually using all rays
	for(int i=rcount;i<2*rcount;i++){//rays are from camera, only the first bounce matters, TODO: remove and replace with above line when possible
		//project intersection point onto screen
		const ray &r=rays[id][i];

		const vec3d pointToCam=position-r.from;

		const double t=norm.dot(origin-r.from)/norm.dot(pointToCam);
		const vec3d point=pointToCam*t+r.from-origin;

		//convert to array index and setgit
		//TODO: get this mapping just right
		const int
			x=(renderTarget->w-1)*(point.dot(xcomp)+1)/2,
			y=(renderTarget->h-1)*(1-point.dot(ycomp))/2;//need to flip the y
		//printf("got ray(%i) with coord: (%i, %i), width is (%i, %i)\n",r.hit,x,y,renderTarget->w,renderTarget->h);
		if(x>=0 || y>=0){
			//printf("got a point with positive value\n");
		}
		if(
			(x>=0) & (x<renderTarget->w) &
			(y>=0) & (y<renderTarget->h)
		){
			//printf("passed test, projecting\n");
			//*
			raw[x+renderTarget->w*y]=r.c;
			/*/
			//color is from normal
			//double dot=std::abs(r.normal.dot(r.dir));
			//double dot=(norm.dot(pointToCam)/abs(pointToCam)+1)/2;
			//double dot=(norm.dot(r.normal)/abs(r.normal)+1)/2;
			//raw[x+renderTarget->w*y]=color(dot,dot,dot);
			raw[x+renderTarget->w*y]=color(r.normal.xyz);
			//*/
		}
	}

	/*debugging
	auto drawline=[&](vec3d v0,vec3d v1,color c){
		const int
			x0=(renderTarget->w-1)*(v0.dot(xcomp)+1)/2,
			y0=(renderTarget->h-1)*(1-v0.dot(ycomp))/2,
			x1=(renderTarget->w-1)*(v1.dot(xcomp)+1)/2,
			y1=(renderTarget->h-1)*(1-v1.dot(ycomp))/2,
			dx=x1-x0,dy=y1-y0,
			n=std::max(std::abs(dx),std::abs(dy));
		for(int i=0;i<=n;i++){
			const int
				x=x0+i*dx/n,
				y=y0+i*dy/n;
			raw[x+renderTarget->w*y]=c;
		}
	};

	drawline(vec3d(0,0,0),vec3d(1,0,0),color(1,0,0));
	drawline(vec3d(0,0,0),vec3d(0,1,0),color(0,1,0));
	drawline(vec3d(0,0,0),vec3d(0,0,1),color(0,0,1));
	//*/
	#endif
}
void camera::doPost(int id,int numthreads,color *raw,surface *target){
	//TODO: this assumes that the target and input have the same
	const int count=target->w*target->h;
	const int start=(id*count)/numthreads,stop=((id+1)*count)/numthreads;
	for(int i=start;i<stop;i++){
		uint32_t
			r=255*raw[i].r,
			g=255*raw[i].g,
			b=255*raw[i].b;
		target->pixels[i]=0xff000000|((r&0xff)<<16)|((g&0xff)<<8)|(b&0xff);
	}
}

void camera::rotateDeg(double angle,const vec3d& axis){
}
void camera::rotateRad(double angle,const vec3d& axis){
}

void camera::lookAt(const vec3d &point,const vec3d& up){
	vec3d z=point-position;
	z.normalize();

	vec3d x=z.cross(up.getNormalized());
	vec3d y=x.cross(z);

	for(int i=0;i<3;i++){
		axes.x[i]=x.xyz[i];
		axes.y[i]=y.xyz[i];
		axes.z[i]=z.xyz[i];
	}
}
