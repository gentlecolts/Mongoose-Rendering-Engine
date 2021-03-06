#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "../2d/surface.h"
#include "../math/math.h"
#include "../core/scene.h"

namespace MG{
	class camera{
		void renderLoop(int id,int numthreads,ray **rays,color *raw,int raycount,surface *renderTarget,int bounces,sceneContainer *usingScene);
		void doPost(int id,int numthreads,color *raw,surface *targetSurface);

		void initRays(int id,int numthreads,ray **rays,int raycount,int bounces);
	public:
		surface *target=0;
		sceneContainer *scene=0;

		/*TODO: stronger evaluation of whether 3d or 4d matrix is right,
		as of right now, however, i remain a fan of the less waste and increased clarity of this option
		4d matrices may be "slick" and a tad more systematic, but it still wastes time operating on values that are never used*/
		matrix3 axes;
		vec3d position;

		camera(surface* renderTarget,sceneContainer* sceneToDraw=0);

		void rotateDeg(double angle,const vec3d& axis);
		void rotateRad(double angle,const vec3d& axis);

		void lookAt(const vec3d &point,const vec3d& up);

		void render(surface *drawTo=0,sceneContainer *drawScene=0);
	};
}

#endif // CAMERA_H_INCLUDED
