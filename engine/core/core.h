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
	///TODO: please stop doing this...
	typedef SDL_Event event;

	class engine{
		//need to intelegently store map chunks

		event evt;
		void (*evtProc)(event*)=0;
		bool isEventSync=true;//if this is false, then

		//window win;
	public:
		///TODO: figure this whole mess out
		/*
		surface hud,scene;//2d overlay and 3d render surface respectively
		bool showHud=false,showScene=true;
		/*/
		window win;
		//*/
		uint32_t bgCol=0;

		//the following are implemented in core.cpp
		///TODO: determine need for alternate constructors
		engine();//these should simply be a call to init
		~engine();//i know i am going to need this at some point

		void init();//(re)initialize the engine.  this should initialize everything and free any objects that might have been in use

		//the following are implemented in scene.cpp
		void addToScene(obj *o);
		void removeFromScene(obj *o);
		void clearScene();

		//the following are implemented in draw.cpp
		void update();

		//the following are implemented in events.cpp
		void setEventHandler(void (*eventProcessor)(event*));
		void setEventHandler(void (*eventProcessor)(event*),bool b);
		bool setEventAsync(bool b);//even if the internals are positive is synchronous, it seems more sensible from a user side to have the function do this
	};
}

#endif // CORE_H_INCLUDED
