#include "../include/unit.h"
#include "../include/tower.h"

namespace unit {
	void Unit::init() {}
	void Unit::render() {}
	void Unit::renderGUI() {}
	void Unit::release() {}

	void Unit::step() {
		tower_ptr target = getNearestTower();
		if (target == nullptr) {
			return; //NO TARGET
		}

		float distance = (target->getX() - getX())*(target->getX() - getX()) + (target->getY() - getY())*(target->getY() - getY());

		if (distance < 10) {
			//Attack tower
		}
		else {
			//Move to tower
			float dx = target->getX() - getX();
			float dy = target->getY() - getY();
			
			if (dx > 0) { position.x++; }
			else { position.x--; }

			if (dy > 0) { position.y++; }
			else { position.y--; }

		}
	}

	tower_ptr Unit::getNearestTower()
	//Not actually implemented correctly, just prototype
	{
		std::vector<tower_ptr> towers = manager->getTowers();
		for (auto tower : towers) {
			if (tower->getHealth() > 0) {
				return tower;
			}
		}
		return nullptr;
	}
}