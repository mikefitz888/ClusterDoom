#include "../include/towerlogic.h"
#include "../include/manager.h"

namespace towerlogic {
	tower_ptr TowerLogic::createTower() {
		return manager->createTower();
	}

	void TowerLogic::removeTower(int x, int y) {
		for(std::vector<tower_ptr>::iterator it = towers.begin(); it != towers.end(); ++it) {
		    if ((*it)->getX() == x && (*it)->getY() == y) {
				return manager->destroyTower((*it));
			}
		 }
	}
}

