#include "pointcloud.h"
#include <cmath>
#include <algorithm>
using namespace MG;

///// point stuff goes here /////
inline bool intersect(const point& p,const ray& r,float &t0,float& t1){	/*
	P=a point on the ellipsoid
		[	1/a		0		0	]
	M=	[	0		1/b		0	]
		[	0		0		1/c	]
	such that the ellipsoid is defined by
	((x-c1)/a)^2 + ((y-c2)/b)^2 + ((z-c3)/c)^2 = 1

	thus this problem may be expressed via:
	|(P-C)*M|^2=1

	since (A dot A) = |A|^2 we can substitute, then distribute M
	(P*M-C*M) dot (P*M-C*M) = 1

	define the following:
	P=P0+t*v
	P1=P0*M-C*M=(P0-C)*M
	v1=v*M

	substituting gives
	(P0*M + t*v*M - C*M) dot (P0*M + t*v*M - C*M) = 1
	(P0*M - C*M + t*v*M) dot (P0*M - C*M + t*v*M) = 1
	(P1 + t*v1) dot (P1 + t*v1) = 1

	finally, we get
	|P1|^2 + 2*t*(P1 dot v1) + t^2*|v1|^2 = 1

	which may be solved as a quadratic where
	a=|v1|^2
	b=2*(P1 dot v1)
	c=|P1|^2 - 1
	*/
	const vec3d P1(
		(r.from.x-p.pos.x)/p.wid.x,
		(r.from.y-p.pos.y)/p.wid.y,
		(r.from.z-p.pos.z)/p.wid.z
	);
	const vec3d v1(
		r.dir.x/p.wid.x,
		r.dir.y/p.wid.y,
		r.dir.z/p.wid.z
	);

	const float
		a=v1.dot(v1),
		b=2*P1.dot(v1),
		c=P1.dot(P1)-1;

	const float
		eq=b*b-4*a*c,
		rt=std::sqrt(eq);
	//in this context, (a) is a sum of squares and thus must always be positive, t0 will always be the lower value
	t0=(-b-rt)/(2*a);
	t1=(-b+rt)/(2*a);
	//*/

	return (eq>=0) & ((t0>=0) | (t1>0));
}

bool point::intersects(const ray& r,float &t0,float& t1) const{
	return intersect(*this,r,t0,t1);
}

///// spacehash stuff goes here /////
/**returns the minimum number of bits required to represent the given value
ex:
f(0)=1
f(1)=1
f(2)=2
f(3)=2
f(4)=3
f(5)=3
f(6)=3
f(7)=3
f(8)=4
f(9)=4
f(10)=4
*/
static inline int minbits(const uint32_t x){
	#if __GNUC__
		#if __x86_64__ || __x86__
			uint32_t y;
			asm("\tbsr %1, %0\n\t"
				"inc %0"
				: "=r"(y)
				: "r" (x)
			);
			return y;
		#else
			return x?32-__builtin_clz(x):1;
		#endif
	#else
		return (x?1+log2(x):1)
	#endif
}

spacehash::spacehash(int hashsize){
	int bits=minbits(hashsize);
	xbits=ybits=zbits=(bits>2)?bits/3:1;

	if(bits>2){
		switch(bits%3){
		case 2:
			zbits++;
		case 1:
			xbits++;
		}
	}

	xdim=(1u<<xbits);
	ydim=(1u<<ybits);
	zdim=(1u<<zbits);

	xmask=xdim-1;
	ymask=ydim-1;
	zmask=zdim-1;

	pointarr=new point[hashsize]();
	pointhash=new std::list<int>[xdim*ydim*zdim];
}
spacehash::spacehash(int hashsize,point points[],int npoints):spacehash(hashsize){
	hash(points,npoints);
}
spacehash::~spacehash(){
	delete[] pointarr;
	delete[] pointhash;
}

//linear transform applied to any given point that's going to be mapped
inline vec3d pointTF(const spacehash &space,const vec3d &p){
	/*not to self: not being modulus is good, but there is one edge case
	if (point-space.pos)/space.scale were to be 1, there would be an issue
	however, since determining pos and scale accounts for the size of each point,
	this could not happen unless a point had zero width/height/depth
	*/
	return vec3d(
		space.xdim*(p.x-space.pos.x)/space.scale.x,
		space.ydim*(p.y-space.pos.y)/space.scale.y,
		space.zdim*(p.z-space.pos.z)/space.scale.z
	);
}
//#define MAPTF(point,dim) (dim*((point)+1)/2)

//the int representing index in the array
inline void remap(const spacehash &space,const vec3d &p,unsigned int &x,unsigned int &y,unsigned int &z){
	const vec3d mapped=pointTF(space,p);
	x=((unsigned int)mapped.x)&space.xmask;
	y=((unsigned int)mapped.y)&space.ymask;
	z=((unsigned int)mapped.z)&space.zmask;
}

void spacehash::hash(point points[],int npoints){
	vec3d
		minp=points[0].pos-points[0].wid,
		maxp=points[0].pos+points[0].wid,
		mintest,maxtest;

	for(int i=1;i<npoints;i++){
		mintest=points[i].pos-points[i].wid;
		maxtest=points[i].pos+points[i].wid;

		minp.x=std::min(minp.x,mintest.x);
		minp.y=std::min(minp.y,mintest.y);
		minp.z=std::min(minp.z,mintest.z);

		maxp.x=std::max(maxp.x,maxtest.x);
		maxp.y=std::max(maxp.y,maxtest.y);
		maxp.z=std::max(maxp.z,maxtest.z);
	}

	/*
	//map all points to [-1,1]
	pos=(maxp+minp)/2;
	scale=(maxp-minp)/2;
	/*/
	//map all points to [0,1]
	pos=minp;
	scale=maxp-minp;
	//*/

	for(int i=0;i<npoints;i++){
		pointarr[i]=points[i];

		unsigned int x,y,z;
		remap(*this,pointarr[i].pos,x,y,z);

		pointhash[x+xdim*(y+ydim*z)].push_back(i);
	}
}

inline std::list<int>& fetch(const spacehash& space,const vec3d& point){
	unsigned int x,y,z;
		remap(space,point,x,y,z);
	return space.pointhash[x+space.xdim*(y+space.ydim*z)];
}

std::list<int>& spacehash::fetchcell(const vec3d& point){
	return fetch(*this,point);
}

///// pointcloud stuff goes here /////

pointcloud::pointcloud(engine* e,point points[],int numpoints,int density):obj(e),hashbox(numpoints/density,points,numpoints){
}
pointcloud::pointcloud(engine* e,metadata *meta,point points[],int numpoints,int density):obj(e,meta),hashbox(numpoints/density,points,numpoints){
}

bool pointcloud::bounceRay(const ray &r_in,uint32_t &color,ray &r_out,double *d,vec3d *normal){
	//convert the ray to the space of the hash
	//TODO: implement transform matrix(?), calculated before the hashbox transform
	ray transRay=r_in;
	transRay.from-=pos;
	//[matrix part would go here]
	transRay.from=pointTF(hashbox,transRay.from);
	transRay.dir=pointTF(hashbox,transRay.dir);

	//find the t start and stop where this ray intersects the hashbox (hashbox is [0,1) in x,y,z)

	//gather all cells and find the the closeset point

	//set the returns up

	return false;
}
