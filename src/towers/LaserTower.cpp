#include "../../include/Towers/LaserTower.h"
#include "../../include/RenderUtils.h"
#include "../../include/ResourceManager.h"
#include "../../include/manager.h"

using graphics::RenderUtils;
using graphics::Colour;

namespace tower {
    LaserTower::LaserTower(id_t key, Manager* m) : Tower(key, TYPE::LASER, m) {
        position.x = 700;
        position.y = 400;
        this->setDepth(-5);
    }

    void LaserTower::init(){

		Tower::init();

        render_manager = manager->getRenderManager();
        game_controller = manager->getGameController();
        texture = manager->getResourceManager()->getTexture("redTower");
        max_power = 100.0f;
    }

	void LaserTower::render() {

		Tower::render();
		texture->render(getXr(), getYr(), 96, 96);

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

    void LaserTower::step() {
        if (timer) {
            timer--; 
            return;
        }
        
        //getUnits(1);
        //if (!current_target || current_target->distanceTo(position) > max_range) {
        auto target = this->manager->getGameController()->getNNearestUnits(this->position, 1, max_range);
        if (!target.size()) return;

        current_target = target[0].second;

        //std::cout << "ELECTRICITY!!" << std::endl;

        //float power = requestEfficiency(30.f /*Maximum power*/, 10.f /*minimum power*/);
		float power = 1.0f;
        if (power == 0.0f) return;

        if (current_target) {
            //std::cout << current_target->getID() << " " << current_target->distanceTo(position) << "\n";
            //printf("(%f, %f) - (%f, %f)\n", position.x, position.y, (float)current_target->getX(), (float)current_target->getY());
			gameobject_ptr obj = game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_LASER, Point<int>(getX(), getY()));

			auto dir = glm::normalize((current_target->getPosition() + current_target->getVelocity() * target[0].first / 7.f) - obj->getPosition());
			auto sdir = glm::vec2(-dir.y, dir.x) * 7.0f;
			if (leftFire) {
				obj->setPosition(getPosition() + sdir);
				leftFire = false;
			}
			else {
				obj->setPosition(getPosition() - sdir);
				leftFire = true;
			}
			//dir = glm::normalize((current_target->getPosition() + current_target->getVelocity() * target[0].first / 7.f) - obj->getPosition());
			obj->setVelocity(dir * 9.f);
            smartpointers::static_pointer_cast<ProjectileLaser>(obj)->setDamage(power*150);
			timer = cooldown;
        }
    }
}