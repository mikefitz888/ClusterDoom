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
		inline std::vector<tower_ptr> getTowers() const {return std::vector<tower_ptr>();}
		inline Tower* createTower(int key){return new Tower(key, NULL);}
		inline Tower* removeTower(int x, int y){return new Tower(0, NULL);} //TODO: this function is placehodor
		TowerLogic(){}; //Prevent initialization, static class
	};
}

#endif //TOWERLOGIC_H