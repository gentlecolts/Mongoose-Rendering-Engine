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

	struct ray{
		vec3d from,to;
		color c;
	};
}

#endif // RAY_H_INCLUDED
