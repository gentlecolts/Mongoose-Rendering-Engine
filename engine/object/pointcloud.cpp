#include "pointcloud.h"
#include <cmath>
#include <algorithm>
#include <utility>
#include <array>
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
f(0)=f(1)=1
f(2)=f(3)=2
f(4)=f(5)=f(6)=f(7)=3
f(8)=f(9)=f(10)=4
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

inline int crazyDimCalc(const double &npoints){
	/*this function comes from the following:
	cost t = n*(a+b*S)
	where
	P = number of points
	Total cells C = d^3 = f(P)
	n = cell count = sqrt(3)*d
	S = points/cell = P/C = P/f(P)
	a = cost per cell
	b = cost per point

	=============== side note ===============
	using the above definitions, but excluding a and b, we get
	cost t = n*S

	which simplifies down to:
	d=sqrt(P*sqrt(3)/t)

	this accurately assures a near constant time lookup for uniformly
	distributed points, but apparently the number of cells iterated over
	causes this approach to actually be slower...
	accounting for a and b gives a forumla which is much closer to the emperical
	optimum cell count
	=========================================

	t = n*(a+b*S)
	t/sqrt(3) = a*f(P)^(1/3) + b*P*f(P)^(-2/3)
	t/sqrt(3) = a*d + b*P/d^2

	solving this for d gives us a horrible, nasty cubic
	but it turns out one of the root terms, with properly chosen values for a,b, and t
	should give an accurate model of what the optimal cell count should be
	*/

	//values for these constants were found through experimentation
	const double
		A=21,
		B=10.5*npoints,
		T=7,

		A2=A*A,
		T3=T*T*T,
		exp1=std::pow(3.,9./2)*A2*B-4*T3,
		X=std::cbrt(std::sqrt(B*exp1)/(2*std::pow(3.,9./4)*A2)-(exp1+2*T3)/(2*std::pow(3.,9./2)*A2*A)),

		d= X+T*T/(27*A2*X)+T/(std::pow(3.,3./2)*A);
	printf("exp1=%f\tX=%f\td=%f\n",exp1,X,d);
	return -d;
}

spacehash::spacehash(int boxcount,int sizeAssert){
	/*TODO:this may need some serious revision
	currently, the hashsize grows roughly
	2^(3*log2(x)/3) which is just x
	that isnt really ideal, the size of this array should grow roughly log2(x)

	consider perhaps
	bits=minbits(x)
	xb=yb=zb=bits/3; and divide it up as needed

	then just use xb/yb/zb as the actual x,y,z dimensions of the box
	this should give a growth of

	xb*yb*zb = (log2(x)/3)^3
	*/
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

	/*
	xdim=(1u<<xbits);
	ydim=(1u<<ybits);
	zdim=(1u<<zbits);

	xmask=xdim-1;
	ymask=ydim-1;
	zmask=zdim-1;

	hashsize=1u<<(xbits+ybits+zbits);
	/*/
	//xdim=(1<<minbits(xbits))*2;
	//ydim=(1<<minbits(ybits))*2;
	//zdim=(1<<minbits(zbits))*2;

	xdim=(std::max(xbits,2u)*3)/1;
	ydim=(std::max(ybits,2u)*3)/1;
	zdim=(std::max(zbits,2u)*3)/1;

	//xdim=ydim=zdim=1;
	//xdim=ydim=zdim=bits;
	const double rt3=std::sqrt(3.0);const int t=bits;
	//xdim=ydim=zdim=std::max(std::sqrt(boxcount*rt3/t),2.);

	//xmask=(1<<minbits(xdim))-1;
	//ymask=(1<<minbits(ydim))-1;
	//zmask=(1<<minbits(zdim))-1;

	xdim=ydim=zdim=std::max(crazyDimCalc(boxcount),2);

	if(sizeAssert>1){
		xdim=ydim=zdim=sizeAssert;
	}

	printf("for %i points using d=%i\n",boxcount,xdim);


	xmask=ymask=zmask=-1u;

	hashsize=xdim*ydim*zdim;
	//*/


	pointhash=new hashtype[hashsize];
}
spacehash::spacehash(int boxcount,point points[],int pointcount,int sizeAssert):spacehash(boxcount,sizeAssert){
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
	const unsigned int index=
		//((((x&space.xmask)<<space.ybits)|(y&space.ymask))<<space.zbits)|(z&space.zmask);
		std::min(x+space.xdim*(y+space.ydim*z),space.hashsize-1);
		//x+space.xdim*(y+space.ydim*z);

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

	//map all points to [0,1]
	pos=minp;
	scale=maxp-minp;

	for(int i=0;i<npoints;i++){
		pointarr[i]=points[i];

		//printf("adding point %i to pos (%f, %f, %f)\n",i,points[i].pos.x,points[i].pos.y,points[i].pos.z);
		hashtype &box=fetch(*this,points[i].pos);
		//printf("size of target vector is: %u\n",i,box.size());
		box.push_back(i);
		//printf("vector size is now: %lu\n\n",box.size());
	}

	/*debugging
	for(unsigned int x=0;x<xdim;x++){
		for(unsigned int y=0;y<ydim;y++){
			for(unsigned int z=0;z<zdim;z++){
				printf("(%u, %u, %u) has size: %lu\n",x,y,z,fetch(*this,x,y,z).size());
			}
		}
	}//*/
	printf("hash[%i] done...\n\tmy size is: <%f, %f, %f>\n\tmy \"min\" corner is <%f, %f, %f>\n",hashsize,scale.x,scale.y,scale.z,pos.x,pos.y,pos.z);
}

