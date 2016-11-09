#include "../include/manager.h"

namespace manager {
	Manager::Manager(){
		tower_logic = TowerLogic();
		unit_logic = UnitLogic();
		game_logic = GameLogic();
	}

	//Tower Methods
	slave_ptr<Tower> Manager::createTower(){

	}

	void Manager::destroyTower(slave_ptr<Tower>& tower){

	}

	std::vector<slave_ptr<Tower>> Manager::getTowers() const {

	}

	//Unit Methods
	slave_ptr<Unit> Manager::createUnit(){

	}

	void Manager::destroyUnit(slave_ptr<Unit>& unit){

	}

	std::vector<slave_ptr<Unit>> Manager::getUnits() const {

	}

	//Game Controller Methods
	slave_ptr<GameObject> Manager::createObject(){

	}

	void Manager::destroyObject(slave_ptr<GameObject>& obj){

	}

	void Manager::init(){

	}

	void Manager::render(){

	}

	void Manager::renderGUI(){

	}

	void Manager::release(){

	}

}