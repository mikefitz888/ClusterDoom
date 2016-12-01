#include "../include/WorldRenderer.h"

namespace worldrenderer {
	using towerlogic::tower_ptr;
	void WorldRenderer::init(){
		render_manager = manager->getRenderManager();
		//render_manager->setRenderParent(this);
		render_manager->init();
		std::cout << "init() done" << std::endl;
	}

	void WorldRenderer::render(){
		std::vector<tower_ptr> towers = manager->getTowers();
		for(auto &tower : towers){
			//tower->render();
		}
	}

	void WorldRenderer::renderGUI(){
		
	}

	void WorldRenderer::release(){
		render_manager->release();
		render_manager = NULL;
	}
}