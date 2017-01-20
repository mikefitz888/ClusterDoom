#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "gameobject.h"
#include "manager.h"

namespace gameobject {
	class GameObject;
}

namespace manager {
	class Manager;
}

namespace gamecontroller {
	using gameobject::GameObject;
	using gameobject::id_t;
	using gameobject::tower_ptr;
	using gameobject::unit_ptr;
	using gameobject::Point;
	using manager::Manager;
	class GameController {
		Manager* manager;
	public:
		GameController(Manager* m);
		GameObject* createObject(id_t key);
		void init() const;
		void step() const;
		void restart() const;
		tower_ptr spawnTowerAt(int x, int y) const;
		tower_ptr spawnTowerAt(Point<int> position) const;
		unit_ptr spawnUnitAt(int x, int y) const;
		unit_ptr spawnUnitAt(Point<int> position) const;
	};
}

#endif //GAMECONTROLLER_H