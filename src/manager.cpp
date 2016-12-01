#include "../include/manager.h"

namespace manager {
	Manager::Manager() {
		tower_logic = new TowerLogic(this);
		unit_logic = new UnitLogic(this);
		game_controller = new GameController(this);
	}

	//Tower Methods
	slave_ptr<Tower> Manager::createTower(){
		GameObject* obj = tower_logic->createTower( getFreePoolKey() );
		obj->init();
		addToPool(obj);
		return slave_ptr<Tower>( static_pointer_cast<Tower>(game_object_pool[obj->getID()]) );
	}

	void Manager::destroyTower(slave_ptr<Tower> tower){
		removeFromPool(static_pointer_cast<GameObject>(tower));
	}

	std::vector<slave_ptr<Tower>> Manager::getTowers() const {
		return (this->tower_logic)->getTowers();
	}

	//Unit Methods
	slave_ptr<Unit> Manager::createUnit(){
		GameObject* obj = unit_logic->createUnit( getFreePoolKey() );
		obj->init();
		addToPool(obj);
		return slave_ptr<Unit>( static_pointer_cast<Unit>(game_object_pool[obj->getID()]) );
	}

	void Manager::destroyUnit(slave_ptr<Unit>& unit){
		removeFromPool(smartpointers::static_pointer_cast<GameObject>(unit));
	}

	std::vector<slave_ptr<Unit>> Manager::getUnits() const {
		return unit_logic->getUnits();
	}

	//Game Controller Methods
	slave_ptr<GameObject> Manager::createObject(){
		GameObject* obj = game_controller->createObject( getFreePoolKey() );
		obj->init();
		addToPool(obj);
		return game_object_pool[obj->getID()];
	}

	void Manager::destroyObject(slave_ptr<GameObject>& obj){
		removeFromPool(obj);
	}

	// Object Pool Methods
	void Manager::addToPool(GameObject* game_object){
		id_t id = game_object->getID();
		if(id < game_object_pool.size()){
			game_object_pool[id] = master_ptr<GameObject>(game_object);
		}else{
			if(id == game_object_pool.size()){
				game_object_pool.push_back(master_ptr<GameObject>(game_object));
			}else{
				std::cout << "Invalid key, investigate.";
			}
		}
	}

	// Destroys the master_ptr
	void Manager::removeFromPool(slave_ptr<GameObject> game_object){
		id_t id = game_object->getID();
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

	void Manager::init(){
		game_controller->init();
	}

	bool Manager::step(){
		game_controller->step();
		return render();
	}

	void Manager::initRenderManager(RenderManager &rm) { 
		render_manager=&rm;
		world_renderer = new WorldRenderer(this);
		world_renderer->init();
	}
	bool Manager::render() {
		if(render_manager != NULL) {
			return render_manager->render();
		}
		return false;
	}
	RenderManager* Manager::getRenderManager() const { 
		return render_manager;
	}
	void Manager::releaseRender() { 
		render_manager->release(); 
		render_manager = NULL;
		delete world_renderer;
	}

}