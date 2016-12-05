#include "../include/gamecontroller.h"

namespace gamecontroller {
	GameController::GameController(Manager* m) : manager(m) {}

	GameObject* GameController::createObject(id_t key){
		return nullptr;
	}

	unit_ptr GameController::spawnUnitAt(int x, int y) const {
		auto unit = manager->createUnit(unit::TYPE::BASE);
		unit->setPosition(x, y);
		return unit;
	}

	tower_ptr GameController::spawnTowerAt(int x, int y) const {
		auto tower = manager->createTower(tower::TYPE::BASE);
		tower->setPosition(x, y);
		return tower;
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