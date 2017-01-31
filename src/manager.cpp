#include "../include/manager.h"
#include "../include/ResourceManager.h"
#include <algorithm> // for remove_if
#include <functional> // for unary_function

namespace manager {

    Manager::Manager() {
        tower_logic      = new TowerLogic(this);
        unit_logic       = new UnitLogic(this);
		object_logic     = new ObjectLogic(this);
        game_controller  = new GameController(this);
        network_manager  = new NetworkManager(this);
        resource_manager = new ResourceManager(this);
        audio_manager    = new AudioManager(this);
    }

	// Object Methods
	slave_ptr<GameObject> Manager::createObject(gameobject::OBJECT_TYPE type) {
		auto obj = object_logic->createObject(getFreePoolKey(), type);
		addToPool(obj);
		obj->init();
		auto passback = slave_ptr<GameObject>(static_pointer_cast<GameObject>(game_object_pool[obj->getID()]));
		return passback;
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

	void Manager::clearTowers() {
		for (auto tower : (this->tower_logic)->getTowers()) {
			id_t id = tower->getID();
			game_object_pool[id].invalidate();
			free_id_list.push_back(id);
			network_manager->sendInstanceDestroy(id);
		}

		tower_logic->clean();
		unit_logic->clean();
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
    /*slave_ptr<GameObject> Manager::createObject(){
        auto obj = game_controller->createObject( getFreePoolKey() );
        addToPool(obj);
        obj->init();
        return game_object_pool[obj->getID()];
    }*/

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

		// Set shared pointer
		gameobject_ptr self = game_object_pool[id];
		self->setSharedPtr(self);

        // Network update
        network_manager->sendInstanceCreate(id, game_object->getSuperType());
    }

    // Destroys the master_ptr
    void Manager::removeFromPool(id_t id){
        game_object_pool[id].invalidate();
        free_id_list.push_back(id);
        tower_logic->clean();
        unit_logic->clean();
        network_manager->sendInstanceDestroy(id);
    }

    /*std::vector<master_ptr<GameObject>>& Manager::getObjectPool() {
        return game_object_pool;
    }*/
    void Manager::sendAllInstancesToClient(network::NetworkClient *network_client) {
        for (slave_ptr<GameObject> object : game_object_pool) {
            if (object) {
                int id = object->getID();
                int type = object->getSuperType();
                network_manager->sendInstanceCreate(network_client, id, type);
            }
        }
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
        audio_manager->stepSounds();
        network_manager->networkStep();
        game_controller->step();
        return render();
    }

    void Manager::initRenderManager(RenderManager &rm) { 
        render_manager=&rm;
        world_renderer = new WorldRenderer(this);
        render_manager->setRenderParent(world_renderer);
        render_manager->init(this);
    }

    bool Manager::render() const {
        if(render_manager) {
            return render_manager->render();
        }
        return false;
    }

    // Get render manager
    RenderManager* Manager::getRenderManager() const { 
        return render_manager;
    }

    // Get resource manager
    ResourceManager* Manager::getResourceManager() const {
        return resource_manager;
    }

    // Get audio manager
    AudioManager* Manager::getAudioManager() const {
        return audio_manager;
    }

    void Manager::release() { 
        render_manager->release(); 
        render_manager = nullptr;
        delete world_renderer;

        network_manager->release();
    }

    void Manager::renderAll() {
        for (slave_ptr<GameObject> obj : game_object_pool) {
            if(obj) obj->render();
        }
    }

    void Manager::stepAll() {
        //std::cout << "stepAll()" << std::endl;
		std::vector<slave_ptr<GameObject>> copy;
		for (slave_ptr<GameObject> obj : game_object_pool) {
			copy.push_back(obj);
		}
        for (slave_ptr<GameObject> obj :copy) {
            if(obj) obj->step();
        }
    }


	void Manager::restart() {}

	/*
		Performs collisions between all the given objects and fires off events
		for those objects that collide with each other.

		Collision events will run both ways if the objects continue to collide.
	
	*/
	void Manager::collisionAll() {

	/*
	Create a copy of the object list. This allows objects
	to destroy themselves during the collision event.
	*/

	std::vector<slave_ptr<GameObject>> copy;
	for (slave_ptr<GameObject> obj : game_object_pool) {
		copy.push_back(obj);
	}

	/*
	For every object, test collisions against every other
	object (excluding self).
	*/

		for (slave_ptr<GameObject> obj : copy) {

			for (slave_ptr<GameObject> other : copy) {

				if (obj && other && obj != other) {
					//Collision* my_collision = obj->getCollision();
					//Collision* other_collision = other->getCollision();

					//if (my_collision->intersects(other_collision)) {

					//}
				}
			}

		}
	}

}