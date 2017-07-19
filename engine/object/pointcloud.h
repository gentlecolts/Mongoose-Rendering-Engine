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
		vec3d pos;
		float wx,wy,wz;

		point():pos(),wx(1),wy(1),wz(1){}

		bool intersects(ray& r,float &t0,float& t1);
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

		spacehash(int hashsize);
		spacehash(int hashsize,point points[],int npoints);
		~spacehash();

		void hash(point points[],int npoints);

		//void transform(fn,condition_fn=0)
	};

	class pointcloud:obj{
	protected:
		spacehash hashbox;
		void init(vec3d points[],int numpoints,int density);
	public:
		/*
		TODO: major revision of this once more robust file reading is implemented
		instead of reciving a vector array and lenght int, this will likely be passed a more sophisticated data type
		*/
        pointcloud(engine* e,point points[],int numpoints,int density=100);
        pointcloud(engine* e,metadata *meta,point points[],int numpoints,int density=100);

        virtual bool bounceRay(ray &r_in,uint32_t &color,ray &r_out,double *d=0,vec3d *normal=0);
	};
}

#endif // POINTCLOUD_H_INCLUDED
