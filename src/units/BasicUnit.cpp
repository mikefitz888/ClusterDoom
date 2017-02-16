#include "../../include/Units/BasicUnit.h"
#include "../../include/ResourceManager.h"
#include "../../include/gameobject.h"
#include "../../include/manager.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace unit {
    BasicUnit::BasicUnit(id_t key, Manager* m) : Unit(key, TYPE::BASIC, m)  {
        position.x = 200;
        position.y = 400;
    }

    void BasicUnit::init(){
		collision_profile.setTypeCircle(20);
        render_manager = manager->getRenderManager();
        texture        = manager->getResourceManager()->getTexture("basic_unit");
    }

    void BasicUnit::step() {
        // Perform parent step
        Unit::step();

        // Network timers
        /*
            Send the position every few steps
        */
        network_update_position_timer--;
        if (network_update_position_timer <= 0) {
            network_update_position_timer = network_update_position_timer_max;
            this->sendNetworkUpdate(BasicUnitUpdateEvents::SEND_POSITION);
        }
    }

    void BasicUnit::render(){
        render_manager = manager->getRenderManager();


        float rotation = (float) (atan2(render_facing.y-getYr(), render_facing.x-getXr()) - M_PI/2);
        texture->render(getXr(), getYr(), 0.10f, 0.10f, rotation);
    }

	void BasicUnit::onCollision(gameobject_ptr other) {
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
	}
}