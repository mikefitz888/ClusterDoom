#include "../../include/Towers/Base.h"
#include "../../include/ResourceManager.h"
#include "../../include/manager.h"
#include "../../include/AnimatedTexture.h"
#include "../../include/RenderUtils.h"

namespace tower {
	Base::Base(id_t key, Manager* m) : Tower(key, TYPE::BASE, m) {

	}

	void Base::init() {

		Tower::init();

		render_manager = manager->getRenderManager();
		texture = manager->getResourceManager()->getTexture("base_image");
        game_controller = manager->getGameController();
        health = max_health;
        exuhporoshun[gameobject::TYPE::OBJECT][gameobject::OBJECT_TYPE::PROJECTILE_LASER] = manager->getResourceManager()->getAnimatedTexture("explosion4");
        //exuhporoshun[unit::TYPE::WIZARD] = manager->getResourceManager()->getAnimatedTexture("explosion2");
		//Get Screen dims and position in center
        this->collision_profile.setTypeCircle(180);

	}

	void Base::render() {

		int size = 100;
		//texture->render((int) getXr(), (int) getYr(), size, size);
        animations.erase(std::remove_if(animations.begin(), animations.end(), [this](std::pair<graphics::AnimatedTexture*, std::pair<float, Point<int>>>& a)->bool {
            if (a.second.first > a.first->getTotalFrames()) return true;
            a.second.first += 1.0f;
            a.first->render((int)a.second.first, a.second.second.x, a.second.second.y, 0.5f, 0.5f);
            return false;
        }), animations.end());
        graphics::RenderUtils::render_circular_health((int)getXr(), (int)getYr(), (int)health, (int)max_health,
            graphics::RenderUtils::colour_blend(graphics::Colour(0, 255, 0, 255), graphics::Colour(255, 0, 0, 255), health / max_health)
        );
	}

    void Base::step() {
        this->collision_profile.setTypeSquare(-75, 75, -75, 75);
        this->collision_profile.setCollidable(true);
    }

    void Base::attacked(gameobject_ptr aggressor) {
        //gameobject::OBJECT_TYPE::PROJECTILE_LASER
        if (exuhporoshun.find(aggressor->getSuperType()) != exuhporoshun.end() ) {
            //exuhporoshun[(unit::TYPE) aggressor->getSubType()]->render()
            if ( exuhporoshun[aggressor->getSuperType()].find(aggressor->getSubType()) != exuhporoshun[aggressor->getSuperType()].end() ) {
                auto r = glm::linearRand(-glm::vec2(20), glm::vec2(20));
                auto pair = std::make_pair<float, Point<int>>(0.f, Point<int>((int) getX() + (int) r.x, (int) getY() + (int) r.y));
                auto pair2 = std::make_pair(exuhporoshun[aggressor->getSuperType()][aggressor->getSubType()], pair);
                animations.push_back(pair2);
            }
        }
    }

    void Base::attacked(gameobject_ptr aggressor, float damage) {
        attacked(aggressor);
        health -= damage;
        if (health <= 0) {
            game_controller->loseGame();
        }
    }
}