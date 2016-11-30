#ifndef TOWER_H
#define TOWER_H

#include "gameobject.h"

namespace tower {
	using gameobject::GameObject;
	using gameobject::id_t;
	class Tower : public GameObject {
	public:
		inline Tower(id_t id) : GameObject(id) { }
	};
}

#endif //TOWER_H