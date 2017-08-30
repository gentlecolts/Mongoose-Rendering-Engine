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

	additionally, factoring the 2 out of b can be used to simplify things slightly:

	let u=(P1 dot v1)
	b=2*u

	t=(-b ± sqrt(b*b-4*a*c))/(2*a)
	t=(-2*u ± sqrt(2*u*2*u-4*a*c))/(2*a)
	t=(-2*u ± sqrt(4*(u*u-a*c)))/(2*a)
	t=(-2*u ± 2*sqrt(u*u-a*c))/(2*a)
	t=2*(-u ± sqrt(u*u-a*c))/(2*a)
	t=(-u ± sqrt(u*u-a*c))/a

	this is, of course, an extremely minor simplification, but it's free and any little bit helps
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

spacehash::spacehash(const spacehash &space){
	npoints=space.npoints;
	pointarr=new point[npoints];
	std::copy(space.pointarr,space.pointarr+npoints,pointarr);

	hashsize=space.hashsize;
	pointhash=new hashtype[hashsize];
	std::copy(space.pointhash,space.pointhash+hashsize,pointhash);

	#define duplicate(x) x=space.x;
	duplicate(xbits)
	duplicate(ybits)
	duplicate(zbits)
	duplicate(xmask)
	duplicate(ymask)
	duplicate(zmask)
	duplicate(xdim)
	duplicate(ydim)
	duplicate(zdim)
	duplicate(pos)
	duplicate(scale)
	#undef duplicate
}

spacehash::spacehash(int boxcount){
	int bits=minbits(boxcount);
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
spacehash::spacehash(int boxcount,point points[],int pointcount):spacehash(boxcount){
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
	//*
	return vec3d(
		space.xdim*(p.x-space.pos.x)/space.scale.x,
		space.ydim*(p.y-space.pos.y)/space.scale.y,
		space.zdim*(p.z-space.pos.z)/space.scale.z
	);
	/*/
	return vec3d(
		2*space.xdim*(p.x-space.pos.x-space.scale.x/2)/space.scale.x,
		2*space.ydim*(p.y-space.pos.y-space.scale.y/2)/space.scale.y,
		2*space.zdim*(p.z-space.pos.z-space.scale.z/2)/space.scale.z
	);
	//*/
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
		hashtype &box=fetch(*this,points[i].pos);
		//printf("size of target vector is: %u\n",i,box.size());
		box.push_back(i);
		printf("vector size is now: %lu\n\n",box.size());
	}

	//*debugging
	for(unsigned int x=0;x<xdim;x++){
		for(unsigned int y=0;y<ydim;y++){
			for(unsigned int z=0;z<zdim;z++){
				printf("(%u, %u, %u) has size: %lu\n",x,y,z,fetch(*this,x,y,z).size());
			}
		}
	}
	printf("hash done...\n\tmy size is: <%f, %f, %f>\n\tmy \"min\" corner is <%f, %f, %f>\n",scale.x,scale.y,scale.z,pos.x,pos.y,pos.z);
	//*/
}

///// pointcloud stuff goes here /////

pointcloud::pointcloud(engine* e,point points[],int numpoints,int density):obj(e),hashbox(numpoints/density,points,numpoints){
}
pointcloud::pointcloud(engine* e,metadata *meta,point points[],int numpoints,int density):obj(e,meta),hashbox(numpoints/density,points,numpoints){
}

//do comparisons without worrying about division by zero
struct safecompare{
	double num,denom;
	safecompare(double x):num(x),denom(1){}
	safecompare(double n,double d):num(n),denom(d){}

	//if denom is negative, the expression needs to be negated
	#define numcomp(op) bool operator op(const double &x) const{return (denom<0)^(num op x*denom);}

	numcomp(>)
	numcomp(<)
	numcomp(==)
	numcomp(>=)
	numcomp(<=)

	#undef numcomp

	//if only one of the two denoms is negative, the expression needs to be negated
	#define classcomp(op) bool operator op(const safecompare &x) const{return ((denom<0)^(x.denom<0))^(num*x.denom op x.num*denom);}
	classcomp(>)
	classcomp(<)
	classcomp(==)
	classcomp(>=)
	classcomp(<=)

	#undef classcomp

	operator double() const{return num/denom;}
};

