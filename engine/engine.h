#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

/*
includes all relevant definitions and classes
the user should not need to specify any particular datatypes

all aspects of this engine must use the namespace MG
*/
#include "2d/surface.h"

namespace MG{
	class engine{
	public:
		surface screen;
		///TODO: determine necessary parameters
		engine();//these should simply be a call to init
		bool init();
	};
}

#endif // ENGINE_H_INCLUDED
