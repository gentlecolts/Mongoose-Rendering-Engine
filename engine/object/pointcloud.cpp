#include "pointcloud.h"
#include <cmath>
using namespace MG;

///// point stuff goes here /////

bool point::intersects(ray& r,float &t0,float& t1){
	/*
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
		(r.from.x-pos.x)/wx,
		(r.from.y-pos.y)/wy,
		(r.from.z-pos.z)/wz
	);
	const vec3d v1(
		r.dir.x/wx,
		r.dir.y/wy,
		r.dir.z/wz
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

void spacehash::hash(point points[],int npoints){
	for(int i=0;i<npoints;i++){
		pointarr[i]=points[i];
		unsigned int
			x=((unsigned int)pointarr[i].pos.x)&xmask,
			y=((unsigned int)pointarr[i].pos.y)&xmask,
			z=((unsigned int)pointarr[i].pos.z)&xmask;
		pointhash[x+xdim*(y+ydim*z)].push_back(i);
	}
}

///// pointcloud stuff goes here /////

pointcloud::pointcloud(engine* e,point points[],int numpoints,int density):obj(e),hashbox(numpoints/density,points,numpoints){
	//init(points,numpoints,density);
}
pointcloud::pointcloud(engine* e,metadata *meta,point points[],int numpoints,int density):obj(e,meta),hashbox(numpoints/density,points,numpoints){
	//init(points,numpoints,density);
}

void pointcloud::init(vec3d points[],int numpoints,int density){
}

bool pointcloud::bounceRay(ray &r_in,uint32_t &color,ray &r_out,double *d,vec3d *normal){
	return false;
}
