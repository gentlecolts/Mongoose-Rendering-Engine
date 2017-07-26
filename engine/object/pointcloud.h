#ifndef POINTCLOUD_H_INCLUDED
#define POINTCLOUD_H_INCLUDED

#include "object.h"
#include "../math/vec3d.h"
#include "../camera/ray.h"
#include <list>

namespace MG{
	struct point{
	/*points in this point cloud are represented as an elipsoid
	Todo: need material information and color
	*/
		vec3d pos,wid;

		point():pos(),wid(1,1,1){}

		bool intersects(const ray& r,float &t0,float& t1) const;
	};

	struct spacehash{
		/*store this list separate for a few reasons
		1 - keep all the points in consecutive memory
		2 - make re-hashing better since index ints are smaller than the value at the index

		TODO: consider if there's any value in the ability to add/remove points
		*/
		point *pointarr;
		std::list<int> *pointhash;

		//some constants determined at initialization
		unsigned int xbits,ybits,zbits;
		unsigned int xmask,ymask,zmask;
		unsigned int xdim,ydim,zdim;
		vec3d pos,scale;

		spacehash(int hashsize);
		spacehash(int hashsize,point points[],int npoints);
		~spacehash();

		void hash(point points[],int npoints);
		std::list<int>& fetchcell(const vec3d& point);

		//void transform(fn,condition_fn=0)
	};

	class pointcloud:obj{
	protected:
		spacehash hashbox;
	public:
		/*
		TODO: major revision of this once more robust file reading is implemented
		instead of reciving a vector array and lenght int, this will likely be passed a more sophisticated data type
		*/
        pointcloud(engine* e,point points[],int numpoints,int density=100);
        pointcloud(engine* e,metadata *meta,point points[],int numpoints,int density=100);

        virtual bool bounceRay(const ray &r_in,uint32_t &color,ray &r_out,double *d=0,vec3d *normal=0);
	};
}

#endif // POINTCLOUD_H_INCLUDED
