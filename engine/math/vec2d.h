#ifndef VEC2D_H_INCLUDED
#define VEC2D_H_INCLUDED
/*
This header outlines the struct vec2d
the vec2d struct provides many commontly used operators for 2d vectors
*/
namespace MG{
	struct vec2d{
		/*
		this union allows for a user to easily access the points as an array for efficency
		or as names for readability as the user chooses;
		*/
		union{
			struct{double x,y;};
			double xy[2];
		};

		vec2d():x(0),y(0){}
		vec2d(double x0,double y0):x(x0),y(y0){}
		vec2d(double v[2]):x(v[0]),y(v[1]){}

		vec2d operator +(const vec2d &v) const;
		vec2d operator -(const vec2d &v) const;
		vec2d& operator =(const vec2d &v);
		vec2d& operator +=(const vec2d &v);
		vec2d& operator -=(const vec2d &v);

		vec2d operator *(double a) const;
		vec2d operator /(double a) const;
		vec2d& operator *=(double a);
		vec2d& operator /=(double a);

		void normalize();
		vec2d getNormalized();

		double dot(const vec2d &v) const;
	};

	double abs(const vec2d &v);//returns the magnitude
	double abs2(const vec2d &v);//returns the magnitude squared
	double invabs(const vec2d &v);//returns 1/magnitude
	double invabs2(const vec2d &v);//returns 1/magnitude^2
}
//this is outside of the namespace because otherwise things refuse to compile
MG::vec2d operator *(double a,const MG::vec2d &v);
#endif // VEC2D_H_INCLUDED
