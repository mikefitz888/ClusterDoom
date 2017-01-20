#include "../../include/Units/BasicUnit.h"

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
        
        glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(getX(), getY(), 0.0));
        transform = glm::scale(transform, glm::vec3(0.25f, 0.25f, 0.25f));
        render_manager->setWorldMatrix(transform);
        texture->render();
    }
}