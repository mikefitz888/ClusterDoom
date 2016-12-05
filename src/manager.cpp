#include "../include/manager.h"

namespace manager {
	Manager::Manager() {
		tower_logic = new TowerLogic(this);
		unit_logic = new UnitLogic(this);
		game_controller = new GameController(this);
	}

	//Tower Methods
	slave_ptr<Tower> Manager::createTower(tower::TYPE type){
		auto obj = tower_logic->createTower(getFreePoolKey(), type );
		addToPool(obj);
		obj->init();
		auto passback = slave_ptr<Tower>( static_pointer_cast<Tower>(game_object_pool[obj->getID()]) );
		tower_logic->giveSlavePtr(passback);
		std::cout << "end of tower create manager" << std::endl;
		return passback;
	}

	void Manager::destroyTower(slave_ptr<Tower> tower){
		removeFromPool(tower->getID());
	}

	std::vector<slave_ptr<Tower>>& Manager::getTowers() const {
		return (this->tower_logic)->getTowers();
	}

	//Unit Methods
	slave_ptr<Unit> Manager::createUnit(unit::TYPE type){
		auto obj = unit_logic->createUnit( getFreePoolKey(), type );
		addToPool(obj);
		obj->init();
		auto passback = slave_ptr<Unit>( static_pointer_cast<Unit>(game_object_pool[obj->getID()]) );
		unit_logic->giveSlavePtr(passback);
		return passback;
	}

	void Manager::destroyUnit(slave_ptr<Unit>& unit){
		removeFromPool(unit->getID());
	}

	std::vector<slave_ptr<Unit>> Manager::getUnits() const {
		return unit_logic->getUnits();
	}

	//Game Controller Methods
	slave_ptr<GameObject> Manager::createObject(){
		auto obj = game_controller->createObject( getFreePoolKey() );
		addToPool(obj);
		obj->init();
		return game_object_pool[obj->getID()];
	}

	void Manager::destroyObject(slave_ptr<GameObject>& obj){
		removeFromPool(obj->getID());
	}

	void Manager::destroy(GameObject* obj) {
		removeFromPool(obj->getID());
	}

	// Object Pool Methods
	void Manager::addToPool(GameObject* game_object){
		auto id = game_object->getID();
		if(id < game_object_pool.size()){
			game_object_pool[id] = master_ptr<GameObject>(game_object);
		}else{
			if(id == game_object_pool.size()){
				game_object_pool.push_back(master_ptr<GameObject>(game_object));
			}else{
				std::cout << "Invalid key, investigate.";
			}
		}
		std::cout << "Object added to pool with id = " << id << std::endl;
		std::cout << "Pool size now = " << game_object_pool.size() << std::endl;
	}

	// Destroys the master_ptr
	void Manager::removeFromPool(id_t id){
		game_object_pool[id].invalidate();
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

	void Manager::init() const {
		game_controller->init();
	}

	bool Manager::step(){
		game_controller->step();
		return render();
	}

	void Manager::initRenderManager(RenderManager &rm) { 
		render_manager=&rm;
		world_renderer = new WorldRenderer(this);
		render_manager->setRenderParent(world_renderer);
		render_manager->init();
	}

	bool Manager::render() const {
		if(render_manager) {
			return render_manager->render();
		}
		return false;
	}

	RenderManager* Manager::getRenderManager() const { 
		return render_manager;
	}

	void Manager::releaseRender() { 
		render_manager->release(); 
		render_manager = nullptr;
		delete world_renderer;
	}

	void Manager::renderAll() {
		for (slave_ptr<GameObject> obj : game_object_pool) {
			if(obj) obj->render();
		}
	}

	void Manager::stepAll() {
		//std::cout << "stepAll()" << std::endl;
		for (slave_ptr<GameObject> obj : game_object_pool) {
			if(obj) obj->step();
		}
	}
}