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

		texture->render(getX(), getY(), 96, 96);
    }
}