#include "../include/manager.h"
#include "../include/towerlogic.h"
#include "../include/ObjectLogic.h"
#include "../include/network/Network.h"
#include "../include/unitlogic.h"
#include "../include/ResourceManager.h"
#include "../include/AudioManager.h"
#include "../include/gamecontroller.h"
#include "../include/WorldRenderer.h"
#include "../include/tower.h"
#include "../include/unit.h"

namespace manager {
    using smartpointers::static_pointer_cast;
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
        auto passback = slave_ptr<GameObject>(static_pointer_cast<GameObject>(/*game_object_pool[obj->getID()]*/obj->getSharedPtr()));
        return passback;
    }

    GameController* Manager::getGameController() const {
        return game_controller;
    }

    gameobject_ptr Manager::getObjectById(unsigned int id) {
        if (this->getObjectExists(id)) {
            return *game_object_pool[id];
        } else {
            return nullptr;
        }
    }

    bool Manager::getObjectExists(unsigned int id) {
        return (this->game_object_pool.find(id) != this->game_object_pool.end());
    }

    //Tower Methods
    slave_ptr<Tower> Manager::createTower(tower::TYPE type){
        auto obj = tower_logic->createTower(getFreePoolKey(), type );
        addToPool(obj);
        obj->init();
        auto passback = slave_ptr<Tower>( static_pointer_cast<Tower>(obj->getSharedPtr() /* game_object_pool[obj->getID()]    game_object_pool.find(obj->getID())*/ ));
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
            bool synced = (*game_object_pool[id])->getNetworkSend();
            (*game_object_pool[id]).invalidate();
            
           // mstr_ptr.invalidate();
            //free_id_list.push_back(id);
            if (synced) {
                network_manager->sendInstanceDestroy(id);
            }
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
        auto passback = slave_ptr<Unit>( static_pointer_cast<Unit>(/*game_object_pool[obj->getID()]*/obj->getSharedPtr()) );
        unit_logic->giveSlavePtr(passback);
        return passback;
    }

    void Manager::destroyUnit(slave_ptr<Unit>& unit){
        removeFromPool(unit->getID());
    }

    std::vector<slave_ptr<Unit>> Manager::getUnits() const {
        return unit_logic->getUnits();
    }

    std::vector<slave_ptr<GameObject>> Manager::getObjects() const {
        std::vector<slave_ptr<GameObject>> objs;
        for (auto map_elem = game_object_pool.begin(); map_elem != game_object_pool.end(); map_elem++) {
            gameobject_ptr obj = *(map_elem->second);
            if (obj) {
                objs.push_back(obj);
            }
        }
        return objs;
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

        // Check if ID exists
        bool exists = false;
        exists = (this->game_object_pool.find(id) != this->game_object_pool.end());
        if (!exists) {
            //auto kek = master_ptr<GameObject>(game_object);
            //game_object_pool[id] = &master_ptr<GameObject>(game_object);
            //game_object_pool.insert(std::map<unsigned int, master_ptr<GameObject>>::value_type(id, master_ptr<GameObject>(game_object)));
            master_ptr<GameObject>* ptr = new master_ptr<GameObject>(game_object);
            game_object_pool.insert(std::pair<unsigned int, master_ptr<GameObject>*>(id, ptr));
            //std::cout << "INSERTED OBJECT WITH ID: " << id << " INTO MAP" << std::endl;
            //game_object_pool.emplace(id, game_object);
        }
        else {
            std::cout << "FATAL ERROR! ID ALREADY IN USE: " << id << std::endl;
        }


        /*if(id < game_object_pool.size()){
            game_object_pool[id] = master_ptr<GameObject>(game_object);
            //game_object_pool.emplace(game_object_pool.begin()+id, game_object);
        }else{
            if(id == game_object_pool.size()){
                game_object_pool.push_back(master_ptr<GameObject>(game_object));
            }else{
                std::cout << "Invalid key, investigate.";
            }
        }*/
        //std::cout << "Object added to pool with id = " << id << std::endl;
        //std::cout << "Pool size now = " << game_object_pool.size() << std::endl;

        // Set shared pointer
        auto ptr = game_object_pool[id];
        gameobject_ptr self = gameobject_ptr(*ptr);
        self->setSharedPtr(self);
		self->setNetworkManager(network_manager);
		self->setNetworkID(self, id, game_object->getSuperType(), game_object->getSubType());

        // Run setup
        self->setup();

        // Network update
        network_manager->sendInstanceCreate(id, game_object->getSuperType(),
                                            game_object->getSubType(),
                                            (int) game_object->getX(),
                                            (int) game_object->getY());
    }

    // Destroys the master_ptr
    void Manager::removeFromPool(id_t id){
        bool synced = (*game_object_pool[id])->getNetworkSend();
        (*game_object_pool[id]).invalidate();
        //free_id_list.push_back(id); // <- Broken

        tower_logic->clean();
        unit_logic->clean();

        if (synced) {
            network_manager->sendInstanceDestroy(id);
        }
    }

    /*std::vector<master_ptr<GameObject>>& Manager::getObjectPool() {
        return game_object_pool;
    }*/
    void Manager::sendAllInstancesToClient(network::NetworkClient *network_client) {
        for (auto map_elem = game_object_pool.begin(); map_elem != game_object_pool.end(); map_elem++) {
            slave_ptr<GameObject> object = *map_elem->second;
            if (object) {
                int id         = object->getID();
                int super_type = object->getSuperType();
				int sub_type   = object->getSubType();
				int x          = (int) object->getX();
				int y		   = (int) object->getY();
                network_manager->sendInstanceCreate(network_client, id, super_type, sub_type, x, y);
            }
        }
    }

    // This removes the free key from list so ensure it is used to create a GameObject
    id_t Manager::getFreePoolKey(){
        unsigned int free_id = this->game_object_max_id;
        game_object_max_id++;
        return free_id;
    }

    void Manager::init() {
        game_controller->init();
    }

    bool Manager::step(){
        audio_manager->stepSounds();
        network_manager->networkStep();
        bool gc = game_controller->step(); 

        return render() && gc;
        //return render();
    }

    void Manager::initRenderManager(RenderManager &rm) {
        render_manager=&rm;
        world_renderer = new WorldRenderer(this);
        render_manager->setRenderParent(world_renderer);
        render_manager->init(this);

		// Create splitmap
		const int splitmap_cellsize = 64;
		int splitmap_width, splitmap_height;
		splitmap_width  = render_manager->getWindowWidth() / splitmap_cellsize;
		splitmap_height = render_manager->getWindowHeight() / splitmap_cellsize;
		splitmap		= new Splitmap(splitmap_cellsize, splitmap_width, splitmap_height);
    }

    bool Manager::render() const {
        //if( stepc % 2 == 0){ render_manager->clear(); return true; }
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
        /*
            Depth object pairs are used to allow objects to be depth sorted.
            This allows us to customise the render order of different objects.
        */

        std::vector<std::pair<int, gameobject_ptr>> depth_object_pairs;
        depth_object_pairs.clear();

        for (auto map_elem = game_object_pool.begin(); map_elem != game_object_pool.end(); map_elem++) {
            slave_ptr<GameObject> obj = *map_elem->second;
            if (obj) {
                if (obj->getReady()) {
                    //obj->render();
                    depth_object_pairs.push_back(std::pair<int, gameobject_ptr>(obj->getDepth(), obj));
                }
            }
        }
        //return;

        // SORT LIST
        /*
            If both objects are at the same depth:
                If both objects have the same super type:
                    Order by sub type
                Else
                    Order by super type
            Else
               Order by depth
            --------------------------------------------------
            This sorting function aims to accomplish two things:
            - Put objects with larger depths at the top of the list so that they render first (Objects with small, or negative depths, will render after
            - For objects at the same depth, group them together by type so that objects of the same type render one after another. (This will improve rendering
            optimisation by reducing the number of context changes, as the same objects are unlikely to have GPU pipeline state changes between them)
        
        */
        std::sort(depth_object_pairs.begin(), depth_object_pairs.end(),
                  [](const std::pair<int, gameobject_ptr> & a, const std::pair<int, gameobject_ptr> & b) -> bool {
                        return (a.first == b.first) ? (
                            (a.second->getSuperType() == b.second->getSuperType()) ?
                                ((a.second->getSubType() == b.second->getSubType()) ?
                                    (a.second->getID() < b.second->getID()) :
                                    (a.second->getSubType() < b.second->getSubType())) :
                                (a.second->getSuperType() < b.second->getSuperType())) :
                            (a.first > b.first);
                  });

        // RENDER BEGIN
        for (auto obj : depth_object_pairs) {
            obj.second->renderBegin();
        }

        // RENDER
        for (auto obj : depth_object_pairs) {
            obj.second->render();
        }

        // RENDER GUI
        for (auto obj : depth_object_pairs) {
            obj.second->renderGUI();
        }

        
    }

    void Manager::stepAll() {
        //std::cout << "stepAll()" << std::endl;
        std::vector<slave_ptr<GameObject>> copy;
        copy.clear();

        for (auto map_elem = game_object_pool.begin(); map_elem != game_object_pool.end(); map_elem++) {
            slave_ptr<GameObject> obj = *map_elem->second;
            if (obj) {
                copy.push_back(obj);
            }
        }
        for (slave_ptr<GameObject> obj :copy) {
            if (obj) {
                obj->setReady(true);
                obj->step();
            }
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
        copy.clear();
        for (auto map_elem = game_object_pool.begin(); map_elem != game_object_pool.end(); map_elem++) {
            slave_ptr<GameObject> obj = *map_elem->second;
			copy.push_back(obj);
		}

		/*
		For every object, test collisions against every other
		object (excluding self).
		*/
		/*int collision_tests = 0;
		for (slave_ptr<GameObject> obj : copy) {
			Collision* my_collision = obj->getCollision();
			my_collision->setParent(obj);

			if (obj->getRunCollisionEvent() && my_collision->getCollidable()) {
				for (slave_ptr<GameObject> other : copy) {
				
					if (obj && other && obj != other) {

						Collision* other_collision = other->getCollision();
						other_collision->setParent(other);

						collision_tests++;
						if (my_collision->intersects(other_collision)) {
							obj->onCollision(other);
							//std::cout << "collision between objects" << std::endl;
						}
					}
				}
			}
		}
		std::cout << "Collision tests: " << collision_tests << std::endl;*/

		// Collision model 2.0
		/*
			Maintain a grid where each cell contains a list of objects that collide with that cell.
			- List is built by using the objects bounding box and filling all the cells it 
			intersects with. (Per object, iterate over the grid-aligned space that its bounding
			box occupies).

			- After these lists are generated, for each list, perform collisions between objects
			within each cell.

			Result: Reduced number of intersection tests needed. Massive reduction for
			sparsely located objects.

			Potential downsides:
			- Objects executing collision events with the same instances.
				Potential solutions: Each object keeps a list of the objects they have already
				collided with on that frame. (Though increased complexity).
		
		*/
		// Clear the splitmap
		splitmap->clear();

		// Add all objects
		for (slave_ptr<GameObject> obj : copy) {
            if (obj) {
                if (obj->getReady()) {
                    splitmap->add(obj);
                }
            }
		}

		// Perform collisions
		splitmap->performAllCollisions();
		

	}

	/*
		Create a new split map with the given size.
			See manager.h for detailed explanation
	*/
	Splitmap::Splitmap(int cellsize, int width, int height) {

		// Declare members
		this->splitmap_cellsize = cellsize;
		this->splitmap_width    = width;
		this->splitmap_height   = height;

		// Initialise splitmap datastructre
		collision_splitmap = new std::vector<gameobject_ptr>**[splitmap_width];
		for (int i = 0; i < splitmap_width; i++) {
			collision_splitmap[i] = new std::vector<gameobject_ptr>*[splitmap_height];

			for (int j = 0; j < splitmap_height; j++) {
				collision_splitmap[i][j] = new std::vector<gameobject_ptr>;
			}
		}



	}

	// Splitmap destructor
	Splitmap::~Splitmap() {

		// Delete splitmap
		for (int i = 0; i < splitmap_width; i++) {
			for (int j = 0; j < splitmap_height; j++) {
				delete collision_splitmap[i][j];
			}
			delete collision_splitmap[i];
		}
		delete collision_splitmap;
	}

	// Clear splitmap
	void Splitmap::clear() {
		for (int i = 0; i < splitmap_width; i++) {
			for (int j = 0; j < splitmap_height; j++) {
				collision_splitmap[i][j]->clear();
			}
		}
		tested_objects.clear();
	}

	// Add element to a specific cell of a splitmap
	void Splitmap::add(int cell_x, int cell_y, gameobject_ptr object) {
		if (cell_x >= 0 && cell_x < splitmap_width &&
			cell_y >= 0 && cell_y < splitmap_height) {
			collision_splitmap[cell_x][cell_y]->push_back(object);
		}

	}

	// Add element to all cells it intersects with
	void Splitmap::add(gameobject_ptr object) {

		// if object does not exist, or is not collidable
		if (!object || !object->getCollision()->getCollidable()) { return; }

		gameobject::BoundingBox bounding_box = object->getCollision()->getBoundingBox();
		glm::vec2 min_cell_pos, max_cell_pos;
		min_cell_pos = this->convertRealWorldPositionToCell((int) object->getX()+ bounding_box.bbox_left, (int) object->getY() + bounding_box.bbox_up);
		max_cell_pos = this->convertRealWorldPositionToCell((int) object->getX()+bounding_box.bbox_right, (int) object->getY() + bounding_box.bbox_down);
		//std::cout << "BBOX : " << min_cell_pos.x << " : " << max_cell_pos.x << std::endl;
		// Mark all intersecting cells
		for (int i = (int) min_cell_pos.x; i <= (int) max_cell_pos.x; i++) {
			for (int j = (int) min_cell_pos.y; j <= (int) min_cell_pos.y; j++) {
				this->add(i, j, object);
			}
		}
	}

	// General getters
	int Splitmap::getWidth() {
		return this->splitmap_width;
	}

	int Splitmap::getHeight() {
		return this->splitmap_height;
	}

	int Splitmap::getCellSize() {
		return this->splitmap_cellsize;
	}
	/*
		Get a copy of the vector of objects in a given cell.
	*/
	bool Splitmap::getCellObjects(int cell_x, int cell_y, std::vector<gameobject_ptr>& vector) {
		if (cell_x >= 0 && cell_x < splitmap_width &&
			cell_y >= 0 && cell_y < splitmap_height) {
			vector = *this->collision_splitmap[cell_x][cell_y];
			return true;
		}
		return false;
	}

	// convert a realworld position to the cell coordinate on the corresponding splitmap
	glm::vec2 Splitmap::convertRealWorldPositionToCell(int rpos_x, int rpos_y) {
		glm::vec2 cell_position;
		cell_position.x = (float) (rpos_x / this->splitmap_cellsize);
		cell_position.y = (float) (rpos_y / this->splitmap_cellsize);
		return cell_position;
	}

	// Perform all collision tests
	void Splitmap::performAllCollisions() {

		/*
			For every cell, perform collision tests
			within subjects.
		
		*/
		int collision_tests = 0;
		for (int i = 0; i < splitmap_width; i++) {
			for (int j = 0; j < splitmap_height; j++) {
				auto objects = *collision_splitmap[i][j];

				/*
					Perform collisions between pairs of objects, 
					avoiding testing itself.

					Note: Will need to mark which pairs of objects have
					already collided
				*/
				for (auto object : objects) {

					if (object) {
						Collision* my_collision = object->getCollision();
						my_collision->setParent(object);

						for (auto other : objects) {
							if (other && object && object != other) {

								// Check if already tested
								std::pair<gameobject_ptr, gameobject_ptr> object_pair(object, other);
								bool already_tested = tested_objects.find(object_pair) != tested_objects.end();

								if (!already_tested) {
									Collision* other_collision = other->getCollision();
									other_collision->setParent(other);
									collision_tests++;
									if (my_collision->intersects(other_collision)) {
										object->onCollision(other);
									}
                                    if (object && other) {
                                        // Mark this as tested, so we don't run the collision event twice
                                        tested_objects.insert(object_pair);
                                    }
								}
							}
						}
					}
				}
			}
		}
		
		//std::cout << "COLLISION TESTS: " << collision_tests << std::endl;

	}


}