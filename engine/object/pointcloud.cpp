#include "pointcloud.h"
#include <cmath>
#include <algorithm>
#include <utility>
using namespace MG;

///// point stuff goes here /////
inline bool intersect(const point& p,const ray& r,double &t0,double& t1){	/*
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
		(r.from.x-p.pos.x)/p.scale.x,
		(r.from.y-p.pos.y)/p.scale.y,
		(r.from.z-p.pos.z)/p.scale.z
	);
	const vec3d v1(
		r.dir.x/p.scale.x,
		r.dir.y/p.scale.y,
		r.dir.z/p.scale.z
	);

	//*
	const double
		a=v1.dot(v1),
		b=2*P1.dot(v1),
		c=P1.dot(P1)-1;

	const double
		eq=b*b-4*a*c,
		rt=std::sqrt(eq);
	//in this context, (a) is a sum of squares and thus must always be positive, t0 will always be the lower value
	t0=(-b-rt)/(2*a);
	t1=(-b+rt)/(2*a);
	/*/
	//this should be completely equivalent to the above, since the 2 can be factored out of b
	//the above, more traditional version will be preserved for clarity
	const double
		a=v1.dot(v1),
		b=P1.dot(v1),
		c=P1.dot(P1)-1;

	const double
		eq=(b*b-a*c),
		rt=std::sqrt(eq);
	//in this context, (a) is a sum of squares and thus must always be positive, t0 will always be the lower value
	t0=(-b-rt)/a;
	t1=(-b+rt)/a;
	//*/

	return (eq>=0) & ((t0>=0) | (t1>=0));
}

bool point::intersects(const ray& r,double &t0,double& t1) const{
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
		return (x?1+std::log2(x):1)
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

	hashsize=1u<<(xbits+ybits+zbits);
	pointhash=new hashtype[hashsize];
}
spacehash::spacehash(int hashsize,point points[],int pointcount):spacehash(hashsize){
	npoints=pointcount;
	pointarr=new point[npoints]();

	hash(points,npoints);
}
//spacehash::spacehash(const spacehash &space){}
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

inline void maskpoint(const spacehash &space,const vec3d &p,unsigned int &x,unsigned int &y,unsigned int &z){
	x=((unsigned int)p.x)&space.xmask;
	y=((unsigned int)p.y)&space.ymask;
	z=((unsigned int)p.z)&space.zmask;
}

//the int representing index in the array
inline void remap(const spacehash &space,const vec3d &p,unsigned int &x,unsigned int &y,unsigned int &z){
	maskpoint(space,pointTF(space,p),x,y,z);
}

inline hashtype& fetch(const spacehash &space,const unsigned int x,const unsigned int y,const unsigned int z){
	const unsigned int index=((((x&space.xmask)<<space.ybits)|(y&space.ymask))<<space.zbits)|(z&space.zmask);
	//const unsigned int index=x+space.xdim*(y+space.ydim*z);

	//printf("dims: (%u, %u, %u)...accessing point: (%u, %u, %u), gives index: %u, max: %u\n",space.xdim,space.ydim,space.zdim,x,y,z,index,1u<<(space.xbits+space.ybits+space.zbits));

	//return space.pointhash[x+space.xdim*(y+space.ydim*z)];
	return space.pointhash[index];
}
inline hashtype& fetch(const spacehash& space,const vec3d& point){
	unsigned int x,y,z;
	remap(space,point,x,y,z);

	return fetch(space,x,y,z);
}

hashtype& spacehash::fetchcell(const vec3d& point){
	return fetch(*this,point);
}

