#include "../include/manager.h"
#include "../include/AudioManager.h"
#include "../include/gamecontroller.h"
#include "../include/WorldRenderer.h"
#include "../include/unit.h"
#include "../include/tower.h"
#include "../include/VertexBuffer.h"
#include "../include/ResourceManager.h"

namespace tower {
    using namespace graphics;

    Tower::Tower(id_t id, TYPE tower_type, Manager* m) :
        GameObject(id, gameobject::TYPE::TOWER, tower_type, m) {}

    void Tower::init(){
        render_manager = manager->getRenderManager();
        game_controller = manager->getGameController();

        texture = manager->getResourceManager()->getTexture("basic_tower");
        /*if(!texture->loadFromFile("src/Resources/Textures/chess_piece_rook.png")){
            std::cout << "[ERROR] Could not load texture! (Tower)" << std::endl;
        }*/

        red = manager->getResourceManager()->getTexture("red");
        /*if (!red->loadFromFile("src/Resources/Textures/red.png")) {
            std::cout << "[ERROR] Could not load texture! (Tower)" << std::endl;
        }*/

        shader = render_manager->createShaderFromFile("src/Resources/Shaders/Render2D_vert.glsl", "src/Resources/Shaders/Render2D_frag.glsl");
        if(shader == nullptr){
            std::cout << "[ERROR] FAILED TO LOAD SHADER (Tower)" << std::endl;
        }

        vbuff = new graphics::VertexBuffer();
        hpbar_buff = new graphics::VertexBuffer();

        vbuff->addQuad(-32.0f, -32.0f, 32.0f, 32.0f);

        hpbar_buff->addQuadRGBA(-32.0f, -2.0f, 32.0f, 2.0f, 1.0f, 0.0f, 0.0f, 1.0f);

        vbuff->freeze();
        hpbar_buff->freeze();
    }

    void Tower::render(){
        render_manager->setActiveShader(shader);
        render_manager->setTexture(texture);

        render_manager->setActiveColour(Colour(0, 0, 255, 255));
        glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(getX(), getY(), 0.0));
        render_manager->setWorldMatrix(transform);
        vbuff->render();
        render_manager->setActiveColour(Colour(255, 255, 255, 255));

        render_manager->setTexture(red);
        float hp = (float) health / 100;
        transform = glm::translate(glm::mat4(), glm::vec3(getX(), getY()-50, 0.0));
        transform = glm::scale(transform, glm::vec3(hp, 1.0f, 1.0f));
        render_manager->setWorldMatrix(transform);
        hpbar_buff->render();

    }

    void Tower::renderGUI(){
        
    }

    void Tower::release(){
        render_manager = nullptr;
        delete vbuff;
        delete texture;
        delete shader;
    }

    //Gameplay Methods
    void Tower::step() {
        if(health == 0) {
            manager->getAudioManager()->playSound("cannon");
        }

        if(health <= 0){
            health--;
            if(health == -100){
                health = max_health;
            }
        }

		//TODO: update render positions
		render_position.x -= (render_position.x - position.x) / 20;
		render_position.y -= (render_position.y - position.y) / 20;

        auto units = manager->getUnits();
        for(auto unit : units) {
            if( (unit->getX()-getX())*(unit->getX()-getX()) + (unit->getY() - getY())*(unit->getY() - getY()) < 10000) {
                attack(unit);
                return;
            }
        }
    }

    unit_ptr Tower::getUnit() {
        getUnits(1);
        if (units.size()) return units[0];
        return nullptr;
    }

    std::vector<unit_ptr>& Tower::getUnits(size_t amt) {
        units.clear();
        if (amt == 0) return units;

        for (auto& unit : manager->getUnits()) {
            units.emplace_back(unit);
            if (--amt == 0) return units;
        }
    }

    float Tower::getHealth() const {
        return health;
    }

    void Tower::attack(unit_ptr unit)    {
        unit->attacked(this->getSharedPtr());
    }

    void Tower::attacked(gameobject_ptr aggressor) {
        if (health > 0)
        {
            //health--;
            if (health < 0)
            {
                health = 0;
            }
        }
    }

    size_t Tower::requestMoney(size_t amt) {
        return game_controller->requestWealth(amt);
    }

    float Tower::requestEfficiency(size_t amt) {
        return (float)requestMoney(amt) / (float)amt;
    }
}
