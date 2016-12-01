#ifndef TOWER_H
#define TOWER_H

#include "gameobject.h"

namespace manager {
	class Manager;
}

namespace tower {
	using graphics::IRenderable;
	using gameobject::GameObject;
	using gameobject::id_t;
	using manager::Manager;

	class Tower : public GameObject {
	public:
		inline Tower(id_t id, Manager* m) : GameObject(id, m) { }
		void init() override;
		void render() override;
		void renderGUI() override;
		void release() override;
	};
}

#endif //TOWER_H