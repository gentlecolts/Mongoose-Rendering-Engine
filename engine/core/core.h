#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

/*
includes all relevant definitions and classes
the user should not need to specify includes for any particular datatypes

all aspects of this engine must use the namespace MG
*/
#include "../2d/surface.h"
#include "../object/object.h"
#include "event.h"
#include "threadpool.h"
#include "../camera/camera.h"
#include <thread>
#include <vector>

namespace MG{
	struct sceneContainer{
		//TODO: need to intelligently store map chunks
		//TODO: need to write functions for retrieving objects, might need a secondary structure to store names, for example
		std::vector<obj*> objects;
		//std::vector<light*> lights;//TODO: need to implement lighting

		sceneContainer();

		//void renderTo(surface &target);
		void render(camera &cam);
	};

	class engine:public event{
	private:
		//TODO: vector may not be the best choice of structure, consider linked list or maybe even a tree for parallelism
		std::vector<event*> events;
		bool isEventSync=true;//if this is false, then event checking is done in a separate thread
		void pollEvents();
		void threadedPolling();
		std::thread *evtThread;

		window win;
		long updateTimestamp=0;

		sceneContainer scene;

		//implemented in draw.cpp
		//runs all staticUpdate and threadedUpdate calls
		void updateTasks(int id,int numThreads);
	public:
		camera mainCamera;
		surface hud,render;//2d overlay and 3d render surface respectively
		bool showHud=true,showScene=true;
		uint32_t bgCol=0;

		//implemented in core.cpp
		///TODO: determine need for alternate constructors
		engine();//these should simply be a call to init
		~engine();//i know i am going to need this at some point

		void init();//(re)initialize the engine.  this should initialize everything and free any objects that might have been in use

		//implemented in scene.cpp
		void addToScene(obj *o);
		void removeFromScene(obj *o);
		void clearScene();

		//implemented in draw.cpp
		bool isTimeToUpdate(int FPS);
		void update();
		void initWindow(int width,int height,int flags=SDL_WINDOW_SHOWN);
		void setTitle(const char* title);

		//implemented in envineEvents.cpp
		int registerEvent(event* evt);//returns the array index
		void removeEvent(int index);//TODO: maybe createe one that takes a pointer instead
		bool setEventAsync(bool b);//even if the internals are positive is synchronous, it seems more sensible from a user side to have the function do this
		virtual void quit();//from event class
	};
}

#endif // CORE_H_INCLUDED
