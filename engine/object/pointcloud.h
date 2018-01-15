#ifndef POINTCLOUD_H_INCLUDED
#define POINTCLOUD_H_INCLUDED

#include "object.h"
#include "../math/vec3d.h"
#include "../camera/ray.h"
#include <list>
#include <vector>

namespace MG{
	struct point{
	/*points in this point cloud are represented as an elipsoid
	Todo: need material information and color
	*/
		vec3d pos,scale;
		color col;

		point():scale(1,1,1){}

		bool intersects(const ray& r,double &t0,double& t1) const;
	};

	//typedef std::list<int> hashtype;
	typedef std::vector<int> hashtype;

	struct spacehash{
		/*store this list separate for a few reasons
		1 - keep all the points in consecutive memory
		2 - make re-hashing better since index ints are smaller than the value at the index

		TODO: consider if there's any value in the ability to add/remove points
		*/
		point *pointarr=0;
		/*TODO: weigh the access speed of std::vector vs the insert/remove speed of std::list
		vectors have better random access (maybe better begin to end iteration?) which may happen a lot more
		however transforming individual points requires inserts and deletes, more or less depending on how flexible said transforms are
		-free transforms like that (probably) dont happen too often, is it worth how much they'd a vector vector vs list?
		-vector push_back isnt so bad in terms of performance
		-begin to end iteration will happen many times per frame regardless of how often transforms happen
		*/
		hashtype *pointhash=0;

		//some constants determined at initialization
		unsigned int xbits,ybits,zbits;
		unsigned int xmask,ymask,zmask;
		unsigned int xdim,ydim,zdim;
		unsigned int npoints=0,hashsize=0;
		vec3d pos,scale;

		spacehash(const spacehash &space);
		spacehash(const int boxcount,const int sizeAssert=0);
		spacehash(const int boxcount,point points[],const int pointcount,const int sizeAssert=0);
		//spacehash(const spacehash &space);//is deep copy needed?
		~spacehash();

		void hash(point points[],int npoints);
		hashtype& fetchcell(const vec3d& point);

		//void transform(fn,condition_fn=0)
	};

	class pointcloud:obj{
	protected:
		spacehash hashbox;
	public:
		/*
		TODO: major revision of this once more robust file reading is implemented
		instead of receiving a vector array and length int, this will likely be passed a more sophisticated data type
		TODO: need to re-work the relationship between the hashbox and the object.  the hashbox should map all points inside itself to [-1,1] (consider whether or not to use real coords of points/ray for intersection test though)
		*/
        pointcloud(sceneContainer* e,point points[],const int numpoints,const int density=100,const int sizeAssert=0);
        pointcloud(sceneContainer* e,metadata *meta,point points[],const int numpoints,const int density=100,const int sizeAssert=0);

        virtual bool bounceRay(const ray &r_in,ray &r_out,double &d);
	};
}

#endif // POINTCLOUD_H_INCLUDED
