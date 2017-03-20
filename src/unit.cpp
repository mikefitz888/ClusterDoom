#include "../include/unit.h"
#include "../include/tower.h"
#include "../include/ResourceManager.h"
#include "../include/VertexBuffer.h"
#include "../include/manager.h"
#include "../include/gamecontroller.h"
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
        texture = manager->getResourceManager()->getTexture("pawn");

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

        vbuff = new graphics::VertexBuffer();
        hpbar_buff = new graphics::VertexBuffer();

        vbuff->addQuad(-100.0f, -56.2f, 100.0f, 56.2f);
        hpbar_buff->addQuadRGBA(-32.0f, -2.0f, 32.0f, 2.0f, 1.0f, 0.0f, 0.0f, 1.0f);

        vbuff->freeze();
        hpbar_buff->freeze();
        
    }
    void Unit::render() {
        render_manager->setActiveShader(shader);
        render_manager->setTexture(texture);

        render_manager->setActiveColour(graphics::Colour(255, 255, 255, 255));
        glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(getX(), getY(), 0.0));
        render_manager->setWorldMatrix(transform);
        vbuff->render();

        render_manager->setTexture(red);
        render_manager->setActiveColour(graphics::Colour(255, 255, 255, 255));
        transform = glm::translate(glm::mat4(), glm::vec3(getX(), getY()-30.0f, 0.0f) );
        transform = glm::scale(transform, glm::vec3(health/1000, 1.0f, 1.0f));
        render_manager->setWorldMatrix(transform);
        hpbar_buff->render();
    }
    void Unit::renderGUI() {}
    void Unit::release() {}

    void Unit::step() {
        /*
        *  Description of Unit activity: 
        *  Move towards Base.
        *  If tower is in range and able to shoot: stop -> attack tower -> continue with step 1
        * 
        *  There should be a cooldown between shots so the unit can "stutter-step"
        */
        GameObject::step();
        this->setSmoothingRate(0.25f);

        auto& target = game_controller->getBase();
        if (!target) {
            return; //NO TARGET
        }
        this->setDestination(target->getPosition(), 1.25f);

        render_facing = getDestination();//target->getPosition();

        //float distance = (target->getX() - getX())*(target->getX() - getX()) + (target->getY() - getY())*(target->getY() - getY());
        /*ivec2 destination = getDestination();
        int distance = DIST_SQ(getX(), destination.x, getY(), destination.y);*/
        if (distanceTo(target->getPosition()) < 100 && cooldown-- == 0) {
           attack(target);
           cooldown = 40;
        }
    }

    tower_ptr Unit::getNearestTower() const
    //Not actually implemented correctly, just prototype
    {
        auto towers = manager->getTowers();
        int minDist = INT_MAX;
        tower_ptr outTower = nullptr;

        int dist;
        for (auto tower : towers) {
            if (tower->getHealth() > 0) {
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

    void Unit::attacked(gameobject_ptr aggressor, int damage) {
        health -= damage;
        if (health <= 0) {
            deliverWealth(20);
            destroySelf();
        }
    }

    void Unit::deliverWealth(size_t amt) {
        manager->getGameController()->increaseWealth(amt);
    }

	/*void Unit::getPath(Point<int> target) {
		Point<int> start = this->position;
		gamecontroller::GameController* gc = this->manager->getGameController();
		int screenWidth = gc->getScreenWidth();
		int screenHeight = gc->getScreenHeight();
		//aStar(start, target, screenWidth, screenHeight, gc, path);
	}*/
}
