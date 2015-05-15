#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

/*
includes all relevant definitions and classes
the user should not need to specify any particular datatypes

all aspects of this engine must use the namespace MG
*/
#include "../2d/surface.h"
#include "../object/object.h"

namespace MG{
	class engine{
		//need to intelegently store map chunks
	public:
		surface screen;
		///TODO: determine need for alternate constructors
		engine();//these should simply be a call to init
		~engine();//i know i am going to need this at some point

		void init();//(re)initialize the engine.  this should initialize everything and free any objects that might have been in use

		void addToScene(obj *o);
		void removeFromScene(obj *o);
		void clearScene();
		void update();

		///TODO: figure out how to do event handling
		//bool getEvents()
	};
}

#endif // CORE_H_INCLUDED
