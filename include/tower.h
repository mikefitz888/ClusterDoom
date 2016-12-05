#ifndef TOWER_H
#define TOWER_H

#include "gameobject.h"
#include "manager.h"

namespace manager {
	class Manager;
}

namespace tower {
	//using containers::optional;
	using graphics::IRenderable;
	using gameobject::GameObject;
	using gameobject::id_t;
	using gameobject::Point;
	using gameobject::tower_ptr;
	using gameobject::unit_ptr;
	using manager::Manager;

	enum TYPE {BASE, BASIC};

	class Tower : public GameObject {
		sf::Texture* texture;
		sf::Texture* red;
		sf::Shader* shader;
		graphics::VertexBuffer* vbuff;
		graphics::VertexBuffer* hpbar_buff;

		float health = 100;
	public:
		inline Tower(id_t id, Manager* m) : GameObject(id, m) { }
		virtual void init() override;
		virtual void render() override;
		virtual void renderGUI() override;
		virtual void release() override;

		//Gameplay methods
		inline float getHealth() const { return health; }

		virtual void attack(unit_ptr tower); //Tower attacks unit
		virtual void attacked(unit_ptr tower); //Unit attacks tower
	};
}

#endif //TOWER_H
