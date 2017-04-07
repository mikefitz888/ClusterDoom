#include "../../include/Towers/BombTower.h"
#include "../../include/RenderUtils.h"
#include "../../include/ResourceManager.h"
#include "../../include/manager.h"

using graphics::RenderUtils;
using graphics::Colour;

namespace tower {
    BombTower::BombTower(id_t key, Manager* m) : Tower(key, TYPE::BOMB, m) {
        position.x = 700;
        position.y = 400;
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

    void BombTower::step() {
        bomb_queue.erase(std::remove_if(bomb_queue.begin(), bomb_queue.end(), [this](const gameobject_ptr& bomb) -> bool {
            if (!bomb) return true;
            bomb->addVelocity(glm::vec2(-bomb->getVelocityX() / 20.f, -bomb->getVelocityY() / 20.f));
            return false;
        }), bomb_queue.end());

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
			float eff = 1.0f;
            if (eff == 0.f) return;
            //std::cout << current_target->getID() << " " << current_target->distanceTo(position) << "\n";
            //printf("(%f, %f) - (%f, %f)\n", position.x, position.y, (float)current_target->getX(), (float)current_target->getY());
            gameobject_ptr obj = game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_BOMB, Point<int>(getX(), getY()));
            float m = target[0].first / max_range;
            obj->setVelocity(m*(current_target->getX()-getX())/10, m*(current_target->getY()-getY())/10 );
			//obj->setDamage(damage);
            smartpointers::static_pointer_cast<ProjectileBomb>(obj)->setExplosionDamage((int) (3000 * eff));
            timer = cooldown;
            bomb_queue.push_back(obj);
            //bomb_queue[obj] = Point<float>(current_target->getX(), current_target->getY());
        }
    }
}