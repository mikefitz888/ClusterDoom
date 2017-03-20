#include "../../include/Units/Wizard.h"
#include "../../include/ResourceManager.h"
#include "../../include/gameobject.h"
#include "../../include/manager.h"
#include "../../include/gamecontroller.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "../../include/RenderUtils.h"

namespace unit {
    Wizard::Wizard(id_t key, Manager* m) : Unit(key, TYPE::WIZARD, m)  {
        position.x = 200;
        position.y = 400;
    }

    void Wizard::init(){
		collision_profile.setTypeCircle(14);
        render_manager = manager->getRenderManager();
        texture        = manager->getResourceManager()->getTexture("basic_unit");


        adjust.push_back(glm::linearRand(-adjust_max, adjust_max));
        adjust.push_back(glm::linearRand(-adjust_max, adjust_max));
        adjust.push_back(glm::linearRand(-adjust_max, adjust_max));
        adjust.push_back(glm::linearRand(-adjust_max, adjust_max));
        this->setDistanceThreshold(50);
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


        float rotation = (float)(atan2(render_facing.y - getYr(), render_facing.x - getXr()) - M_PI / 2);
        texture->render(getXr(), getYr(), 0.40f, 0.40f, rotation);

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
				int x, y;
				x = getX();
				y = getY();

				glm::vec2 direction = glm::vec2(other->getPosition().x - x,
					other->getPosition().y - y);
				direction = glm::normalize(direction);

				x -= (int) (direction.x * 5);
				y -= (int) (direction.y * 5);
				setX(x);
				setY(y);
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