#include "../../include/Units/Wizard.h"
#include "../../include/ResourceManager.h"
#include "../../include/gameobject.h"
#include "../../include/manager.h"
#include "../../include/gamecontroller.h"
#include "../../include/GameObjects/Projectiles.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "../../include/RenderUtils.h"
#include "../../include/AnimatedTexture.h"

namespace unit {
    Wizard::Wizard(id_t key, Manager* m) : Unit(key, TYPE::WIZARD, m)  {
        position.x = 200;
        position.y = 400;
    }

    void Wizard::init(){

		Unit::init();

		collision_profile.setTypeCircle(14);
        render_manager = manager->getRenderManager();
        texture        = manager->getResourceManager()->getAnimatedTexture("wizard_unit");


        adjust.push_back(glm::linearRand(-adjust_max, adjust_max));
        adjust.push_back(glm::linearRand(-adjust_max, adjust_max));
        adjust.push_back(glm::linearRand(-adjust_max, adjust_max));
        adjust.push_back(glm::linearRand(-adjust_max, adjust_max));
        this->setDistanceThreshold(50);

		health = 4500; //4500
		maxHealth = 4500; //4500
		unitSpeed = 0.3f;
       /* Path path;
        path.push_back(vec2(100, 100));
        path.push_back(vec2(500, 50));
        path.push_back(vec2(250, 300));
        path.push_back(vec2(1280, 720));
        this->setPath(path, 10);*/
    }

    void Wizard::step() {
        // Perform parent step
        Unit::step();

        //Walks towards base
        //If tower near, teleport closer to base (channelled w/ cd)
        auto& base = game_controller->getBase();
        setFrozen(false);
        if (channeling && health < maxHealth) {
            setFrozen(true);
            if (channel_time-- == 100) {
                glm::vec2 dir = glm::normalize(base->getPosition() - getPosition());
                float dist = (float) distanceTo(base->getPosition());
                setPosition(getPosition() + dir * std::fmin(dist - 80, 180.f));
                
            }
            else if (channel_time-- == 0) {
                channeling = false;
                channel_cooldown = 350;
            }
            else return;
        }

        if (!getAtDestination() && distanceTo(base->getPosition()) > 200 && channel_cooldown-- == 0) {
            channeling = true;
            channel_time = 200;
        }
        

        if (distanceTo(base->getPosition()) < 160 && cooldown-- == 0) {
            //gameobject_ptr obj1 = game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_LASER, Point<int>(getX(), getY()));
            //smartpointers::static_pointer_cast<unit::Unit>(other);
            smartpointers::slave_ptr<ProjectileLaser> obj1 = smartpointers::static_pointer_cast<ProjectileLaser>(game_controller->spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_LASER, Point<float>(getX(), getY())));
            obj1->setCollisionType(gameobject::TYPE::TOWER);
            //std::cout << "unit fired\n";
            auto dir = glm::normalize((base->getPosition()) - obj1->getPosition());
            obj1->setVelocity(dir * 7.f);
            //auto sdir = glm::vec2(-dir.y, dir.x) * 10.f;
            //obj1->setPosition(getPosition() + sdir);
            //obj2->setPosition(getPosition() - sdir);


            //attack(base);
            cooldown = 40;
        }
        

        // Network timers
        /*
            Send the position every few steps
        */
        /*network_update_position_timer--;
        if (network_update_position_timer <= 0) {
            network_update_position_timer = network_update_position_timer_max;
            this->sendNetworkUpdate(BasicUnitUpdateEvents::SEND_POSITION);
        }*/
    }

    void Wizard::render() {
        //render_manager = manager->getRenderManager();
		Unit::render();

        //texture->render(getXr(), getYr(), 0.40f, 0.40f, rotation);
        /*animation_progress = (animation_progress + 1) % 16;
        int m = 1;
        if (this->getAtDestination()) m = 0;
        float n = std::fabs(channel_time - 100.f) / 100.f;
        if (!channeling) n = 1.f;
        //texture->render(m*animation_progress / 8, getXr(), getYr(), 0.10f*n, 0.10f*n, rotation); currently no animation
		texture->render(1, getXr(), getYr(), 0.10f*n, 0.10f*n, rotation + ((1.0f-n)*10.0*M_PI));*/

		float n = std::fabs(channel_time - 100.f) / 100.f;
		if (!channeling) n = 1.f;

		float rotation = (float)(atan2(render_facing.y - getYr(), render_facing.x - getXr()) - M_PI / 2) + ((1.0f - n)*10.0f*(float) M_PI);

		float animationSpeed = 0.13f;
		animation_progress = animation_progress + animationSpeed;
		int m = 1;
		if (this->getAtDestination()) m = 0;


		texture->render( m* (((int)animation_progress) % 12), (int) getXr(), (int) getYr(), 0.18f*n, 0.18f*n, rotation);

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
    }

	void Wizard::onCollision(gameobject_ptr other) {
		// Push away from other objects:

		switch (other->getSuperType()) {
			// COLLISION WITH OTHER INSTANCES
			case gameobject::TYPE::UNIT: {
                // TODO: Should this be a float?
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

	// NETWORKING
	/*void BasicUnit::writeNetworkUpdate(int event_id, Buffer &buffer) {
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