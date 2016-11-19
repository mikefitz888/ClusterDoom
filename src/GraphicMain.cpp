
#include <iostream>
#include "../include/VertexBuffer.h"
#include "../include/RenderManager.h"

using namespace graphics;

class TempRenderScene : public IRenderable {

private:
	sf::Texture  *texture;
	sf::Shader   *shader, *alt_shader;
	VertexBuffer *vbuff;
	int counter = 0;
	bool other_shader = false;

	float x;

public:
	virtual void init() {
		x = 0.0f;
		// Load texture
		texture = new sf::Texture();
		if (!texture->loadFromFile("Resources/Textures/datboi.jpg")) {
			std::cout << "[ERROR] Could not load texture!" << std::endl;
		}
		
		// Load Shader
		shader = RenderManager::createShaderFromFile("Resources/Shaders/Render2D_vert.glsl", "Resources/Shaders/Render2D_frag.glsl");
		if (shader == NULL) {
			std::cout << "[ERROR] FAILED TO LOAD SHADER" << std::endl;
		}

		alt_shader = RenderManager::createShaderFromFile("Resources/Shaders/blue_vert.glsl", "Resources/Shaders/blue_frag.glsl");
		if (alt_shader == NULL) {
			std::cout << "[ERROR] FAILED TO LOAD SHADER" << std::endl;
		}


		// Create vertex buffer
		vbuff = new VertexBuffer();
		vbuff->addQuad(-64.0f, -64.0f, 64.0f, 64.0f);
		vbuff->freeze();

	};

	virtual void render() {

		// Switch shaders every 120 frames (2 seconds)
		counter++;
		if (counter > 120) {
			other_shader = !other_shader;
			counter = 0;
		}

		if (other_shader) {
			RenderManager::setActiveShader(alt_shader);
		} else {
			RenderManager::setActiveShader(shader);
		}
		RenderManager::setTexture(texture);

		// Apply transformations
		glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(x, 200.0, 0.0));
		RenderManager::setWorldMatrix(transform);

		vbuff->render();
		x += 5;

		if( x > (1280.0+64.0)){ x = -64.0f; }
	};

	virtual void renderGUI() {};
	virtual void release() {
		delete vbuff;
		delete texture;
		delete shader;
	};
};

int main() {

	// Create new RenderScene
	IRenderable *scene = new TempRenderScene();
	RenderManager::setRenderParent(scene);

	// Begin the graphics
	RenderManager::init();
	RenderManager::setWindowTitle("Clusterdoom");
	//RenderManager::render_manager.setWindowFullScreen(true);

	// run the main loop
	bool running = true;
	while (running) {

		// ..Perform Game Logic..
		

		// .. Perform render step
		running = RenderManager::render();
	}

	// release resources...
	RenderManager::release();

	return 0;
}