///// pointcloud stuff goes here /////

pointcloud::pointcloud(engine* e,point points[],int numpoints,int density,int sizeAssert):obj(e),hashbox(numpoints/density,points,numpoints,sizeAssert){
}
pointcloud::pointcloud(engine* e,metadata *meta,point points[],int numpoints,int density,int sizeAssert):obj(e,meta),hashbox(numpoints/density,points,numpoints,sizeAssert){
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
	//*
	if(tmin.x>tmax.x){std::swap(tmin.x,tmax.x);}
	if(tmin.y>tmax.y){std::swap(tmin.y,tmax.y);}
	if(tmin.z>tmax.z){std::swap(tmin.z,tmax.z);}
	/*/
	if(rin_loc.dir.x<0){std::swap(tmin.x,tmax.x);}
	if(rin_loc.dir.y<0){std::swap(tmin.y,tmax.y);}
	if(rin_loc.dir.z<0){std::swap(tmin.z,tmax.z);}
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

	unsigned int xin,yin,zin,xout,yout,zout;
	remap(hashbox,rin_loc.dir*t0+rin_loc.from,xin,yin,zin);
	remap(hashbox,rin_loc.dir*t1+rin_loc.from,xout,yout,zout);

	struct interholder{
		double t=INFINITY;
		int index=0;
		bool hit=false;
	} closest;

	std::vector<interholder> intersections;

	#define SEEK_APPROACH 1
	#define FETCH_PLIST 1

	#if SEEK_APPROACH==0 ///// check every cell /////
	//printf("hashsize: %i\n",hashbox.hashsize);
	for(int i=0;i<hashbox.hashsize;i++){
		//printf("%i\n",i);
		hashtype &plist=hashbox.pointhash[i];
	#elif SEEK_APPROACH==1
		//gather all cells and find the the closeset point
	const int
		dx=xout-xin,dy=yout-yin,dz=zout-zin,
		n=std::max(std::max(std::abs(dx),std::abs(dy)),std::abs(dz)),
		n2=n?n:1;//if n is zero, n2=1, otherwise n2=n

	#define POINT_CALC_TYPE 2

	#if POINT_CALC_TYPE==0
	const double
		dxn=n?(double)(dx)/n:0,
		dyn=n?(double)(dy)/n:0,
		dzn=n?(double)(dz)/n:0;
	#endif

	//printf("n: %i\t<dxn,dyn,dzn>: <%f, %f, %f>\n",n,dxn,dyn,dzn);
	//printf("n: %i\t<xin,yin,zin>: <%u, %u, %u>\t<xout,yout,zout>: <%u, %u, %u>\t<dxn,dyn,dzn>: <%f, %f, %f>\n",n,xin,yin,zin,xout,yout,zout,dxn,dyn,dzn);
	//gather list of all possible points
	#if FETCH_PLIST
	hashtype* linearr[9*(n+1)];
	#else
	vec3<unsigned int> linearr[9*(n+1)];
	#endif
	unsigned int linesize=0;

	#if 1
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
	//TODO: optimize this?
	//replacing i*dxn with (i*dx)/n is actually slower by a bit, dont do it
	for(int i=0;i<=n;i++){
		const unsigned int
		#if POINT_CALC_TYPE==0
			x=xin+(int)(i*dxn),//xin+i*dx/n
			y=yin+(int)(i*dyn),//yin+i*dy/n
			z=zin+(int)(i*dzn);//zin+i*dz/n
		#elif POINT_CALC_TYPE==1
			x=(n*xin+i*dx)/n2,//xin+i*dx/n
			y=(n*yin+i*dy)/n2,//yin+i*dy/n
			z=(n*zin+i*dz)/n2;//zin+i*dz/n
		#elif POINT_CALC_TYPE==2
			x=xin+(i*dx)/n2,//xin+i*dx/n
			y=yin+(i*dy)/n2,//yin+i*dy/n
			z=zin+(i*dz)/n2;//zin+i*dz/n
		#endif
		for(int j=0;j<9;j++){
			const unsigned int
				px=x+xshift[j],
				py=y+yshift[j],
				pz=z+zshift[j];
			#if FETCH_PLIST
			linearr[linesize]=&fetch(hashbox,px,py,pz);
			#else
			linearr[linesize].x=px;
			linearr[linesize].y=py;
			linearr[linesize].z=pz;
			#endif

			//only increase size if the current point is in bounds
			#define compfn ((px<hashbox.xdim) &&\
				(py<hashbox.ydim) &&\
				(pz<hashbox.zdim))

			linesize+=compfn;
			//if(compfn){++linesize;}
		}
	}
	#else
	auto x=xin,y=yin,z=zin;
	static unsigned int facemap[]={
		1,//xlow exit maps to xhigh entrance
		0,//xhigh exit maps to xlow entrance
		3,//ylow exit maps to yhigh entrance
		2,//yhigh exit maps to ylow entrance
		5,//zlow exit maps to zhigh entrance
		4,//zhigh exit maps to zlow entrance
		6//none maps to none, used for first point
	};
	auto lastt=t0;
	unsigned int lastplane=6;

	auto offset=hashbox.pos-rin_loc.from;
	auto lineptr=linearr;

	while(x!=xout && y!=yout && z!=zout){//if n==0, then this will never loop
		//add current box
		//linearr[linesize]=&fetch(hashbox,x,y,z);
		*lineptr=&fetch(hashbox,x,y,z);
		++lineptr;
		++linesize;

		//calculate next
		double ts[6]={
			(x+offset.x)/rin_loc.dir.x,(x+1+offset.x)/rin_loc.dir.x,
			(y+offset.y)/rin_loc.dir.y,(y+1+offset.y)/rin_loc.dir.y,
			(z+offset.z)/rin_loc.dir.z,(z+1+offset.z)/rin_loc.dir.z
		};

		auto mint=INFINITY;
		int mini=0;
		const unsigned int lasti=facemap[lastplane];
		for(unsigned int i=0;i<6;i++){
			if(i!=lasti && ts[i]>lastt && ts[i]<mint){
				mint=ts[i];
				mini=i;
			}
		}
		x+=(mini==1)-(mini==0);
		y+=(mini==3)-(mini==2);
		z+=(mini==5)-(mini==4);

		lastplane=mini;
		lastt=mint;
	}

	//add the end node
	linearr[linesize]=&fetch(hashbox,xout,yout,zout);
	++linesize;
	#endif

	//printf("%i\n",linesize);

	int counter=0;
	//*
	#if FETCH_PLIST
	#define ALLOW_BREAK 1
	//std::for_each(linearr,linearr+linesize,[&](hashtype *plist){
	for(hashtype** blahrg=linearr;blahrg<linearr+linesize;blahrg++){auto plist=*blahrg;
	#else
	std::for_each(linearr,linearr+linesize,[&](vec3<unsigned int> &i){
		hashtype &plist=fetch(hashbox,i.x,i.y,i.z);
	#endif
	/*/
	for(auto i=linearr;i<linearr+linesize;i++){
		hashtype &plist=fetch(hashbox,i->x,i->y,i->z);
	//*/
	#endif
	#undef SEEK_APPROACH

		//printf("line is at (%u, %u, %u), list size is: %lu\n",x,y,z,plist.size());
		//if(plist.size()){printf("line is at (%u, %u, %u), list size is: %lu\n",x,y,z,plist.size());}
		//if(plist.size()){printf("<%f, %f, %f>*t+C\tline is at (%u, %u, %u), list size is: %lu\n",rin_loc.dir.x,rin_loc.dir.y,rin_loc.dir.z,x,y,z,plist.size());}

		//std::array<interholder,plist.size()> arr;
		//for(){


		#define LOOP_APPROACH 1
		#if LOOP_APPROACH==0
		for(int pos:plist){
			interholder itest;
			double t0,t1;
			itest.index=pos;
			//note, t0 is always less than t1
			itest.hit=intersect(hashbox.pointarr[pos],rin_loc,t0,t1);
			itest.t=(t0>=0)?t0:t1;

			//if hit is true, then test_t must be positive, close_t starts out at INFINITY
			closest=(itest.hit & (itest.t<closest.t))?itest:closest;
		}
		//if(closest.hit){break;}
	}
		#elif LOOP_APPROACH==1
		#if FETCH_PLIST
		std::for_each(plist->begin(),plist->end(),[&closest,this,&rin_loc](int pos){
		#else
		std::for_each(plist.begin(),plist.end(),[&closest,this,&rin_loc](int pos){
		#endif
			interholder itest;
			double t0,t1;
			itest.index=pos;
			//note, t0 is always less than t1
			itest.hit=intersect(hashbox.pointarr[pos],rin_loc,t0,t1);
			itest.t=(t0>=0)?t0:t1;
			//itest.t=((!itest.hit) | (itest.t<0))?INFINITY:itest.t;

			//if hit is true, then test_t must be positive, close_t starts out at INFINITY
			closest=(itest.hit & (itest.t<closest.t))?itest:closest;
		});
		if(closest.hit){break;}
	}
		#elif LOOP_APPROACH==2
		//std::vector<interholder> inters;
		const int lsize=plist.size();
		interholder inters[lsize];
		//printf("transforming\n");
		std::transform(plist.begin(),plist.end(),inters,[&](int pos){
			interholder itest;
			double t0,t1;
			itest.index=pos;
			itest.hit=intersect(hashbox.pointarr[pos],rin_loc,t0,t1);
			itest.t=(t0>=0)?t0:t1;

			//have to do this, cant use test.hit in the min_element check or else it messes up
			itest.t=itest.hit?itest.t:INFINITY;
			return itest;
		});

		auto mintest=[](interholder &test,interholder &old){
			//return test.hit & (test.t<old.t);
			return (test.t<old.t);
		};

		auto closeptr=std::min_element(inters,inters+lsize,mintest);

		if(closeptr!=inters+lsize){
			closest=mintest(*closeptr,closest)?*closeptr:closest;
		}
		//if(closest.hit){break;}
	}
		#elif LOOP_APPROACH==3
		//printf("transforming\n");
		std::transform(plist.begin(),plist.end(),std::back_inserter(intersections),[&](int pos){
			interholder itest;
			double t0,t1;
			itest.index=pos;
			itest.hit=intersect(hashbox.pointarr[pos],rin_loc,t0,t1);
			itest.t=(t0>=0)?t0:t1;

			//have to do this, cant use test.hit in the min_element check or else it messes up
			itest.t=itest.hit?itest.t:INFINITY;
			return itest;
		});
		counter+=plist.size();
		//printf("added %i elements to intersection\n",plist.size());
	}
	//printf("intersection has %i elements, should be %i\n",intersections.size(),counter);
	//printf("went through %i cells and collected %i points\n",linesize,intersections.size());

	auto closeptr=std::min_element(intersections.begin(),intersections.end(),[](interholder &test,interholder &old){
		return (test.t<old.t);
	});

	if(closeptr!=intersections.end()){
		closest=*closeptr;
	}
		#endif
	//);

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

	return r_out.hit=closest.hit;
}
