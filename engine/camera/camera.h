#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "../2d/surface.h"
#include "../math/math.h"

namespace MG{
	class scene;

	class camera{
	public:
		surface *target=0;
		scene *sceneToDraw=0;

		matrix3 axes;
		vec3d position;

		camera();
		camera(surface* renderTarget);
		camera(scene *s);
		camera(surface* renderTarget,scene* sceneToDraw);

		void rotateDeg(double angle,vec3d *axis);
		void rotateRad(double angle,vec3d *axis);

		void render(surface *drawTo=0,scene *drawScene=0);
	};
}

#endif // CAMERA_H_INCLUDED
