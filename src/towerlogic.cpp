#include "../include/towerlogic.h"

namespace towerlogic {
	tower_ptr TowerLogic::createTower() {
		return manager->createTower();
	}

	Tower* TowerLogic::createTower(id_t key){
		return new Tower(key, manager);
	}

	void TowerLogic::removeTower(int x, int y) {
		for(std::vector<tower_ptr>::iterator it = towers.begin(); it != towers.end(); ++it) {
		    if ((*it)->getX() == x && (*it)->getY() == y) {
				return manager->destroyTower((*it));
			}
		 }
	}
}

