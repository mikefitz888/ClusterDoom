#include "../../include/Towers/BombTower.h"
#include "../../include/RenderUtils.h"
#include "../../include/ResourceManager.h"
#include "../../include/manager.h"

using graphics::RenderUtils;
using graphics::Colour;

namespace tower {
    BombTower::BombTower(id_t key, Manager* m) : Tower(key, TYPE::BOMB, m) {
        this->setDepth(-5);
    }

    void BombTower::init(){

		Tower::init();

        render_manager = manager->getRenderManager();
        game_controller = manager->getGameController();
        texture = manager->getResourceManager()->getTexture("greenTower");
    }

	void BombTower::render() {

		Tower::render();

	}

    void BombTower::step() {
        //Simulates deceleration on the bomb, bomb is removed from vector once it explodes.
        bomb_queue.erase(std::remove_if(bomb_queue.begin(), bomb_queue.end(), [this](const gameobject_ptr& bomb) -> bool {
            if (!bomb) return true;
            bomb->addVelocity(glm::vec2(-bomb->getVelocityX() / 15.f, -bomb->getVelocityY() / 15.f));
            return false;
        }), bomb_queue.end());

        // Get efficiency
        float eff = game_controller->towerEfficiency(getPosition());
        disabled = (eff <= 0.0f);

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
            //float eff = requestEfficiency(1000, 100);
			
            if (eff <= 0.f) return;

            gameobject_ptr obj = game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_BOMB, Point<float>(getX(), getY()));
            float m = target[0].first / max_range;
            obj->setVelocity(m*(current_target->getX()-getX())/10, m*(current_target->getY()-getY())/10 );
			//obj->setDamage(damage);
            smartpointers::static_pointer_cast<ProjectileBomb>(obj)->setExplosionDamage((int) (3750 * eff));
            timer = cooldown;
            bomb_queue.push_back(obj);
            //bomb_queue[obj] = Point<float>(current_target->getX(), current_target->getY());
        }
    }
}