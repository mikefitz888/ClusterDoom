#include "../include/gameobject.h"
#include "../include/manager.h"

namespace gameobject {
	void GameObject::destroySelf() {
		manager->destroy(this);
	}
}