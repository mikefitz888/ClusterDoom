#ifndef CVINTERFACE_H
#define CVINTERFACE_H

#include "gamecontroller.h"
#include "gameobject.h"
namespace manager {
	class Manager;
}

namespace cvinterface {
	using gamecontroller::GameController;
	using gameobject::Point;
	using gameobject::tower_ptr;
	class ICVInterface {
		GameController* game_controller;
		std::vector<Point<int>> tower_locations;
		std::vector<tower_ptr> towers; //Up date this list to ensure a correspondance between it and tower_locations
	public:
		inline ICVInterface(GameController* gc) : game_controller(gc) {}
		void init();
		void release();
		void updateTowerList(std::vector<Point<int>> &towers);
	};
}


#endif //CVINTERFACE_H