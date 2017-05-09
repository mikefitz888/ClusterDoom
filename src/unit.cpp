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

    void Unit::init() {
        render_manager = manager->getRenderManager();

        /*texture = new sf::Texture();
        if (!texture->loadFromFile("src/Resources/Textures/pawn.png")) {
            std::cout << "[ERROR] Could not load texture! (Tower)" << std::endl;
        }*/
        texture = manager->getResourceManager()->getAnimatedTexture("robot_unit");
		deathTexture = manager->getResourceManager()->getAnimatedTexture("robot_death");


        /*red = new sf::Texture();
        if (!red->loadFromFile("src/Resources/Textures/red.png")) {
            std::cout << "[ERROR] Could not load texture! (Tower)" << std::endl;
        }*/
        red = manager->getResourceManager()->getTexture("red");

        /*shader = render_manager->createShaderFromFile("src/Resources/Shaders/Render2D_vert.glsl", "src/Resources/Shaders/Render2D_frag.glsl");
        if (shader == nullptr) {
            std::cout << "[ERROR] FAILED TO LOAD SHADER (Tower)" << std::endl;
        }
        else {
            std::cout << "Loaded shader" << std::endl;
        }*/
        shader = manager->getResourceManager()->getShader("default");

        unit_shadow = manager->getResourceManager()->getTexture("unit_shadow");
        unit_ice = manager->getResourceManager()->getTexture("ice_effect_frozen");
       // vbuff = new graphics::VertexBuffer();
       // hpbar_buff = new graphics::VertexBuffer();

      //  vbuff->addQuad(-100.0f, -56.2f, 100.0f, 56.2f);
      //  hpbar_buff->addQuadRGBA(-32.0f, -2.0f, 32.0f, 2.0f, 1.0f, 0.0f, 0.0f, 1.0f);

      //  vbuff->freeze();
      //  hpbar_buff->freeze();
        
    }

    void Unit::renderBegin() {
        unit_shadow->render((int) getXr(), (int) getYr());
    }
    void Unit::render() {

        if (isUnderGlacialEffect())
        {
            render_manager->setActiveColour(graphics::Colour(77, 166, 255, 255));
        }
        glacial_effect_vis += (this->glacialEffectModifier - glacial_effect_vis)*0.035f;
        /*render_manager->setActiveShader(shader);
        render_manager->setTexture(texture);

        render_manager->setActiveColour(graphics::Colour(255, 255, 255, 255));
        glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(getX(), getY(), 0.0));
        render_manager->setWorldMatrix(transform);
        vbuff->render();

        render_manager->setTexture(red);
        render_manager->setActiveColour(graphics::Colour(255, 255, 255, 255));
        transform = glm::translate(glm::mat4(), glm::vec3(getX(), getY()-30.0f, 0.0f) );
        transform = glm::scale(transform, glm::vec3(health/maxHealth, 1.0f, 1.0f));
        render_manager->setWorldMatrix(transform);
        hpbar_buff->render();*/
    }
    void Unit::renderGUI() {
        if (this->health_alpha > 0.0f) {
            //red->render(getXr() - 26, getYr() - 30, 0.4f*health / maxHealth, 0.07f, 0.0f, 0.0f);

            // Base
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
                mine->takeResource();
            }
        } else {
            this->setDestination(target->getPosition(), unitSpeed);
        }

        render_facing = getDestination();//target->getPosition();

        //float distance = (target->getX() - getX())*(target->getX() - getX()) + (target->getY() - getY())*(target->getY() - getY());
        /*ivec2 destination = getDestination();
        int distance = DIST_SQ(getX(), destination.x, getY(), destination.y);*/
        /*if (distanceTo(target->getPosition()) < 100 && cooldown-- == 0) {
           attack(target);
           cooldown = 40;
        }*/
    }

    tower_ptr Unit::getNearestTower() const
    //Not actually implemented correctly, just prototype
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

    void Unit::attacked(gameobject_ptr aggressor, float damage) {
        health -= damage;
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

	/*void Unit::getPath(Point<int> target) {
		Point<int> start = this->position;
		gamecontroller::GameController* gc = this->manager->getGameController();
		int screenWidth = gc->getScreenWidth();
		int screenHeight = gc->getScreenHeight();
		//aStar(start, target, screenWidth, screenHeight, gc, path);
	}*/
}
