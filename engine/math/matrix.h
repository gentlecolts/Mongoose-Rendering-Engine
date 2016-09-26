#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

namespace MG{
	struct matrix3{
		//allow for the whole array to be accesed as either one array or as its vectors
		//note that this means the array must be interpreted as a transpose of the matrix it represents
		union {
			double xyz[9];
			struct{double x[3],y[3],z[3];};
		};
	};
}

#endif // MATRIX_H_INCLUDED
