#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED
/**
this file contains the definition for the abstract object class, which is the parent class to all 3d objects supported by this engine
*/

#include "../math/vec3d.h"
#include "../camera/ray.h"
#include "meta.h"
#include <stdint.h>
#include <fstream>



namespace MG{
	class engine;

	class obj{
		/*TODO: file streaming/caching is probably wanted, but does this really need to be in the global object model?
		also worth noting is future plan to read a more sophisticated file format via dictionaries or something, which will need more work
		*/
		std::fstream file;
	public:
		vec3d pos;

		virtual bool bounceRay(const ray &r_in,uint32_t &color,ray &r_out,double *d=0,vec3d *normal=0) = 0;

		virtual void firstUpdate(){}//the firstUpdate of every object will be called each frame before any object's update
		virtual void forkedUpdate(){}//forkedUpdate is called after first update and is called in true threaded parallel
		virtual void joinedUpdate(){}//called after all forkedUpdates finish
		virtual void timedUpdate(){}//called at a settable interval unless the previous batch has not finished

		//this is called before any child constructors, adds itself to the given engine object
		obj(engine* e,metadata *meta=0);
		obj(engine* e,std::string fname,metadata *meta=0);
	};
}

#endif // OBJECT_H_INCLUDED
