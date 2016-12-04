#include "../include/towerlogic.h"
#include <stdio.h>

namespace towerlogic {
	tower_ptr TowerLogic::createTower() const {
		return manager->createTower();
	}

	Tower* TowerLogic::createTower(id_t key) const {
		return new Tower(key, manager);
	}

	void TowerLogic::giveSlavePtr(tower_ptr tower){
		towers.push_back(tower);
		//printf("Recieved slave pointer: %d\n", 0);
		std::cout << "got: -" << towers.size() << "- towers" << std::endl;

	}

	void TowerLogic::removeTower(int x, int y) {
		for(auto it = towers.begin(); it != towers.end(); ++it) {
		    if ((*it)->getX() == x && (*it)->getY() == y) {
				return manager->destroyTower((*it));
			}
		 }
	}
}

