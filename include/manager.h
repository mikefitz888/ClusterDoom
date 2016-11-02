#include <vector>
#include "util.h"

class Manager {
	private:
		std::vector<GameObject*> object_reference_pool;
	public:
		std::vector<Tower*> getTowers();
		Tower* createTower();
		void removeTower(Point position);

		std::vector<Unit*> getUnits(); 
		Unit* createUnit();
		void removeUnit();

		void step();
		void draw();
}

//TODO: FUNCTIONS FOR NETWORK INTERFACE
//TODO: FUNCTIONS FOR WORLDLOGIC

std::vector<Tower*> Manager::getTowers(){
	return TowerLogic::getTowers();
}

Tower* Manager::createTower(){
	Tower* created = TowerLogic::createTower();
	object_reference_pool.push_back(created);
	return created;
}

void Manager::removeTower(Point &position){
	Tower* removed = TowerLogic::removeTower(Point position);
	remove(object_reference_pool, removed); //util function, remove item from vector
}

std::vector<Unit*> Manager::getUnits(){
	return UnitLogic::getUnits();
}

Unit* Manager::createUnit(){
	Unit* created = UnitLogic::createUnit();
	object_reference_pool.push_back(created);
	return created;
}

void Manager::removeUnit(Unit* unit){
	UnitLogic::removeUnit(unit);
	remove(object_reference_pool, unit); //util function, remove item from vector
}

void Manager::step(){
	TowerLogic::step();
	UnitLogic::step();
	WorldLogic::step();
}

void Manager::draw(){
	TowerLogic::draw();
	UnitLogic::draw();
	WorldLogic::draw();
}