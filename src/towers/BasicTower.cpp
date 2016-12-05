#include "../../include/Towers/BasicTower.h"

namespace tower {
	BasicTower::BasicTower(id_t key, Manager* m) : Tower(key, m) {
		position.x = 200;
		position.y = 400;
	}

	void BasicTower::init(){
		render_manager = manager->getRenderManager();
		texture = new sf::Texture();
		if(!texture->loadFromFile("Resources/Textures/Towers/Basic.jpg")){
			std::cout << "[ERROR] Could not load texture! (BasicTower)" << std::endl;
		}
	}

	void BasicTower::render(){
		
	}
}