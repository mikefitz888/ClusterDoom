#ifndef TOWERLOGIC_H
#define TOWERLOGIC_H

#include <vector>
#include "smartpointers.h"
#include "tower.h"
#include "manager.h"

namespace manager {
	class Manager;
}

namespace tower {
	class Tower;
}

namespace towerlogic {
	using tower::Tower;
	using manager::Manager;
	typedef smartpointers::slave_ptr<Tower> tower_ptr;

	class TowerLogic {
		std::vector<tower_ptr> towers;
		Manager * manager;
	public:
		inline std::vector<tower_ptr> getTowers() const {return std::vector<tower_ptr>();}
		Tower* createTower(id_t key);
		tower_ptr createTower();
		void removeTower(int x, int y);
		TowerLogic(Manager* m) : manager(m) {}; //Prevent initialization, static class
	};
}

#endif //TOWERLOGIC_H

