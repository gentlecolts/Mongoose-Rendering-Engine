#ifndef RAY_H_INCLUDED
#define RAY_H_INCLUDED

#include "../math/vec3d.h"

namespace MG{
	struct color{
		union{
			struct{float r,g,b;};
			float rgb[3];
		};

		color():r(0),g(0),b(0){}
		color(double r0,double g0,double b0):r(r0),g(g0),b(b0){}
		color(double c[3]):r(c[0]),g(c[1]),b(c[2]){}

		template<typename T> explicit operator vec3<T>() const{
			return vec3<T>(r,g,b);
		}
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
