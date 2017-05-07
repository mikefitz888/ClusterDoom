#include "../../include/Towers/ElectricTower.h"
#include "../../include/RenderUtils.h"
#include "../../include/ResourceManager.h"
#include "../../include/manager.h"
#include "../../include/AudioManager.h"

using graphics::RenderUtils;
using graphics::Colour;

namespace tower {
    ElectricTower::ElectricTower(id_t key, Manager* m) : Tower(key, TYPE::ELECTRIC, m) {
        this->setDepth(-5);
    }

    void ElectricTower::init(){

		Tower::init();

        render_manager = manager->getRenderManager();
        game_controller = manager->getGameController();
        texture = manager->getResourceManager()->getTexture("blueTower");
    }

	void ElectricTower::render() {

		Tower::render();
		texture->render((int) getXr(), (int) getYr(), 96, 96);

		/*float mod = (200.0f - delete_queue) / 200.0f;
		int size = (int)(96.0f);
		//setActiveColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) 
		render_manager->setActiveColour(255, 255, 255, (char)(255.0 * mod));
		if (health <= 0) {
			render_manager->setActiveColour(255, 0, 0, (char)(255.0 * mod / 2));
		}
		texture->render(getXr(), getYr(), size, size);

		RenderUtils::render_circular_health(getXr(), getYr(), (int) health, (int) max_health, RenderUtils::colour_blend(Colour(0,255,0,255), Colour(255,0,0,255), health/max_health));
		*/
	}

    void ElectricTower::step() {

        // Play electricity sound when attacking
        if (current_target && sound_cooldown <= 0) {
            this->manager->getAudioManager()->playSound("robot_death");
            sound_cooldown = 10;
        }

        if (timer) {
            timer--; 
            return;
        }
        
        //getUnits(1);
        if (!current_target || (unsigned) current_target->distanceTo(position) > max_range) {
            auto units_nearby = this->manager->getGameController()->getNNearestUnits(this->position, 1, max_range);
            if (units_nearby.size()) current_target = units_nearby[0].second;
            else current_target = nullptr;
        }

        //std::cout << "ELECTRICITY!!" << std::endl;
        sound_cooldown--;
        if (current_target) {
            //float eff = requestEfficiency(cost_per_attack, 2.f);
			float eff = game_controller->towerEfficiency(getPosition());
            //std::cout << current_target->getID() << " " << current_target->distanceTo(position) << "\n";
            //printf("(%f, %f) - (%f, %f)\n", position.x, position.y, (float)current_target->getX(), (float)current_target->getY());
            gameobject_ptr obj = game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_ELECTRICITY, Point<float>(getX(), getY()));
            smartpointers::slave_ptr<ProjectileElectricity> elec = smartpointers::static_pointer_cast<ProjectileElectricity>(obj);

            elec->setForkParent(this->getSharedPtr());
            elec->setRange(max_range);
            elec->setDamage((int)(eff * 20.0f));
            elec->setTargetObject(current_target);
			//elec->setDamage(damage);
            timer = cooldown;
            
        }


    }
}