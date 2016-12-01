#include "../include/manager.h"

namespace tower {
	void Tower::init(){
		render_manager = manager->getRenderManager();
	}

	void Tower::render(){

	}

	void Tower::renderGUI(){

	}

	void Tower::release(){
		render_manager = NULL;
	}
}