#ifndef RAY_H_INCLUDED
#define RAY_H_INCLUDED

#include "../math/vec3d.h"

namespace MG{
	struct color{
		union{
			struct{float r,g,b;};
			float rgb[3];
		};
	};

	/*TODO: consider need for additional parameters, such as:
	-travel distance at "from"
	-intensity (ie how much real light does the ray represent)
	-polarization
	-real ray vs shadow vs miss vs DNE (ray after a miss?)
	-visible to the camera or not?
	and other factors that might be needed
	*/
	struct ray{
		vec3d from,dir;
		color c;
	};
}

#endif // RAY_H_INCLUDED
