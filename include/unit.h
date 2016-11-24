#ifndef UNIT_H
#define UNIT_H

#include "gameobject.h"

using gameobject::id_t;

namespace unit {
	using gameobject::GameObject;
	class Unit : public GameObject {
	public:
		inline Unit(id_t id) : GameObject(id) {}
	};
}

#endif //UNIT_H