template <typename T> inline int sgn(T val) {
    return (T(0) < val) - (val < T(0));
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

	//find the t start and stop where this ray intersects the hashbox (hashbox is [0,hashbox.dim] in x,y,z)
	vec3d tmin,tmax;
	//vec3<safecompare> tmin,tmax;

	///// TODO: fix the missing pixel problem (again...check on v2) /////

	//*
	tmin.x=(hashbox.xdim+hashbox.pos.x-rin_loc.from.x)/rin_loc.dir.x; tmax.x=(hashbox.pos.x-rin_loc.from.x)/rin_loc.dir.x;
	tmin.y=(hashbox.ydim+hashbox.pos.y-rin_loc.from.y)/rin_loc.dir.y; tmax.y=(hashbox.pos.y-rin_loc.from.y)/rin_loc.dir.y;
	tmin.z=(hashbox.zdim+hashbox.pos.z-rin_loc.from.z)/rin_loc.dir.z; tmax.z=(hashbox.pos.z-rin_loc.from.z)/rin_loc.dir.z;
	/*/
	tmin.x=safecompare(hashbox.xdim+hashbox.pos.x-rin_loc.from.x,rin_loc.dir.x); tmax.x=safecompare(hashbox.pos.x-rin_loc.from.x,rin_loc.dir.x);
	tmin.y=safecompare(hashbox.ydim+hashbox.pos.y-rin_loc.from.y,rin_loc.dir.y); tmax.y=safecompare(hashbox.pos.y-rin_loc.from.y,rin_loc.dir.y);
	tmin.z=safecompare(hashbox.zdim+hashbox.pos.z-rin_loc.from.z,rin_loc.dir.z); tmax.z=safecompare(hashbox.pos.z-rin_loc.from.z,rin_loc.dir.z);
	//*/

	//TODO: better approach to this?
	if(tmin.x>tmax.x){std::swap(tmin.x,tmax.x);}
	if(tmin.y>tmax.y){std::swap(tmin.y,tmax.y);}
	if(tmin.z>tmax.z){std::swap(tmin.z,tmax.z);}

	/*
	tmin.x=(std::abs(tmin.x)==INFINITY)?NAN:tmin.x;
	tmax.x=(std::abs(tmax.x)==INFINITY)?NAN:tmax.x;
	tmin.y=(std::abs(tmin.y)==INFINITY)?NAN:tmin.y;
	tmax.y=(std::abs(tmax.y)==INFINITY)?NAN:tmax.y;
	tmin.z=(std::abs(tmin.z)==INFINITY)?NAN:tmin.z;
	tmax.z=(std::abs(tmax.z)==INFINITY)?NAN:tmax.z;
	//*/

	const double
		//the ray enters the cube at the last of the possible entry planes
		t0=std::max(std::max(tmin.x,tmin.y),tmin.z),
		//t0=*(double*)&std::max(std::max(*(long long*)&tmin.x,*(long long*)&tmin.y),*(long long*)&tmin.z),
		//the ray exits the cube at the first of the possible exit planes
		t1=std::min(std::min(tmax.x,tmax.y),tmax.z);
		//t1=*(double*)&std::min(std::min(*(long long*)&tmax.x,*(long long*)&tmax.y),*(long long*)&tmax.z);

	//if(!(std::isfinite(t0) & std::isfinite(t1))){printf("bad t: {%f, %f}\n",t0,t1);}

	//this might not actually be causing the dot problem
	if((t0>t1)|(t1<0)){return r_out.hit=false;}

	//printf("hitbox intersection: {%f, %f}\n",t0,t1);

	/*test hitbox only
	d=(t0>=0)?t0:t1;
	r_out.from=r_in.dir*d+r_in.from;
	r_out.c=color(1,1,1);

	return r_out.hit=true;
	//*/

	//*
	unsigned int xin,yin,zin,xout,yout,zout;
	//maskpoint(hashbox,rin_loc.dir*t0+rin_loc.from,xin,yin,zin);
	//maskpoint(hashbox,rin_loc.dir*t1+rin_loc.from,xout,yout,zout);
	remap(hashbox,rin_loc.dir*t0+rin_loc.from,xin,yin,zin);
	remap(hashbox,rin_loc.dir*t1+rin_loc.from,xout,yout,zout);
	/*/
	const vec3d v0=rin_loc.dir*t0+rin_loc.from,v1=rin_loc.dir*t1+rin_loc.from;
	const double
		xin=v0.x,yin=v0.y,zin=v0.z,
		xout=v1.x,yout=v1.y,zout=v1.z;
	//*/
	//gather all cells and find the the closeset point
	const int
		dx=xout-xin,dy=yout-yin,dz=zout-zin,
		n=std::max(std::max(std::abs(dx),std::abs(dy)),std::abs(dz));

	const double
		dxn=n?(double)(dx)/n:0,
		dyn=n?(double)(dy)/n:0,
		dzn=n?(double)(dz)/n:0;

	//printf("n: %i\t<dxn,dyn,dzn>: <%f, %f, %f>\n",n,dxn,dyn,dzn);
	//printf("n: %i\t<xin,yin,zin>: <%u, %u, %u>\t<xout,yout,zout>: <%u, %u, %u>\t<dxn,dyn,dzn>: <%f, %f, %f>\n",n,xin,yin,zin,xout,yout,zout,dxn,dyn,dzn);

	struct interholder{
		double t=INFINITY;
		int index=0;
		bool hit=false;
	} closest;

	#if 1
	#define SEEK_APPROACH 3
	#if SEEK_APPROACH==0 ///// check every cell /////
	//printf("hashsize: %i\n",hashbox.hashsize);
	for(int i=0;i<hashbox.hashsize;i++){
		//printf("%i\n",i);
		hashtype &plist=hashbox.pointhash[i];
	#elif SEEK_APPROACH==1 ///// draw a line (line is too simple, misses boxes) /////
	for(int i=0;i<=n;i++){
		//get the correct chunk of the box
		#if 1
		const double t=t0+(double(i))*(t1-t0)/n;
		hashtype &plist=fetch(hashbox,rin_loc.dir*t+rin_loc.from);
		#else
		const unsigned int
			x=xin+(int)(i*dxn),
			y=yin+(int)(i*dyn),
			z=zin+(int)(i*dzn);
		hashtype &plist=fetch(hashbox,x,y,z);
		#endif
	#elif SEEK_APPROACH==2 ///// draw a line hitting every box /////
	/*
	const int
		sgnx=sgn(rin_loc.dir.x),
		sgny=sgn(rin_loc.dir.y),
		sgnz=sgn(rin_loc.dir.z);
	*/
	int
		xp=xin-1,
		yp=yin-1,
		zp=zin-1;
		/*/
		xp=(sgnx<0)?hashbox.xdim:-1,
		yp=(sgny<0)?hashbox.ydim:-1,
		zp=(sgnz<0)?hashbox.zdim:-1;
		//*/
	vec3d state_tplus,state_tminus;

	auto findNextPlane=[&](){
		//compute the current t using last state's values
		//const double t=std::min(std::min(state_tplus.x,state_tplus.y),state_tplus.z);
		double t=(state_tplus.x>=0)?state_tplus.x:INFINITY;
		t=((state_tplus.y>=0) & (state_tplus.y<t))?state_tplus.y:t;
		t=((state_tplus.z>=0) & (state_tplus.z<t))?state_tplus.z:t;

		t=((state_tminus.x>=0) & (state_tminus.x<t))?state_tminus.x:t;
		t=((state_tminus.y>=0) & (state_tminus.y<t))?state_tminus.y:t;
		t=((state_tminus.z>=0) & (state_tminus.z<t))?state_tminus.z:t;
		//TODO: do i need to handle edge case?
		//*
		xp+=(t==state_tplus.x) - (t==state_tminus.x);
		yp+=(t==state_tplus.y) - (t==state_tminus.y);
		zp+=(t==state_tplus.z) - (t==state_tminus.z);
		/*/
		xp+=sgnx*(t==state_t.x);
		yp+=sgny*(t==state_t.y);
		zp+=sgnz*(t==state_t.z);
		//*/

		//compute next state and return the t where you are right now
		vec3d testvec=vec3d(xp,yp,zp)-pointTF(hashbox,rin_loc.from);
		state_tminus.x=testvec.x/rin_loc.dir.x;
		state_tminus.y=testvec.y/rin_loc.dir.y;
		state_tminus.z=testvec.z/rin_loc.dir.z;

		state_tplus.x=(testvec.x+1)/rin_loc.dir.x;
		state_tplus.y=(testvec.y+1)/rin_loc.dir.y;
		state_tplus.z=(testvec.z+1)/rin_loc.dir.z;
		return std::min(std::min(state_tplus.x,state_tplus.y),state_tplus.z);
	};
	double tlow=0;
	//find move xp,yp,zp to the entrance of the cube
	/*
	do{
		printf("seeking startpoint (%u,%u,%u)\n",xp,yp,zp);
		tlow=findNextPlane();
	}while(tlow<t0);
	//}while(tlow<t0 && xp<hashbox.xdim && yp<hashbox.ydim && zp<hashbox.zdim);
	*/

	printf("starting at (%u,%u,%u)\n",xp,yp,zp);

	while(tlow<=t1){
	//while(tlow<=t1 && xp<hashbox.xdim && yp<hashbox.ydim && zp<hashbox.zdim){
		hashtype &plist=fetch(hashbox,xp,yp,zp);
		tlow=findNextPlane();
		printf("testing: (%i,%i,%i)\t list size:%lu\n",xp,yp,zp,plist.size());
		//printf("testing: (%u,%u,%u)\t list size:%lu\n",xp,yp,zp,plist.size());
	#elif SEEK_APPROACH==3
	//gather list of all possible points
	vec3<unsigned int> linearr[9*(n+1)];
	unsigned int linesize=0;

	int
		xshift[]={-1,-1,-1,0,0,0,1,1,1},
		yshift[]={-1,0,1,-1,0,1,-1,0,1},
		zshift[]={ 0,0,0, 0,0,0, 0,0,0};

	if(n==std::abs(dx)){
		std::swap(xshift,zshift);
	}
	//if both of these ifs happen, it doesnt matter because either x or y can be zeroed in that case
	if(n==std::abs(dy)){
		std::swap(yshift,zshift);
	}

	//this is, a surprisingly big bottleneck
	//TODO: optimize this
	for(int i=0;i<=n;i++){
		const unsigned int
			x=xin+(int)(i*dxn),
			y=yin+(int)(i*dyn),
			z=zin+(int)(i*dzn);
		for(int j=0;j<9;j++){
			linearr[linesize].x=x+xshift[j];
			linearr[linesize].y=y+yshift[j];
			linearr[linesize].z=z+zshift[j];

			//only increase size if the current point is in bounds
			linesize+=
				(linearr[linesize].x<hashbox.xdim) &
				(linearr[linesize].y<hashbox.ydim) &
				(linearr[linesize].z<hashbox.zdim);
		}
	}

	for(int i=0;i<linesize;i++){
		hashtype &plist=fetch(hashbox,linearr[i].x,linearr[i].y,linearr[i].z);
	#endif
	#undef SEEK_APPROACH

		//printf("line is at (%u, %u, %u), list size is: %lu\n",x,y,z,plist.size());
		//if(plist.size()){printf("line is at (%u, %u, %u), list size is: %lu\n",x,y,z,plist.size());}
		//if(plist.size()){printf("<%f, %f, %f>*t+C\tline is at (%u, %u, %u), list size is: %lu\n",rin_loc.dir.x,rin_loc.dir.y,rin_loc.dir.z,x,y,z,plist.size());}

		for(int pos:plist){
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
			//printf("closest t: %f\n",closest.t);
			//if(closest.hit){printf("closest t: %f\n",closest.t);}
			//closest=itest.hit?itest:closest;
		}

		//if(closest.hit){break;}
	}
	#endif

	/*forget the above and intersect a single point at origin
	{
		point p;
		p.col=color(1,1,1);
		double tlow,thigh;
		hashbox.pointarr[closest.index=0]=p;
		closest.hit=intersect(p,r_in,tlow,thigh);
		//closest.hit=intersect(p,rin_loc,tlow,thigh);
		closest.t=(tlow>=0)?tlow:thigh;
	}
	//*/

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
	/*
	if(closest.hit){
		printf("final: t=%f\tin:<%f, %f, %f>*t + <%f, %f, %f>\tout:<%f, %f, %f>*t + <%f, %f, %f>\n",d,
			r_in.dir.x,r_in.dir.y,r_in.dir.z,r_in.from.x,r_in.from.y,r_in.from.z,
			r_out.dir.x,r_out.dir.y,r_out.dir.z,r_out.from.x,r_out.from.y,r_out.from.z
		);
	}
	//*/
	return r_out.hit=closest.hit;
}
