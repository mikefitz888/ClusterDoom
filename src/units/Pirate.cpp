#include "../../include/Units/Pirate.h"
#include "../../include/ResourceManager.h"
#include "../../include/gameobject.h"
#include "../../include/manager.h"
#include "../../include/gamecontroller.h"
#include "../../include/GameObjects/Projectiles.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "../../include/RenderUtils.h"
#include "../../include/AnimatedTexture.h"
#include "../../include/AudioManager.h"

namespace unit {
    Pirate::Pirate(id_t key, Manager* m) : Unit(key, TYPE::PIRATE, m)  {

    }

    Pirate::~Pirate() {
        int rand_res = rand() % 2;
        if (rand_res == 0) {
            this->manager->getAudioManager()->playSound("pirate_death_1");
        } else if (rand_res == 1) {
            this->manager->getAudioManager()->playSound("pirate_death_2");
        }
    }

    void Pirate::init(){

		Unit::init();

		collision_profile.setTypeCircle(14);
        render_manager = manager->getRenderManager();
        texture        = manager->getResourceManager()->getAnimatedTexture("pirate_unit");
		deathTexture = manager->getResourceManager()->getAnimatedTexture("pirate_death");
		attackTexture = manager->getResourceManager()->getAnimatedTexture("pirate_attack");
        this->setDistanceThreshold(95);

		health = 3000;
		maxHealth = 3000;
		unitSpeed = 0.4f+glm::linearRand(0.0f, 0.35f);

       /* Path path;
        path.push_back(vec2(100, 100));
        path.push_back(vec2(500, 50));
        path.push_back(vec2(250, 300));
        path.push_back(vec2(1280, 720));
        this->setPath(path, 10);*/
    }

    void Pirate::step() {

		if (dead) {
			if (animationProgress >= 15.0f) {
				destroySelf();
			}
			else {
				return;
			}
		}

        // Perform parent step
        Unit::step();

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
            if (!pressed) {
                Path _path;
                _path.clear();
                sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                if (manager->getGameController()->getPath(ivec2(render_position.x, render_position.y), vec2(mouse_pos.x, mouse_pos.y), _path)) {
                    this->clearPath();
                    this->setPath(_path, 2);

                    std::cout << "NEW PATH: LENGTH: " << _path.size() << std::endl;
                    for (auto a : _path) {
                        std::cout << "  (" << a.x << "," << a.y << ")" << std::endl;
                    }
                }
                pressed = true;
            }
        } else {
            pressed = false;
        }

        // Check path
        if (this->getPathComplete()) {
            this->restartPath();
        }

        // Network timers
        /*
            Send the position every few steps
        */

        // TEMP: Temporarily disabling the basic unit update, as it doesn't really serve a purpose atm
        /*network_update_position_timer--;
        if (network_update_position_timer <= 0) {
            network_update_position_timer = network_update_position_timer_max;
            this->sendNetworkUpdate(BasicUnitUpdateEvents::SEND_POSITION);
        }*/
        
        auto& base = game_controller->getBase();
        if (!this->isUnderGlacialEffect()) {
            if (distanceTo(base->getPosition()) < 180 && cooldownGun-- == 0) {
                //gameobject_ptr obj1 = game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_LASER, Point<int>(getX(), getY()));
                //smartpointers::static_pointer_cast<unit::Unit>(other);
                smartpointers::slave_ptr<ProjectileLaser> obj1 = smartpointers::static_pointer_cast<ProjectileLaser>(game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_LASER, Point<float>(getX(), getY())));
                obj1->textureName = "musketShot";
                obj1->setCollisionType(gameobject::TYPE::TOWER);
                //std::cout << "unit fired\n";
                auto dir = glm::normalize((base->getPosition()) - obj1->getPosition());
                obj1->setVelocity(dir * 7.f);
                //auto sdir = glm::vec2(-dir.y, dir.x) * 10.f;
                //obj1->setPosition(getPosition() + sdir);
                //obj2->setPosition(getPosition() - sdir);


                //attack(base);
                cooldownGun = 420;
            }
            if (distanceTo(base->getPosition()) < 100 && cooldown-- == 0) {
                //gameobject_ptr obj1 = game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_LASER, Point<int>(getX(), getY()));
                //smartpointers::static_pointer_cast<unit::Unit>(other);
                smartpointers::slave_ptr<ProjectileLaser> obj1 = smartpointers::static_pointer_cast<ProjectileLaser>(game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_LASER, Point<float>(getX(), getY())));
                obj1->textureName = "meleeEffect";
                obj1->setCollisionType(gameobject::TYPE::TOWER);
                //std::cout << "unit fired\n";
                auto dir = glm::normalize((base->getPosition()) - obj1->getPosition());
                obj1->setVelocity(dir * 1.f);
                //auto sdir = glm::vec2(-dir.y, dir.x) * 10.f;
                //obj1->setPosition(getPosition() + sdir);
                //obj2->setPosition(getPosition() - sdir);


