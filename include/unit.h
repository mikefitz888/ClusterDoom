#ifndef UNIT_H
#define UNIT_H

#include "gameobject.h"

namespace manager {
	class Manager;
}

namespace unit {
	using gameobject::id_t;
	using gameobject::GameObject;
	using manager::Manager;

	class Unit : public GameObject {
	public:
		inline Unit(id_t id, Manager* m) : GameObject(id, m) {}
		void init() override;
		void render() override;
		void renderGUI() override;
		void release() override;
	};
}

#endif //UNIT_H