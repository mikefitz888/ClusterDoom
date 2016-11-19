#ifndef TOWERLOGIC_H
#define TOWERLOGIC_H

#include <vector>
#include "smartpointers.h"

namespace towerlogic {
	typedef smartpointers::slave_ptr<Tower> tower_ptr;

	class TowerLogic {
		std::vector<tower_ptr> towers;
	public:
		std::vector<Tower*> getTowers();
		Tower* createTower();
		Tower* removeTower(Point& position);
		TowerLogic(){}; //Prevent initialization, static class
	}
}

#endif //TOWERLOGIC_H