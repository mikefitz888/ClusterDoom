#include "../include/manager.h"

namespace tower {
	void Tower::init(){
		render_manager = manager->getRenderManager();
		texture = new sf::Texture();
		if(!texture->loadFromFile("src/Resources/Textures/datboi.jpg")){
			std::cout << "[ERROR] Could not load texture! (Tower)" << std::endl;
		}else{
			std::cout << "Loaded Texture" << std::endl;
		}

		shader = render_manager->createShaderFromFile("src/Resources/Shaders/Render2D_vert.glsl", "src/Resources/Shaders/Render2D_frag.glsl");
		if(shader == NULL){
			std::cout << "[ERROR] FAILED TO LOAD SHADER (Tower)" << std::endl;
		}else{
			std::cout << "Loaded shader" << std::endl;
		}

		//vbuff = new graphics::VertexBuffer();
		//vbuff->addQuad(-64.0f, -64.0f, 64.0f, 64.0f);
		//vbuff->freeze();
	}

	void Tower::render(){
		//std::cout << "Tower Render" << std::endl;
	}

	void Tower::renderGUI(){

	}

	void Tower::release(){
		render_manager = NULL;
	}
}