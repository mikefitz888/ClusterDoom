#include "../../include/Towers/BasicTower.h"
#include "../../include/RenderUtils.h"
#include "../../include/ResourceManager.h"
#include "../../include/manager.h"

using graphics::RenderUtils;
using graphics::Colour;

namespace tower {
    BasicTower::BasicTower(id_t key, Manager* m) : Tower(key, TYPE::BASIC, m) {
        position.x = 200;
        position.y = 400;
    }

    void BasicTower::init(){

		Tower::init();

        render_manager = manager->getRenderManager();
        texture = manager->getResourceManager()->getTexture("blackTower");
    }

	void BasicTower::render() {
		Tower::render();
	}
}