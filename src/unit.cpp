#include "../include/unit.h"
#include "../include/tower.h"
#include "../include/ResourceManager.h"
#include "../include/gameobject.h"
#include "../include/VertexBuffer.h"
#include "../include/manager.h"
#include "../include/gamecontroller.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "../include/RenderUtils.h"
#include "../include/AnimatedTexture.h"

#include <stdexcept>


namespace unit {
    Unit::Unit(id_t id, TYPE unit_type, Manager* m) :
        GameObject(id, gameobject::TYPE::UNIT, unit_type, m) {}

    //Load textures and animations
    void Unit::init() {
        render_manager = manager->getRenderManager();

        texture = manager->getResourceManager()->getAnimatedTexture("robot_unit");
		deathTexture = manager->getResourceManager()->getAnimatedTexture("robot_death");

        red = manager->getResourceManager()->getTexture("red");

        shader = manager->getResourceManager()->getShader("default");

        unit_shadow = manager->getResourceManager()->getTexture("unit_shadow");
        unit_ice = manager->getResourceManager()->getTexture("ice_effect_frozen");
        
    }

    void Unit::renderBegin() {
        unit_shadow->render((int) getXr(), (int) getYr());
    }
    void Unit::render() {

        if (isUnderGlacialEffect())
        {
            render_manager->setActiveColour(graphics::Colour(77, 166, 255, 255));
        }
        glacial_effect_vis += ((float)this->glacialEffectModifier - glacial_effect_vis)*0.035f;

    }
    void Unit::renderGUI() {
        if (this->health_alpha > 0.0f) {
            //red->render(getXr() - 26, getYr() - 30, 0.4f*health / maxHealth, 0.07f, 0.0f, 0.0f);

            // Render health-bar
            int alpha = (int)(this->health_alpha*255.0f);
            this->render_manager->setActiveColour(graphics::Colour(96, 96, 96, alpha));
            red->render((int) getXr() - 16, (int) getYr() - 30, 32, 5);
            if (health > 0) {
                this->render_manager->setActiveColour(graphics::Colour(255, 255, 255, alpha));
                red->render((int) getXr() - 15, (int) getYr() - 29, (int)(32.0f * (float)(health_visual / maxHealth)), (int)3);
            }

            // Reset rendering colour
            this->render_manager->setActiveColour(graphics::Colour(255, 255, 255, 255));
        }
    }
    void Unit::release() {}

    void Unit::step() {

		if (dead) {
			return;
		}

        /*
        *  Description of Unit activity: 
        *  Move towards Base.
        *  If tower is in range and able to shoot: stop -> attack tower -> continue with step 1
        * 
        *  There should be a cooldown between shots so the unit can "stutter-step"
        */
        this->setSmoothingRate(0.15f);
        GameObject::step();

        // Health clamp
        this->health = glm::clamp(this->health, 0.0f, this->maxHealth);
        this->health_visual = glm::clamp(this->health_visual, 0.0f, this->maxHealth);
        this->health_visual += (this->health - this->health_visual)*0.05f;

        // Health display timer
        health_timer --;
        if (health_timer > 0.0f) {
            if (health_alpha < 1.0f) {
                health_alpha += 0.10f;
            }
        } else {
            if (health_alpha > 0.0f) {
                health_alpha -= 0.03f;
            }
        }

        auto& target = game_controller->getBase();

        if (!target && !mineIsTarget) {
            return; //NO TARGET
        }

        if (mineIsTarget && mine) { 
            this->setDestination(mine->getPosition(), unitSpeed);
            /*
                MINING FUNCTIONALITY
            */
            if (distanceTo(mine->getPosition()) < 100) {
                mine->takeResource(true);
            }
        } else {
            this->setDestination(target->getPosition(), unitSpeed);
        }

        render_facing = getDestination();//target->getPosition();

    }

    tower_ptr Unit::getNearestTower() const
    {
        auto towers = manager->getTowers();
        int minDist = INT_MAX;
        tower_ptr outTower = nullptr;

        int dist;
        for (auto tower : towers) {
            if (tower && tower->getHealth() > 0) {
                dist = distanceTo(smartpointers::dynamic_pointer_cast<GameObject>(tower));
                //printf("dist: %d, minDist: %d\n", dist, minDist);
                if (dist < minDist) {
                    outTower = tower;
                    minDist = dist;
                }
            }
        }

        if (!outTower) {
            return nullptr;
        }
        return outTower;
    }

    void Unit::attack(tower_ptr tower) {
        tower->attacked(this->getSharedPtr());
    }

    void Unit::attacked(gameobject_ptr aggressor) {
        this->attacked(aggressor, 1);
    }

    //Control attack interations, deliver gold on dealth
    void Unit::attacked(gameobject_ptr aggressor, float damage) {
        health -= damage / difficulty;
        if (damage > 0) {
            this->health_timer = 140;
            this->health_alpha = 1.0f;
        }
        if (health <= 0) {
			if (!dead) {
				deliverWealth(20);
				dead = true;
				animationProgress = 0.0f;
			}

        }
    }

    void Unit::heal(float health_delta)
    {
        health += health_delta;
        health = health > maxHealth ? maxHealth : health;
        if (health < maxHealth) {
            this->health_timer = 140;
            this->health_alpha = 1.0f;
        }
    }

    void Unit::deliverWealth(size_t amt) {
        //manager->getGameController()->addEfficiency(amt);
    }

	void Unit::renderAnimation(graphics::AnimatedTexture* texture, float size,  float animationSpeed, int numFrames, float sizeMod, float rotMod) {
		float rotation = (float)(atan2(render_facing.y - getYr(), render_facing.x - getXr()) - M_PI / 2) + rotMod;
		float finSize = size * sizeMod;
		animationProgress = animationProgress + animationSpeed;
		int m = 1;
		//if (this->getAtDestination() && !dead) m = 0;
		texture->render(m* (((int)animationProgress) % numFrames), (int)getXr(), (int)getYr(), finSize, finSize, rotation);
	}

	bool Unit::isDead() {
		return dead;
	}

    void Unit::targetMine(smartpointers::slave_ptr<ResourceMine> t_mine) {
        mine = t_mine;
        mineIsTarget = true;
    }

    void Unit::setDifficulty(float d) {
        difficulty = d;
    }

	/*void Unit::getPath(Point<int> target) {
		Point<int> start = this->position;
		gamecontroller::GameController* gc = this->manager->getGameController();
		int screenWidth = gc->getScreenWidth();
		int screenHeight = gc->getScreenHeight();
		//aStar(start, target, screenWidth, screenHeight, gc, path);
	}*/
}
