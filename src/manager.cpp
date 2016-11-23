#include "../include/manager.h"

namespace manager {
	Manager::Manager() : tower_logic(), unit_logic() { /* Initializer list preferred, less contructor calls */ }

	//Tower Methods
	slave_ptr<Tower> Manager::createTower(){
		GameObject* obj = tower_logic.createTower( getFreePoolKey() );
		addToPool(obj);
	}

	void Manager::destroyTower(slave_ptr<Tower> tower){
		removeFromPool(smartpointers::static_pointer_cast<GameObject>(tower));
	}

	std::vector<slave_ptr<Tower>> Manager::getTowers() const {
		return (this->tower_logic).getTowers();
	}

	//Unit Methods
	slave_ptr<Unit> Manager::createUnit(){
		GameObject* obj = unit_logic.createUnit( getFreePoolKey() );
		addToPool(obj);
	}

	void Manager::destroyUnit(slave_ptr<Unit>& unit){
		removeFromPool(smartpointers::static_pointer_cast<GameObject>(unit));
	}

	std::vector<slave_ptr<Unit>> Manager::getUnits() const {
		return unit_logic.getUnits();
	}

	//Game Controller Methods
	slave_ptr<GameObject> Manager::createObject(){
		GameObject* obj = game_logic.createObject( getFreePoolKey() );
		addToPool(obj);
	}

	void Manager::destroyObject(slave_ptr<GameObject>& obj){
		removeFromPool(obj);
	}

	// Object Pool Methods
	void Manager::addToPool(GameObject* game_object){
		game_object_pool.insert(std::pair<id_t, master_ptr<GameObject>>(game_object->getID(), master_ptr<GameObject>(game_object)));
	}

	// Destroys the master_ptr
	void Manager::removeFromPool(slave_ptr<GameObject> game_object){
		id_t id = game_object->getID();
		//TODO: fix required, does not compile
		//game_object_pool[id].destroy();
		free_id_list.push_back(id);
	}

	// This removes the free key from list so ensure it is used to create a GameObject
	id_t Manager::getFreePoolKey(){
		if(!free_id_list.size()){
			return game_object_pool.size();
		}
		id_t id = free_id_list.back();
		free_id_list.pop_back();
		return id;
	}

	//Manager is not IRenderable but is in control of some
	void Manager::init(){

	}

	void Manager::render(){

	}

	void Manager::renderGUI(){

	}

	void Manager::release(){

	}

	void Manager::step(){

	}

}