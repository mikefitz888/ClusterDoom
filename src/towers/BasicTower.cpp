#include "../../include/tower.h"

namespace tower {
	class BasicTower : Tower {
		sf::Texture* texture = nullptr;
	public:
		BasicTower(id_t key, Manager* m);
		void init() override;
		void render() override;
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