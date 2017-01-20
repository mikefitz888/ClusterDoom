#include "../../include/Towers/BasicTower.h"

namespace tower {
	BasicTower::BasicTower(id_t key, Manager* m) : Tower(key, TYPE::BASIC, m) {
		position.x = 200;
		position.y = 400;
	}

	void BasicTower::init(){
		render_manager = manager->getRenderManager();
		texture = manager->getResourceManager()->getTexture("basic_tower");
	}

	void BasicTower::render(){

		glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(getX(), getY(), 0.0));
		render_manager->setWorldMatrix(transform);
		texture->render();
	}
}