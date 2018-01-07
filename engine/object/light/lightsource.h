#ifndef LIGHTSOURCE_H_INCLUDED
#define LIGHTSOURCE_H_INCLUDED

#include "../../math/vec3d.h"
#include "../../camera/ray.h"

namespace MG{
	class sceneContainer;

	class light{
		/*TODO: file streaming/caching is probably wanted, but does this really need to be in the global object model?
		also worth noting is future plan to read a more sophisticated file format via dictionaries or something, which will need more work
		*/
	protected:
		sceneContainer *owner;
	public:
		vec3d pos;
		color c{1,1,1};

		light(const light &o);
		light(sceneContainer* sc);
		virtual ~light();

		virtual void getRays(const int n,ray* target) const=0;
	};
}

#endif // LIGHTSOURCE_H_INCLUDED
