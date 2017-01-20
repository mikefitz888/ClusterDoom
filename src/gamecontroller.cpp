#include "../include/gamecontroller.h"

namespace gamecontroller {
	GameController::GameController(Manager* m) : manager(m) {}

	GameObject* GameController::createObject(id_t key){
		return nullptr;
	}

	unit_ptr GameController::spawnUnitAt(int x, int y) const {
		auto unit = manager->createUnit(unit::TYPE::BASIC);
		unit->setPosition(x, y);
		return unit;
	}

	unit_ptr GameController::spawnUnitAt(Point<int> position) const {
		return spawnUnitAt(position.x, position.y);
	}


	tower_ptr GameController::spawnTowerAt(int x, int y) const {
		auto tower = manager->createTower(tower::TYPE::BASIC);
		tower->setPosition(x, y);
		return tower;
	}

	tower_ptr GameController::spawnTowerAt(Point<int> position) const {
		return spawnTowerAt(position.x, position.y);
	}

	void GameController::restart() const {
		for(auto obj : manager->getTowers()){
			//destroy
		}
		for(auto obj : manager->getUnits()){
			//destroy
		}
	}


	void GameController::init() const {
		spawnTowerAt(400, 200);
		spawnTowerAt(800, 200);
		spawnTowerAt(800, 600);
		spawnTowerAt(400, 600);
		spawnUnitAt(100, 50);
	}
	
	void GameController::step() const {
		manager->stepAll();
	}
}