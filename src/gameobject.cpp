#include "../include/gameobject.h"
#include "../include/manager.h"

namespace gameobject {
    int GameObject::_destroySelf() {
        manager->destroy(this);
        return 0;
    }

    float GameObject::distanceTo(smartpointers::slave_ptr<GameObject> other) const {
        //int x = (other->getX() - getX());
        //int y = (other->getY() - getY());
        //return (x*x + y*y);
        return position.distanceTo(other->getPosition());
    }

	float GameObject::distanceTo(Point<int> point) const {
		//int x = (point.x - getX());
		//int y = (point.y - getY());
		//return (x*x + y*y);
        return position.distanceTo(point);
	}
}