#ifndef TOWERLOGIC_H
#define TOWERLOGIC_H

#include <vector>
#include "smartpointers.h"
#include "tower.h"
#include "manager.h"

namespace towerlogic {
	using tower::Tower;
	using manager::Manager;
	typedef smartpointers::slave_ptr<Tower> tower_ptr;

	class TowerLogic {
		std::vector<tower_ptr> towers;
		Manager * manager;
	public:
		inline std::vector<tower_ptr> getTowers() const {return std::vector<tower_ptr>();}
		inline Tower* createTower(int key){return new Tower(key, NULL);}
		tower_ptr createTower();
		inline Tower* removeTower(int x, int y){return new Tower(0, NULL);} //TODO: this function is placehodor
		TowerLogic(Manager* m) : manager(m) {}; //Prevent initialization, static class
	};
}

#endif //TOWERLOGIC_H
