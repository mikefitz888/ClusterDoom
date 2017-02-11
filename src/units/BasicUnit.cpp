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
        /*texture = new sf::Texture();
        if(!texture->loadFromFile("Resources/Textures/Units/Basic.jpg")){
            std::cout << "[ERROR] Could not load texture! (BasicUnit)" << std::endl;
        }*/
        texture = manager->getResourceManager()->getTexture("basic_unit");
    }

    void BasicUnit::render(){
        render_manager = manager->getRenderManager();


        float rotation = (float) (atan2(render_facing.y-getY(), render_facing.x-getX()) - M_PI/2);
        texture->render(getX(), getY(), 0.10f, 0.10f, rotation);
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
}