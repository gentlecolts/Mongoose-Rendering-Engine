#include "pointcloud.h"
#include <cmath>
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
		(r.from.x-p.pos.x)/p.wx,
		(r.from.y-p.pos.y)/p.wy,
		(r.from.z-p.pos.z)/p.wz
	);
	const vec3d v1(
		r.dir.x/p.wx,
		r.dir.y/p.wy,
		r.dir.z/p.wz
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
#define MAPTF(point,dim) (dim*(point))
//#define MAPTF(point,dim) (dim*((point)+1)/2)

//the int representing index in the array
#define REMAP(point,dim,mask) (MAPTF((unsigned int)point,dim)&mask)

void spacehash::hash(point points[],int npoints){
	for(int i=0;i<npoints;i++){
		pointarr[i]=points[i];
		const unsigned int
			x=REMAP(pointarr[i].pos.x,xdim,xmask),
			y=REMAP(pointarr[i].pos.y,ydim,ymask),
			z=REMAP(pointarr[i].pos.z,zdim,zmask);
		pointhash[x+xdim*(y+ydim*z)].push_back(i);
	}
}

inline std::list<int>& fetch(const spacehash& space,const vec3d& point){
	const unsigned int
		x=REMAP(point.x,space.xdim,space.xmask),
		y=REMAP(point.y,space.ydim,space.ymask),
		z=REMAP(point.x,space.zdim,space.zmask);
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
	transRay.from.x=MAPTF(transRay.from.x,hashbox.xdim);
	transRay.from.y=MAPTF(transRay.from.y,hashbox.ydim);
	transRay.from.z=MAPTF(transRay.from.z,hashbox.zdim);
	transRay.dir.x=MAPTF(transRay.dir.x,hashbox.xdim);
	transRay.dir.y=MAPTF(transRay.dir.y,hashbox.ydim);
	transRay.dir.z=MAPTF(transRay.dir.z,hashbox.zdim);

	//find the t start and stop where this ray intersects the hashbox (hashbox is [0,1) in x,y,z)

	//gather all cells and find the the closeset point

	//set the returns up

	return false;
}
