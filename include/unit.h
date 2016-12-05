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

	enum TYPE {BASE, BASIC};

	class Unit : public GameObject {
		sf::Texture* texture;
		sf::Shader* shader;
		graphics::VertexBuffer* vbuff;
	protected:
		tower_ptr getNearestTower() const;
		Point<float> velocity = Point<float>(0, 0);
		float health = 100;
	public:
		inline Unit(id_t id, Manager* m) : GameObject(id, m) {}
		//IRenderable methods
		virtual void init() override;
		virtual void render() override;
		virtual void renderGUI() override;
		virtual void release() override;

		//Gameplay methods
		virtual void step() override;
		virtual void attack(tower_ptr tower);
		virtual void attacked(GameObject* aggressor);
	};
}

#endif //UNIT_H