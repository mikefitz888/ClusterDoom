#include "../include/unit.h"
#include "../include/tower.h"
#include <climits>

namespace unit {
	void Unit::init() {
		render_manager = manager->getRenderManager();
		texture = new sf::Texture();
		if (!texture->loadFromFile("src/Resources/Textures/pawn.png")) {
			std::cout << "[ERROR] Could not load texture! (Tower)" << std::endl;
		}

		red = new sf::Texture();
		if (!red->loadFromFile("src/Resources/Textures/red.png")) {
			std::cout << "[ERROR] Could not load texture! (Tower)" << std::endl;
		}

		shader = render_manager->createShaderFromFile("src/Resources/Shaders/Render2D_vert.glsl", "src/Resources/Shaders/Render2D_frag.glsl");
		if (shader == nullptr) {
			std::cout << "[ERROR] FAILED TO LOAD SHADER (Tower)" << std::endl;
		}
		else {
			std::cout << "Loaded shader" << std::endl;
		}

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
		auto target = getNearestTower();
		if (!target) {
			return; //NO TARGET
		}

		float distance = (target->getX() - getX())*(target->getX() - getX()) + (target->getY() - getY())*(target->getY() - getY());
		if (distance < 2000) {
			attack(target);
		}
		else {
			//Move to tower
			float dx = target->getX() - getX();
			float dy = target->getY() - getY();
			
			if (dx > 0) { position.x++; }
			else { position.x--; }

			if (dy > 0) { position.y++; }
			else { position.y--; }

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
		tower->attacked(this);
	}

	// BE VERY CAREFUL HERE, NON-SMARTPOINTER ACCESSIBLE
	void Unit::attacked(GameObject* aggressor) {
		health--;
		if(health <= 0) {
			destroySelf();
		}
	}
}
