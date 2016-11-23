#ifndef TOWERLOGIC_H
#define TOWERLOGIC_H

#include <vector>
#include "smartpointers.h"
#include "tower.h"

namespace towerlogic {
	using tower::Tower;
	typedef smartpointers::slave_ptr<Tower> tower_ptr;

	class TowerLogic {
		std::vector<tower_ptr> towers;
	public:
		std::vector<tower_ptr> getTowers() const;
		Tower* createTower(int key);
		Tower* removeTower(int x, int y);
		TowerLogic(){}; //Prevent initialization, static class
	};
}

#endif //TOWERLOGIC_H