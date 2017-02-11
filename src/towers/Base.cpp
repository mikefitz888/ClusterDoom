#include "../../include/Towers/Base.h"
#include "../../include/ResourceManager.h"
#include "../../include/manager.h"

namespace tower {
	Base::Base(id_t key, Manager* m) : Tower(key, TYPE::BASE, m) {

	}

	void Base::init() {
		render_manager = manager->getRenderManager();
		texture = manager->getResourceManager()->getTexture("base_image");

		//Get Screen dims and position in center
	}

	void Base::render() {
		int size = 100;
		texture->render(getXr(), getYr(), size, size);
	}
}