void spacehash::hash(point points[],int npoints){
	//TODO: merge instead of clearing
	//delete[] pointarr;
	//delete[] pointhash;

	vec3d
		minp=points[0].pos-points[0].scale,
		maxp=points[0].pos+points[0].scale,
		mintest,maxtest;

	for(int i=1;i<npoints;i++){
		mintest=points[i].pos-points[i].scale;
		maxtest=points[i].pos+points[i].scale;

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

		printf("adding point %i to pos (%f, %f, %f)\n",i,points[i].pos.x,points[i].pos.y,points[i].pos.z);
		auto &box=fetch(*this,points[i].pos);
		//printf("size of target vector is: %u\n",i,box.size());
		box.push_back(i);
		printf("vector size is now: %lu\n\n",box.size());
	}

	//*debugging
	for(unsigned int x=0;x<xdim;x++){
		for(unsigned int y=0;y<ydim;y++){
			for(unsigned int z=0;z<zdim;z++){
				printf("(%i, %i, %i) has size: %lu\n",x,y,z,fetch(*this,x,y,z).size());
			}
		}
	}
	printf("hash done\n");
	//*/
}

///// pointcloud stuff goes here /////

pointcloud::pointcloud(engine* e,point points[],int numpoints,int density):obj(e),hashbox(numpoints/density,points,numpoints){
}
pointcloud::pointcloud(engine* e,metadata *meta,point points[],int numpoints,int density):obj(e,meta),hashbox(numpoints/density,points,numpoints){
}

