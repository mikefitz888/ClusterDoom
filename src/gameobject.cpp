#include "../include/gameobject.h"
#include "../include/manager.h"

namespace gameobject {
	int GameObject::_destroySelf() {
		manager->destroy(this);
		return 0;
	}
}