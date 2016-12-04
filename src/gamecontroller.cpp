#include "../include/gamecontroller.h"

namespace gamecontroller {
	GameController::GameController(Manager* m) : manager(m) {}

	GameObject* GameController::createObject(id_t key){
		return NULL;
	}

	void GameController::init(){
		manager->createTower();
		manager->createUnit();
	}
	
	void GameController::step(){
		manager->stepAll();
	}
}