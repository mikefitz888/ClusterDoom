#include "../include/towerlogic.h"
#include "../include/manager.h"

namespace towerlogic {
	tower_ptr TowerLogic::createTower() {
		return manager->createTower();
	}
}
