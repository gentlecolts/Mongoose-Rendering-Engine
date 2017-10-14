#ifndef SCENE_H_INCLUDED
#define SCENE_H_INCLUDED

#include <vector>
#include <queue>
#include "../object/object.h"

namespace MG{
	struct sceneContainer{
		//TODO: need to intelligently store map chunks
		//TODO: need to write functions for retrieving objects, might need a secondary structure to store names, for example
		std::vector<obj*> objects;
		std::queue<obj*> toRemove;
		//std::queue<std::vector<obj*>::iterator> toRemove;
		//std::vector<light*> lights;//TODO: need to implement lighting

		sceneContainer();

		//void renderTo(surface &target);
		//void render(camera &cam);
		void render(int numRays,ray *raysIn,ray *raysOut);
	};
}
#endif // SCENE_H_INCLUDED
