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

	class obj{
	public:
		vec3d pos;
		virtual bool bounceRay(ray &r,uint32_t &color,double *d=0,vec3d *normal=0) = 0;
		virtual void update() = 0;//will be called every frame
	};
}

#endif // OBJECT_H_INCLUDED
