#include "../include/WorldRenderer.h"

namespace worldrenderer {
	using towerlogic::tower_ptr;
	void WorldRenderer::init(){
		render_manager = manager->getRenderManager();
		std::cout << "init() done" << std::endl;

		std::vector<tower_ptr> towers = manager->getTowers();
		for(auto &tower : towers){
			tower->init();
		}
	}

	void WorldRenderer::render(){
		std::vector<tower_ptr> towers = manager->getTowers();
		for(auto &tower : towers){
			tower->render();
		}
	}

	void WorldRenderer::renderGUI(){
		
	}

	void WorldRenderer::release(){
		render_manager = NULL;
	}
}