#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

/*
includes all relevant definitions and classes
the user should not need to specify any particular datatypes

all aspects of this engine must use the namespace MG
*/
#include <SDL2/SDL.h>
#include "2d/surface.h"

namespace MG{
	class engine{
	public:
		surface screen;
		///TODO: determine need for alternate constructors
		engine();//these should simply be a call to init
		~engine();//i know i am going to need this at some point

		void init();//(re)initialize the engine.  this should initialize everything and free any objects that might have been in use
		void initScreen(int width,int height,int flags=-1);

		void update();
	};
}

#endif // ENGINE_H_INCLUDED
