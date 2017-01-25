#include "../../include/Units/BasicUnit.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace unit {
    BasicUnit::BasicUnit(id_t key, Manager* m) : Unit(key, TYPE::BASIC, m) {
        position.x = 200;
        position.y = 400;
    }

    void BasicUnit::init(){
        render_manager = manager->getRenderManager();
        /*texture = new sf::Texture();
        if(!texture->loadFromFile("Resources/Textures/Units/Basic.jpg")){
            std::cout << "[ERROR] Could not load texture! (BasicUnit)" << std::endl;
        }*/
        texture = manager->getResourceManager()->getTexture("basic_unit");
    }

    void BasicUnit::render(){
        render_manager = manager->getRenderManager();


        float rotation = atan2(render_facing.y-getY(), render_facing.x-getX()) - M_PI/2;
        texture->render(getX(), getY(), 0.10f, 0.10f, rotation);
    }
}