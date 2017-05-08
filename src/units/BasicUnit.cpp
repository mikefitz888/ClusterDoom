#include "../../include/Units/BasicUnit.h"
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
    BasicUnit::BasicUnit(id_t key, Manager* m) : Unit(key, TYPE::BASIC, m) {

    }

    BasicUnit::~BasicUnit() {
        this->manager->getAudioManager()->playSound("robot_death");
    }

    void BasicUnit::init() {

        Unit::init();

        collision_profile.setTypeCircle(14);
        render_manager = manager->getRenderManager();
        texture = manager->getResourceManager()->getAnimatedTexture("robot_unit");
		deathTexture = manager->getResourceManager()->getAnimatedTexture("robot_death");
		attackTexture = manager->getResourceManager()->getAnimatedTexture("robot_attack");
        this->setDistanceThreshold(95);

        health = 8000;
        maxHealth = 8000;
        unitSpeed = 0.1f;

        /* Path path;
        path.push_back(vec2(100, 100));
        path.push_back(vec2(500, 50));
        path.push_back(vec2(250, 300));
        path.push_back(vec2(1280, 720));
        this->setPath(path, 10);*/
    }

    void BasicUnit::step() {

        // Perform parent step
        Unit::step();

		if (dead) {
			if (animationProgress >= 15.0f) {
				destroySelf();
			}
			else {
				return;
			}
		}



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
        }
        else {
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

    void BasicUnit::render() {

        Unit::render();

		if (!dead) {
			auto& base = game_controller->getBase();
			if (distanceTo(base->getPosition()) < 100) {
				renderAnimation(attackTexture, 0.18f, 0.18f, 12, 1.0f, 0.0f);
			}
			else {
				renderAnimation(texture, 0.18f, 0.09f, 12, 1.0f, 0.0f);
			}
		}
		else {
			renderAnimation(deathTexture, 0.18f, 0.36f, 16, 1.0f, 0.0f);
		}

		//renderAnimation(texture, 0.13f, 0.1f*this->glacial_effect_vis, 12, 1.0f, 0.0f);

        /*render_manager = manager->getRenderManager();


        float rotation = (float)(atan2(render_facing.y - getYr(), render_facing.x - getXr()) - M_PI / 2);
        float animationSpeed = 0.1f;
        animation_progress = animation_progress + animationSpeed;
        int m = 1;
        int frame[6] = { 0,1,2,3,2,1 };
        if (this->getAtDestination()) m = 0;


        texture->render(m*frame[((int)animation_progress) % 6], (int) getXr(), (int) getYr(), 0.13f, 0.13f, rotation);*/

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

        if (isUnderGlacialEffect())
        {
            render_manager->setActiveColour(graphics::Colour(255, 255, 255, 255));
        }
    }

    void BasicUnit::onCollision(gameobject_ptr other) {
        // Push away from other objects:

        switch (other->getSuperType()) {
            // COLLISION WITH OTHER INSTANCES
        case gameobject::TYPE::UNIT: {
            glm::vec2 spos = getPosition();
            glm::vec2 _direction = other->getPosition() - spos;//glm::vec2(other->getPosition().x - x, other->getPosition().y - y);
            if (glm::length(_direction) > 0.0f) {
                _direction = glm::normalize(_direction);

            }
            else { // If two units occupy the exact same space, move in random direction
                _direction = glm::normalize(glm::linearRand(glm::vec2(-1.0f, -1.0f), glm::vec2(1.0f, 1.0f)));
            }
            spos -= _direction*5.0f;
            this->setPosition(spos);
        } break;
            // COLLISION WITH TOWERS
        case gameobject::TYPE::TOWER: {

        } break;
            // COLLISION WITH OBJECTS:
        case gameobject::TYPE::OBJECT: {

        } break;
        }
    }

    Point<float> BasicUnit::getMagneticVelocity()
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
    void BasicUnit::recvNetworkInteraction(int event_id, Buffer &buffer, network::NetworkClient* interaction_connection_client) {
        // TODO: BasicUnit interaction events
    }
}