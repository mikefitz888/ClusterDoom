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
        render_manager = manager->getRenderManager();
        texture = manager->getResourceManager()->getTexture("basic_tower");
    }

	void BasicTower::render() {
		float mod = (200.0f - delete_queue) / 200.0f;
		int size = (int)(96.0f);
		//setActiveColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) 
		render_manager->setActiveColour(255, 255, 255, (char)(255.0 * mod));
        
		if (health <= 0) {
			render_manager->setActiveColour(255, 0, 0, (char)(255.0 * mod / 2));
		}
		texture->render(getXr(), getYr(), size, size);

		RenderUtils::render_circular_health(getXr(), getYr(), (int) health, (int) max_health, RenderUtils::colour_blend(Colour(0,255,0,255), Colour(255,0,0,255), health/max_health));
	}
}