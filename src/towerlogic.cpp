#include "../include/towerlogic.h"
#include <stdio.h>

namespace towerlogic {
<<<<<<< HEAD

=======
	//using gameobject::id_t;
>>>>>>> 3960a524e51e0ae597cce932fde5416eff1dff5a

	tower_ptr TowerLogic::createTower() {
		return manager->createTower();
	}

	Tower* TowerLogic::createTower(id_t key){
		return new Tower(key, manager);
	}

	void TowerLogic::giveSlavePtr(tower_ptr tower){
		towers.push_back(tower);
		//printf("Recieved slave pointer: %d\n", 0);
		std::cout << "got: -" << towers.size() << "- towers" << std::endl;

	}

	void TowerLogic::removeTower(int x, int y) {
		for(std::vector<tower_ptr>::iterator it = towers.begin(); it != towers.end(); ++it) {
		    if ((*it)->getX() == x && (*it)->getY() == y) {
				return manager->destroyTower((*it));
			}
		 }
	}
}

