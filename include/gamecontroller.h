#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "gameobject.h"

namespace gameobject {
	class GameObject;
}

namespace gamecontroller {
	using gameobject::GameObject;
	using gameobject::id_t;
	class GameController {
	public:
		inline GameController(){}
		GameObject* createObject(id_t key);
	};
}

#endif //GAMECONTROLLER_H