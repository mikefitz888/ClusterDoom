#include "../include/unit.h"
#include "../include/tower.h"

namespace unit {
	void Unit::init() {
		render_manager = manager->getRenderManager();
		texture = new sf::Texture();
		if (!texture->loadFromFile("src/Resources/Textures/pawn.png")) {
			std::cout << "[ERROR] Could not load texture! (Tower)" << std::endl;
		}
		else {
			std::cout << "Loaded Texture" << std::endl;
		}

		shader = render_manager->createShaderFromFile("src/Resources/Shaders/Render2D_vert.glsl", "src/Resources/Shaders/Render2D_frag.glsl");
		if (shader == nullptr) {
			std::cout << "[ERROR] FAILED TO LOAD SHADER (Tower)" << std::endl;
		}
		else {
			std::cout << "Loaded shader" << std::endl;
		}

		vbuff = new graphics::VertexBuffer();
		vbuff->addQuad(-64.0f, -64.0f, 64.0f, 64.0f);
		vbuff->freeze();
	}
	void Unit::render() {
		render_manager->setActiveShader(shader);
		render_manager->setTexture(texture);
		glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(getX(), getY(), 0.0));
		render_manager->setWorldMatrix(transform);

		vbuff->render();
	}
	void Unit::renderGUI() {}
	void Unit::release() {}

	void Unit::step() {
		auto target = getNearestTower();
		if (!target) {
			return; //NO TARGET
		}

		float distance = (target->getX() - getX())*(target->getX() - getX()) + (target->getY() - getY())*(target->getY() - getY());
		if (distance < 10) {
			//Attack tower
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
		for (auto tower : towers) {
			if (tower->getHealth() > 0) {
				return tower;
			}
		}
		return nullptr;
	}
}