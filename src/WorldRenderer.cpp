#include "../include/WorldRenderer.h"

namespace worldrenderer {
	void WorldRenderer::init(){
		render_manager = manager->getRenderManager();
		render_manager->init();
	}

	void WorldRenderer::render(){
		render_manager->render();
	}

	void WorldRenderer::renderGUI(){
		
	}

	void WorldRenderer::release(){
		render_manager = NULL;
	}
}