#ifndef VEC3D_H_INCLUDED
#define VEC3D_H_INCLUDED
/*
This header outlines the struct vec3
the vec3 struct provides many commontly used operators for 3d vectors
*/
#include <cmath>

namespace MG{
	template<typename T>
	struct vec3{
		/*
		this union allows for a user to easily access the points as an array for efficency
		or as names for readability as the user chooses;
		*/
		union{
			struct{T x,y,z;};
			T xyz[3];
		};

		vec3():x(0),y(0),z(0){}
		vec3(T x0,T y0,T z0):x(x0),y(y0),z(z0){}
		vec3(T v[3]):x(v[0]),y(v[1]),z(v[2]){}

		vec3 operator +(const vec3 &v) const{return vec3(x+v.x,y+v.y,z+v.z);}
		vec3 operator -(const vec3 &v) const{return vec3(x-v.x,y-v.y,z-v.z);}

		vec3& operator =(const vec3 &v){
			x=v.x;
			y=v.y;
			z=v.z;
			return *this;
		}
		vec3& operator +=(const vec3 &v){
			x+=v.x;
			y+=v.y;
			z+=v.z;
			return *this;
		}
		vec3& operator -=(const vec3 &v){
			x-=v.x;
			y-=v.y;
			z-=v.z;
			return *this;
		}

		vec3 operator *(T a) const{return vec3(x*a,y*a,z*a);}
		vec3 operator /(T a) const{return vec3(x/a,y/a,z/a);}
		vec3& operator *=(T a){
			x*=a;
			y*=a;
			z*=a;
			return *this;
		}
		vec3& operator /=(T a){
			x/=a;
			y/=a;
			z/=a;
			return *this;
		}

		void normalize(){
			const T d=1/std::sqrt(x*x+y*y+z*z);
			x*=d;
			y*=d;
			z*=d;
		}
		vec3 getNormalized() const{
			const T d=1/std::sqrt(x*x+y*y+z*z);
			return vec3(x*d,y*d,z*d);
		}

		T dot(const vec3 &v) const{return x*v.x+y*v.y+z*v.z;}
		vec3 cross(const vec3 &v) const{
			return vec3(
				y*v.z-z*v.y,
				z*v.x-x*v.z,
				x*v.y-y*v.x
			);
		}
	};

	//returns the magnitude
	template<typename T> T abs(const vec3<T> &v){return std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z);}
	//returns the magnitude squared
	template<typename T> T abs2(const vec3<T> &v){return v.x*v.x+v.y*v.y+v.z*v.z;}
	//returns 1/magnitude
	template<typename T> T invabs(const vec3<T> &v){return 1/std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z);}
	//returns 1/magnitude^2
	template<typename T> T invabs2(const vec3<T> &v){return 1/(v.x*v.x+v.y*v.y+v.z*v.z);}


	typedef vec3<float> vec3f;
	typedef vec3<double> vec3d;
	typedef vec3<long double> vec3l;
}
//this is outside of the namespace because otherwise things refuse to compile
template<typename T>
MG::vec3<T> operator *(T a,const MG::vec3<T> &v){return MG::vec3<T>(v.x*a,v.y*a,v.z*a);}
#endif // VEC3D_H_INCLUDED
