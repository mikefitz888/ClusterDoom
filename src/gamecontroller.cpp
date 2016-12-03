#include "../include/gamecontroller.h"

namespace gamecontroller {
	GameController::GameController(Manager* m) : manager(m) {}

	GameObject* GameController::createObject(id_t key){
		return NULL;
	}

	void GameController::init(){
		std::cout << "Tower created supposedly" << std::endl;
		manager->createTower();
	}
	
	void GameController::step(){

	}
}