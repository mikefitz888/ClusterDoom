#include "../include/manager.h"
#include "../include/AudioManager.h"
#include "../include/gamecontroller.h"
#include "../include/WorldRenderer.h"
#include "../include/unit.h"
#include "../include/tower.h"
#include "../include/VertexBuffer.h"
#include "../include/ResourceManager.h"
#include "../include/RenderUtils.h"

namespace tower {
    using namespace graphics;

    Tower::Tower(id_t id, TYPE tower_type, Manager* m) :
        GameObject(id, gameobject::TYPE::TOWER, tower_type, m) {}

    //Load textures/animations
    void Tower::init(){
        render_manager = manager->getRenderManager();
        game_controller = manager->getGameController();

        texture = manager->getResourceManager()->getTexture("blackTower");

        red = manager->getResourceManager()->getTexture("red");

		rangeTexture = manager->getResourceManager()->getTexture("rangeCircle");

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

		lastX = getX();
		lastY = getY();
    }

    void Tower::render(){

		float mod = (200.0f - delete_queue) / 200.0f;
		int size = (int)(96.0f);
		//setActiveColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) 
		render_manager->setActiveColour(255, 255, 255, (char)(255.0 * mod));

		if (health <= 0) {
			render_manager->setActiveColour(255, 0, 0, (char)(255.0 * mod / 2));
		}

        // --- Disabled flash ---
        if (disabled) {
            flash_timer--;
            if (flash_timer <= 0) {
                flash_timer = flash_timer_max;
                flash = !flash;
            }
        } else {
            flash = false;
        }

        // --- Render circular bar ---
        Colour col = RenderUtils::colour_blend(Colour(0, 255, 0, 255), Colour(127, 0, 0, 255), game_controller->towerEfficiency(getPosition()) * 2 - 1);
        if (flash) {
            col = Colour(25, 25, 25, 255);
        }
        if (disabled) {
            col.a = 75;
        }

		RenderUtils::render_circular_health((int) getXr(), (int) getYr(), (int)health, (int)max_health,
            //RenderUtils::colour_blend(
            //    Colour(55, 55, 55, 255),
                col
            //    1.0f-std::min((float)game_controller->availableWealth() / max_power, 1.0f)
            //)
        );

		if (isMoving()) {
			rangeTexture->render((int)getXr(), (int)getYr(), 0.5f, 0.5f, 0.0f, 0.0f); //2*maxRange(200)/textureWidth(800) = 0.5f
		}
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

        //Unsafe code, removed it. Only code general to all towers should go here
        /*auto units = manager->getUnits();
        for(auto& unit : units) {
            if( (unit->getX()-getX())*(unit->getX()-getX()) + (unit->getY() - getY())*(unit->getY() - getY()) < 10000) {
                attack(unit);
                return;
            }
        }*/
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
        unit->attacked(this->getSharedPtr(), (float) damage);
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

    void Tower::attacked(gameobject_ptr aggressor, float damage){}

    //Returns current tower efficiency
    float Tower::requestEfficiency() {
        return game_controller->towerEfficiency(getPosition());
    }

	bool Tower::isMoving() {
		float newX = getX();
		if (lastX != newX) {
			lastX = newX;
			return true;
		}
		float newY = getY();
		if (lastY != newY) {
			lastY = newY;
			return true;
		}
		return false;
	}
}
