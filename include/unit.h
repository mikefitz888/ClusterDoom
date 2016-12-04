#ifndef UNIT_H
#define UNIT_H

#include "gameobject.h"

namespace manager {
	class Manager;
}

namespace unit {
	using gameobject::id_t;
	using gameobject::GameObject;
	using gameobject::tower_ptr;
	using gameobject::unit_ptr;
	using gameobject::Point;
	using manager::Manager;
	//using containers::optional;

	class Unit : public GameObject {
		sf::Texture* texture;
		sf::Shader* shader;
		graphics::VertexBuffer* vbuff;
	protected:
		tower_ptr getNearestTower();
		Point<float> velocity = Point<float>(0, 0);
	public:
		inline Unit(id_t id, Manager* m) : GameObject(id, m) {}
		//IRenderable methods
		void init() override;
		void render() override;
		void renderGUI() override;
		void release() override;

		//Gameplay methods
		void step();
	};
}

#endif //UNIT_H