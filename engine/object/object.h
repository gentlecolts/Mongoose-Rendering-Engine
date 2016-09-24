#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED
/**
this file contains the definition for the abstract object class, which is the parent class to all 3d objects supported by this engine
*/

#include "../math/vec3d.h"

namespace MG{
	struct ray{
		vec3d from,to;
	};
	struct color{
		union{
			struct{float r,g,b;};
			float rgb[3];
		};
	};

	class engine;

	class obj{
	public:
		vec3d pos;
		virtual bool bounceRay(ray &r,uint32_t &color,double *d=0,vec3d *normal=0) = 0;

		virtual void firstUpdate(){}//the firstUpdate of every object will be called each frame before any object's update
		virtual void staticUpdate(){}//will be called every frame after firstUpdate
		virtual void threadedUpdate(){}//this will be called in parallel to all staticUpdate calls
		virtual void timedUpdate(){}//called at a setable interval unless the previous batch has not finished

		//this is called before any child constructors
		obj(engine* e);
	};
}

#endif // OBJECT_H_INCLUDED
