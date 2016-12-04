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
	using manager::Manager;

	class Tower : public GameObject {
		sf::Texture* texture;
		sf::Shader* shader;
		graphics::VertexBuffer* vbuff;

		float health = 100;
	public:
		inline Tower(id_t id, Manager* m) : GameObject(id, m) { }
		void init() override;
		void render() override;
		void renderGUI() override;
		void release() override;

		//Gameplay methods
		inline float getHealth() { return health; }
	};
}

#endif //TOWER_H
