#include "../include/cvInterface.h"

namespace cvinterface {
	void ICVInterface::updateTowerList(std::vector<Point<int>>& towerlist) {
		tower_locations = towerlist;
		if(tower_locations.size() > towers.size()) {
			auto tower = game_controller->spawnTowerAt( tower_locations[towers.size()] );
			towers.push_back(tower);
		}else if(tower_locations.size() < towers.size()) {
			//There are less towers in play than in game, find removed tower; For each point find nearest tower and pair
			
		}else {
			//Tower count unchanged
		}

		//Deal with any movement
	}

	void ICVInterface::init() {
		
	}

	void ICVInterface::release() {
		
	}


}