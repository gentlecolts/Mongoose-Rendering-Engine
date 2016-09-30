#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED
/**
this file contains the definition for the abstract object class, which is the parent class to all 3d objects supported by this engine
*/

#include "../math/vec3d.h"
#include "meta.h"
#include <stdint.h>
#include <fstream>

namespace MG{
	struct color{
		union{
			struct{float r,g,b;};
			float rgb[3];
		};
	};

	struct ray{
		vec3d from,to;
		color c;
	};

	class engine;

	class obj{
		std::fstream file;
	public:
		vec3d pos;
		virtual bool bounceRay(ray &r,uint32_t &color,double *d=0,vec3d *normal=0) = 0;

		virtual void firstUpdate(){}//the firstUpdate of every object will be called each frame before any object's update
		virtual void staticUpdate(){}//will be called every frame after firstUpdate
		virtual void threadedUpdate(){}//this will be called in parallel to all staticUpdate calls
		virtual void timedUpdate(){}//called at a setable interval unless the previous batch has not finished

		//this is called before any child constructors, adds itself to the given engine object
		obj(engine* e,metadata *meta=0);
		obj(engine* e,std::string fname,metadata *meta=0);
	};
}

#endif // OBJECT_H_INCLUDED
