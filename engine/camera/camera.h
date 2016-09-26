#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "../2d/surface.h"

namespace MG{
	class scene;

	class camera{
		surface *target;
	public:
		camera(surface* renderTarget);
	};
}

#endif // CAMERA_H_INCLUDED
