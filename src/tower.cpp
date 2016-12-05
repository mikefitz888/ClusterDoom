#include "../include/manager.h"

namespace tower {
	void Tower::init(){
		render_manager = manager->getRenderManager();

		texture = new sf::Texture();
		if(!texture->loadFromFile("src/Resources/Textures/chess_piece_rook.png")){
			std::cout << "[ERROR] Could not load texture! (Tower)" << std::endl;
		}

		shader = render_manager->createShaderFromFile("src/Resources/Shaders/Render2D_vert.glsl", "src/Resources/Shaders/Render2D_frag.glsl");
		if(shader == nullptr){
			std::cout << "[ERROR] FAILED TO LOAD SHADER (Tower)" << std::endl;
		}

		vbuff = new graphics::VertexBuffer();
		//hpbar_buff = new graphics::VertexBuffer();
		
		vbuff->addQuad(-32.0f, -32.0f, 32.0f, 32.0f);

		//hpbar_buff->addQuad(-32.0f, -2.0f, 32.0f, 2.0f);

		vbuff->freeze();
		//hpbar_buff->freeze();
	}

	void Tower::render(){
		render_manager->setActiveShader(shader);
		render_manager->setTexture(texture);
		
		glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(getX(), getY(), 0.0));
		render_manager->setWorldMatrix(transform);
		vbuff->render();
		
		render_manager->setTexture(red);
		transform = glm::translate(glm::mat4(), glm::vec3(getX(), getY()-50, 0.0));
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
	void Tower::attack(unit_ptr tower)	{
		
	}

	void Tower::attacked(unit_ptr tower) {
		
	}
}