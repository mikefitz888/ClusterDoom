#include "../include/WorldRenderer.h"

namespace worldrenderer {
	using towerlogic::tower_ptr;
	void WorldRenderer::init(){
		render_manager = manager->getRenderManager();
	}

	void WorldRenderer::render(){
		manager->renderAll();
	}

	void WorldRenderer::renderGUI(){
		
	}

	void WorldRenderer::release(){
		render_manager = nullptr;
	}
}