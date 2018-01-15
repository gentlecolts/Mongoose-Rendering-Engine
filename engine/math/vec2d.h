#ifndef VEC2D_H_INCLUDED
#define VEC2D_H_INCLUDED
/*
This header outlines the struct vec2
the vec2 struct provides many commontly used operators for 2d vectors
*/
#include <cmath>
namespace MG{
	template<typename T>
	struct vec2{
		/*
		this union allows for a user to easily access the points as an array for efficency
		or as names for readability as the user chooses;
		*/
		union{
			struct{T x,y;};
			T xy[2];
		};

		vec2():x(0),y(0){}
		vec2(T x0,T y0):x(x0),y(y0){}
		vec2(T v[2]):x(v[0]),y(v[1]){}

		vec2 operator +(const vec2 &v) const{return vec2(x+v.x,y+v.y);}
		vec2 operator -(const vec2 &v) const{return vec2(x-v.x,y-v.y);}
		vec2 operator *(const vec2 &v) const{return vec2(x*v.x,y*v.y);}
		vec2 operator /(const vec2 &v) const{return vec2(x/v.x,y/v.y);}

		vec2& operator =(const vec2 &v){
			x=v.x;
			y=v.y;
			return *this;
		}
		vec2& operator +=(const vec2 &v){
			x+=v.x;
			y+=v.y;
			return *this;
		}
		vec2& operator -=(const vec2 &v){
			x-=v.x;
			y-=v.y;
			return *this;
		}

		vec2 operator *(T a) const{return vec2(x*a,y*a);}
		vec2 operator /(T a) const{return vec2(x/a,y/a);}
		vec2& operator *=(T a){
			x*=a;
			y*=a;
			return *this;
		}
		vec2& operator /=(T a){
			x/=a;
			y/=a;
			return *this;
		}

		//note that std::hypot has increased precision, but may come at a performance cost
		void normalize(){
			const T d=1/std::sqrt(x*x+y*y);
			//const T d=1/std::hypot(x,y);
			x*=d;
			y*=d;
		}
		vec2 getNormalized() const{
			const T d=1/std::sqrt(x*x+y*y);
			//const T d=1/std::hypot(x,y);
			return vec2(x*d,y*d);
		}

		T dot(const vec2 &v) const{return x*v.x+y*v.y;}
	};

	//returns the magnitude
	template<typename T> T abs(const vec2<T> &v){
		return std::sqrt(v.x*v.x+v.y*v.y);
		//return std::hypot(v.x,v.y);
	}
	//returns the magnitude squared
	template<typename T> T abs2(const vec2<T> &v){return v.x*v.x+v.y*v.y;}

	//returns 1/magnitude
	template<typename T> T invabs(const vec2<T> &v){
		return 1/std::sqrt(v.x*v.x+v.y*v.y);
		//return 1/hypot(v.x,v.y);
	}
	//returns 1/magnitude^2
	template<typename T> T invabs2(const vec2<T> &v);

	typedef vec2<float> vec2f;
	typedef vec2<double> vec2d;
	typedef vec2<long double> vec2l;
}
//this is outside of the namespace because otherwise things refuse to compile
template<typename T>
MG::vec2<T> operator *(T a,const MG::vec2<T> &v){return MG::vec2<T>(v.x*a,v.y*a);}
#endif // VEC2D_H_INCLUDED
