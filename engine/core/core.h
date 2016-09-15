#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

/*
includes all relevant definitions and classes
the user should not need to specify includes for any particular datatypes

all aspects of this engine must use the namespace MG
*/
#include "../2d/surface.h"
#include "../object/object.h"
#include "../object/meta.h"
#include "event.h"
#include <thread>
#include <vector>

namespace MG{
	class engine:public event{
	private:
		///TODO: need to intelegently store map chunks
		std::vector<event*> events;
		bool isEventSync=true;//if this is false, then event checking is done in a separate thread
		void pollEvents();
		void threadedPolling();
		std::thread *evtThread;

		window win;
	public:
		surface hud,scene;//2d overlay and 3d render surface respectively
		bool showHud=true,showScene=true;
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
		void initWindow(int width,int height,int flags=SDL_WINDOW_SHOWN);
		void setTitle(const char* title);

		//the following are implemented in envineEvents.cpp
		int registerEvent(event* evt);//returns the array index
		void removeEvent(int index);//TODO: maybe createe one that takes a pointer instead
		bool setEventAsync(bool b);//even if the internals are positive is synchronous, it seems more sensible from a user side to have the function do this

		//from event class
		virtual void quit();

		//the following are implemented in io.cpp
		obj& newObj(meta *metadata=0);
		obj& newObjFromFile(std::string fname,meta *metadata=0);
		obj& newOctreeFromFile(std::string fname,meta *metadata=0);
	};
}

#endif // CORE_H_INCLUDED
