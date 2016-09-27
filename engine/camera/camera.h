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

		/*TODO: stronger evaluation of whether 3d or 4d matrix is right,
		as of right now, however, i remain a fan of the less waste and increased clarity of this option
		4d matrices may be "slick" and a tad more systematic, but it still wastes time operating on values that are never used*/
		matrix3 axes;
		vec3d position;

		camera(surface* renderTarget,scene* sceneToDraw=0);

		void rotateDeg(double angle,vec3d *axis);
		void rotateRad(double angle,vec3d *axis);

		void render(surface *drawTo=0,scene *drawScene=0);
	};
}

#endif // CAMERA_H_INCLUDED
