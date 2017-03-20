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
        render_manager = manager->getRenderManager();
        game_controller = manager->getGameController();
        texture = manager->getResourceManager()->getTexture("basic_tower");
    }

	void LaserTower::render() {
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

        if (current_target) {
            //std::cout << current_target->getID() << " " << current_target->distanceTo(position) << "\n";
            //printf("(%f, %f) - (%f, %f)\n", position.x, position.y, (float)current_target->getX(), (float)current_target->getY());
            gameobject_ptr obj1 = game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_LASER, Point<int>(getX(), getY()));
            gameobject_ptr obj2 = game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_LASER, Point<int>(getX(), getY()));
            
            auto dir = glm::normalize((current_target->getPosition() + current_target->getVelocity() * target[0].first/7.f) - obj1->getPosition() );

            auto sdir = glm::vec2(-dir.y, dir.x) * 10.f;
            obj1->setPosition(getPosition() + sdir);
            obj2->setPosition(getPosition() - sdir);

            obj1->setVelocity(dir * 7.f);
            obj2->setVelocity(dir * 7.f);

        }
    }
}