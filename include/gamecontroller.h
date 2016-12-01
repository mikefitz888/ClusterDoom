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
	using manager::Manager;
	class GameController {
		Manager* manager;
	public:
		GameController(Manager* m);
		GameObject* createObject(id_t key);
		void step();
	};
}

#endif //GAMECONTROLLER_H