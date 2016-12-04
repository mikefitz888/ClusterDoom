#include "../include/gamecontroller.h"

namespace gamecontroller {
	GameController::GameController(Manager* m) : manager(m) {}

	GameObject* GameController::createObject(id_t key){
		return nullptr;
	}

	void GameController::init() const {
		manager->createTower();
		manager->createUnit();
	}
	
	void GameController::step() const {
		manager->stepAll();
	}
}