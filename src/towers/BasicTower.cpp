#include "../../include/tower.h"

namespace tower {
	class BasicTower : Tower {
		sf::Texture* texture;
	public:
		BasicTower(id_t key, Manager* m);
		void init();
		void render();
	};

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