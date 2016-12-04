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
		inline TowerLogic(Manager* m) : manager(m) {};
		inline std::vector<tower_ptr>& getTowers() {return towers;}
		Tower* createTower (id_t key) const;
		tower_ptr createTower() const;
		void removeTower(int x, int y);
		void removeTower(tower_ptr tower);
		
		void giveSlavePtr(tower_ptr tower);
	};
}

#endif //TOWERLOGIC_H

