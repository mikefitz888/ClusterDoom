#include "../../include/Towers/ElectricTower.h"
#include "../../include/RenderUtils.h"
#include "../../include/ResourceManager.h"
#include "../../include/manager.h"

using graphics::RenderUtils;
using graphics::Colour;

namespace tower {
    ElectricTower::ElectricTower(id_t key, Manager* m) : Tower(key, TYPE::ELECTRIC, m) {
        position.x = 200;
        position.y = 400;
    }

    void ElectricTower::init(){
        render_manager = manager->getRenderManager();
        game_controller = manager->getGameController();
        texture = manager->getResourceManager()->getTexture("basic_tower");
    }

	void ElectricTower::render() {
		float mod = (200.0f - delete_queue) / 200.0f;
		int size = (int)(96.0f);
		//setActiveColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) 
		render_manager->setActiveColour(255, 255, 255, (int)(255.0 * mod));
		if (health <= 0) {
			render_manager->setActiveColour(255, 0, 0, (int)(255.0 * mod / 2));
		}
		texture->render(getXr(), getYr(), size, size);

		RenderUtils::render_circular_health(getXr(), getYr(), (int) health, (int) max_health, RenderUtils::colour_blend(Colour(0,255,0,255), Colour(255,0,0,255), health/max_health));
	}

    void ElectricTower::step() {
        if (timer) {
            timer--; 
            return;
        }
        
        //getUnits(1);
        auto units_nearby = this->manager->getGameController()->getNNearestUnits(this->position, 5, 1000);
        if (units_nearby.size() > 0) {
            std::cout << "ELECTRICITY!!" << std::endl;
            gameobject_ptr obj = game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_ELECTRICITY, Point<int>(getX(), getY()));
            smartpointers::slave_ptr<ProjectileElectricity> elec = smartpointers::static_pointer_cast<ProjectileElectricity>(obj);

            elec->setForkParent( this->getSharedPtr() );
            elec->setRange(1000);
            elec->setDamage( (requestEfficiency(cost_per_attack) * damage) );
            elec->setTargetObject(units_nearby[0].second);
            timer = cooldown;
        }
    }
}