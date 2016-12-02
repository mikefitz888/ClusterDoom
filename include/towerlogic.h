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
	using gameobject::id_t;

	typedef smartpointers::slave_ptr<Tower> tower_ptr;

	class TowerLogic {
		std::vector<tower_ptr> towers;
		Manager * manager;
	public:
		inline std::vector<tower_ptr> getTowers() const {return towers;}
		Tower* createTower(id_t key);
		tower_ptr createTower();
		void removeTower(int x, int y);
		TowerLogic(Manager* m) : manager(m) {}; //Prevent initialization, static class
		void giveSlavePtr(tower_ptr tower);
	};
}

#endif //TOWERLOGIC_H