                //attack(base);
                cooldown = 80;
            }
        }
    }

    void Pirate::render() {

		Unit::render();

		if (!dead) {
			auto& base = game_controller->getBase();
			if (distanceTo(base->getPosition()) < 100) {
				renderAnimation(attackTexture, 0.18f, 0.36f*this->glacial_effect_vis, 12, 1.0f, 0.0f);
			}
			else {
				renderAnimation(texture, 0.22f, 0.3f*this->glacial_effect_vis, 12, 1.0f, 0.0f);
			}
		}
		else {
			renderAnimation(deathTexture, 0.15f, 0.36f, 16, 1.0f, 0.0f);
		}

        /*render_manager = manager->getRenderManager();


        float rotation = (float)(atan2(render_facing.y - getYr(), render_facing.x - getXr()) - M_PI / 2);
		float animationSpeed = 0.3f;
        animation_progress = animation_progress + animationSpeed;
        int m = 1;
        if (this->getAtDestination()) m = 0;

       
        texture->render(m*((int)animation_progress) % 12, (int) getXr(), (int) getYr(), 0.22f, 0.22f, rotation);*/

        // ******************************************************************************************************//
        // DRAW PATH
        /*graphics::Texture* _red = manager->getResourceManager()->getTexture("red");
        Path _path = this->getPath();
        if (this->getFollowingPath() && _path.size() > 0) {
            for (unsigned int n = 0; n < _path.size() - 1; n++) {
                vec2 n1 = _path[n];
                vec2 n2 = _path[n + 1];
                graphics::RenderUtils::draw_line(n1.x, n1.y, n2.x, n2.y, 3, graphics::Colour(255,255,255,255));
                _red->render(n2.x, n2.y, 0.10f, 0.10f, 0.0f);
            }
        }*/
        // ******************************************************************************************************//

        if (this->glacial_effect_vis > 0.0f) {
            float factor = (1.0f - this->glacial_effect_vis);
            render_manager->setActiveColour(graphics::Colour(255, 255, 255, 200 * factor));
            unit_ice->render(getXr(), getYr(), 0.35f*factor, 0.35f*factor, 0.0f);
            render_manager->setActiveColour(graphics::Colour(255, 255, 255, 255));
        }
    }

	void Pirate::onCollision(gameobject_ptr other) {
		// Push away from other objects:

		switch (other->getSuperType()) {
			// COLLISION WITH OTHER INSTANCES
			case gameobject::TYPE::UNIT: {
                // TODO: Should these be floats?
				int x, y;
				x = (int) getX();
				y = (int) getY();

				glm::vec2 direction = glm::vec2(other->getPosition().x - x,
					other->getPosition().y - y);
				direction = glm::normalize(direction);

				x -= (int) (direction.x * 5);
				y -= (int) (direction.y * 5);
				setX((float) x);
				setY((float) y);
			} break;
			// COLLISION WITH TOWERS
			case gameobject::TYPE::TOWER: {

			} break;
			// COLLISION WITH OBJECTS:
			case gameobject::TYPE::OBJECT: {

			} break;
		}
	}

    /*Point<float> BasicUnit::getMagneticVelocity()
    {
        return magneticVelocity;
    }

    void BasicUnit::setMagneticVelocity(const Point<float> v)
    {
        magneticVelocity = v;
    }

	// NETWORKING
	void BasicUnit::writeNetworkUpdate(int event_id, Buffer &buffer) {
		switch (event_id) {
			case BasicUnitUpdateEvents::SEND_POSITION:
				buffer << (int)getX();
				buffer << (int)getY();
                //std::cout << "[NETWORK::BasicUnit] Sending Position to clients" << std::endl;
				break;

			case BasicUnitUpdateEvents::SEND_HEALTH:
				buffer << (unsigned int)100;
				break;
		}
	}
	void BasicUnit::recvNetworkInteraction(int event_id, Buffer &buffer) {
		// TODO: BasicUnit interaction events
	}*/
}