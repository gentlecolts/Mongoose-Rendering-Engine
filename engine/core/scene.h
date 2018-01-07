#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include <vector>
#include <queue>
#include "../object/object.h"
#include "../object/light/lightsource.h"

namespace MG{
	struct sceneContainer{
		//TODO: need to intelligently store map chunks
		//TODO: need to write functions for retrieving objects, might need a secondary structure to store names, for example
		std::vector<obj*> objects;
		std::queue<obj*> toRemove;
		std::vector<light*> lights;
		//std::queue<std::vector<obj*>::iterator> toRemove;
		//std::vector<light*> lights;//TODO: need to implement lighting

		sceneContainer();

		//void renderTo(surface &target);
		//void render(camera &cam);
		void render(int numRays,ray *raysIn,ray *raysOut);

		void addObject(obj *o);
		void removeObject(obj *o);
		void addLight();
		void removeLight();
		void clearScene();
	};
}
#endif // SCENE_H_INCLUDED