bool pointcloud::bounceRay(const ray &r_in,ray &r_out,double &d){
	/*testcode, rays always hit and out ray is red, if screen is red then all other code works
	d=1;
	r_out.normal=vec3d(1,1,1);
	r_out.c.r=1;
	r_out.from=r_in.dir+r_in.from;
	r_out.dir=-1.0*r_in.dir;
	return r_out.hit=true;
	//*/

	//convert the ray to the space of the hash
	//TODO: implement transform matrix(?), calculated before the hashbox transform
	ray rin_loc=r_in;
	//move the ray so that the hashbox's lower corner is at origin
	rin_loc.from-=pos;
	//[matrix transform would go here]
	//rin_loc.from=pointTF(hashbox,rin_loc.from);
	//rin_loc.dir=pointTF(hashbox,rin_loc.dir);

	//find the t start and stop where this ray intersects the hashbox (hashbox is [0,hashbox.dim] in x,y,z)
	vec3d tmin,tmax;

	tmin.x=(hashbox.xdim+hashbox.pos.x-rin_loc.from.x)/rin_loc.dir.x; tmax.x=(hashbox.pos.x-rin_loc.from.x)/rin_loc.dir.x;
	tmin.y=(hashbox.ydim+hashbox.pos.y-rin_loc.from.y)/rin_loc.dir.y; tmax.y=(hashbox.pos.y-rin_loc.from.y)/rin_loc.dir.y;
	tmin.z=(hashbox.zdim+hashbox.pos.z-rin_loc.from.z)/rin_loc.dir.z; tmax.z=(hashbox.pos.z-rin_loc.from.z)/rin_loc.dir.z;

	//TODO: better approach to this?
	if(tmin.x>tmax.x){std::swap(tmin.x,tmax.x);}
	if(tmin.y>tmax.y){std::swap(tmin.y,tmax.y);}
	if(tmin.z>tmax.z){std::swap(tmin.z,tmax.z);}

	const double
		//the ray enters the cube at the last of the possible entry planes
		t0=std::max(std::max(tmin.x,tmin.y),tmin.z),
		//the ray exits the cube at the first of the possible exit planes
		t1=std::min(std::min(tmax.x,tmax.y),tmax.z);

	if(!( (t1>=t0)&((t0>=0)|(t1>=0)) )){return r_out.hit=false;}

	unsigned int xin,yin,zin,xout,yout,zout;
	maskpoint(hashbox,rin_loc.dir*t0+rin_loc.from,xin,yin,zin);
	maskpoint(hashbox,rin_loc.dir*t1+rin_loc.from,xout,yout,zout);

	//gather all cells and find the the closeset point
	const int
		dx=xout-xin,dy=yout-yin,dz=zout-zin,
		n=std::max(std::max(std::abs(dx),std::abs(dy)),std::abs(dz));

	if(n<=0){return r_out.hit=false;}

	struct interholder{
		double t=INFINITY;
		int index=0;
		bool hit=false;
	} closest;

	//*
	for(int i=0;i<=n;i++){
		//get the correct chunk of the box
		const unsigned int
			x=xin+i*dx/n,
			y=yin+i*dy/n,
			z=zin+i*dz/n;
		hashtype &plist=fetch(hashbox,x,y,z);
	/*/
	for(int i=0;i<hashbox.hashsize;i++){
		hashtype &plist=hashbox.pointhash[i];
	//*/
		//printf("line is at (%u, %u, %u), list size is: %lu\n",x,y,z,plist.size());

		std::for_each(plist.begin(),plist.end(),[&](int pos){
			interholder itest;
			double t0,t1;
			itest.index=pos;
			//note, t0 is always less than t1
			itest.hit=intersect(hashbox.pointarr[pos],rin_loc,t0,t1);
			itest.t=(t0>=0)?t0:t1;

			/*
			printf("testing point #%i\nray: <%f, %f, %f>*t + <%f, %f, %f>\npoint: <%f, %f, %f>\n",pos,
				rin_loc.dir.x,rin_loc.dir.y,rin_loc.dir.z,
				rin_loc.from.x,rin_loc.from.y,rin_loc.from.z,
				hashbox.pointarr[pos].pos.x,hashbox.pointarr[pos].pos.y,hashbox.pointarr[pos].pos.z
			);//*/

			//printf("did ray hit point #%i:\t%s,\tt0=%f\tt1=%f\n",pos,itest.hit?"yes":"no",t0,t1);
			/*
			if(itest.hit){
				//printf("hit #%i\n",pos);
				//printf("hit #%i:\tt0=%f\tt1=%f\n",pos,t0,t1);
				//printf("hit #%i:\tt=%f\t<%f, %f, %f>*t + <%f, %f, %f>\n",pos,itest.t,rin_loc.dir.x,rin_loc.dir.y,rin_loc.dir.z,rin_loc.from.x,rin_loc.from.y,rin_loc.from.z);
				printf("hit #%i:\tt=%f\t<%f, %f, %f>*t + <%f, %f, %f>\n",pos,itest.t,r_in.dir.x,r_in.dir.y,r_in.dir.z,r_in.from.x,r_in.from.y,r_in.from.z);
			}
			//*/


			//if hit is true, then test_t must be positive, close_t starts out at INFINITY
			closest=(itest.hit & (itest.t<closest.t))?itest:closest;
			//closest=itest.hit?itest:closest;
		});
	}

	//set the returns up, remember to stay in world coords and not the cord space of this object
	d=closest.t;
	r_out.from=r_in.dir*d+r_in.from;

	point &p=hashbox.pointarr[closest.index];
	//TODO: instead of referencing, copy and apply this object's transform matrix to the point
	vec3d inters=r_out.from-pos-p.pos;

	r_out.normal=vec3d(
		2*p.scale.x*inters.x,
		2*p.scale.y*inters.y,
		2*p.scale.z*inters.z
	).getNormalized();

	//compute a purely reflected ray, TODO: more complex, material based bounces
	r_out.dir=r_in.dir-2*(r_in.dir.dot(r_out.normal))*r_out.normal;
	r_out.c=p.col;

	//printf("final: hit(%i)\tt=%f\t<%f, %f, %f>*t + <%f, %f, %f>\n",closest.hit,d,r_out.dir.x,r_out.dir.y,r_out.dir.z,r_out.from.x,r_out.from.y,r_out.from.z);
	//*
	if(closest.hit){
		printf("final: t=%f\tin:<%f, %f, %f>*t + <%f, %f, %f>\tout:<%f, %f, %f>*t + <%f, %f, %f>\n",d,
			r_in.dir.x,r_in.dir.y,r_in.dir.z,r_in.from.x,r_in.from.y,r_in.from.z,
			r_out.dir.x,r_out.dir.y,r_out.dir.z,r_out.from.x,r_out.from.y,r_out.from.z
		);
	}
	//*/
	return r_out.hit=closest.hit;
}
