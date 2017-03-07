#ifndef MANAGER_H
#define MANAGER_H

#include "gamecore.h"
#include "../include/smartpointers.h"

namespace manager {
    using gameobject::GameObject;
    using smartpointers::master_ptr;
    using smartpointers::slave_ptr;
    using gameobject::gameobject_ptr;
    using smartpointers::static_pointer_cast;
    using towerlogic::TowerLogic;
    using unitlogic::UnitLogic;
    using gamecontroller::GameController;
    using unit::Unit;
    using tower::Tower;
    using graphics::RenderManager;
    using worldrenderer::WorldRenderer;
    using network::NetworkManager;
    using gameobject::Collision;

    class Manager {
        std::vector<master_ptr<GameObject>> game_object_pool;
        std::vector<id_t> free_id_list_tmp;
        std::vector<id_t> free_id_list;

        TowerLogic* tower_logic;
        UnitLogic* unit_logic;
        GameController* game_controller; //IRenderable
        ObjectLogic* object_logic;

        RenderManager*  render_manager    = nullptr;
        WorldRenderer*  world_renderer    = nullptr;
        NetworkManager* network_manager   = nullptr;
        ResourceManager* resource_manager = nullptr;
        AudioManager*    audio_manager    = nullptr;

        void addToPool(GameObject* game_object);
        void removeFromPool(id_t id);
        id_t getFreePoolKey();

		// Event management
		Splitmap* splitmap;

    public:
        Manager();
        GameController* getGameController() const;
        //Tower Methods
        slave_ptr<Tower> createTower(tower::TYPE type);
        slave_ptr<GameObject> createObject(gameobject::OBJECT_TYPE type);
        void destroyTower(slave_ptr<Tower> tower);
        void clearTowers();
        std::vector<slave_ptr<Tower>>& getTowers() const;

        //Unit Methods
        slave_ptr<Unit> createUnit(unit::TYPE type);
        void destroyUnit(slave_ptr<Unit>& unit);
        std::vector<slave_ptr<Unit>> getUnits() const;

        //Game Controller Methods (World Logic)
        //slave_ptr<GameObject> createObject(gameobject::OBJECT_TYPE type);
        void destroyObject(slave_ptr<GameObject>& obj);


        //GameObject Methods
        void destroy(GameObject* obj);

        //Network Methods
        void sendAllInstancesToClient(network::NetworkClient *network_client);

        //void sendEvent();
        //void receiveEvent();

        //Manager Methods
        void init();
        void initRenderManager(RenderManager &rm);
        bool render() const;
        RenderManager*   getRenderManager() const;
        ResourceManager* getResourceManager() const;
        AudioManager*    getAudioManager() const;
        void release();
        void renderAll();
        void stepAll();
        void collisionAll();


        bool step();
        void restart();
    };


	/*
		Splitmap:
			- Splitmap is used by the collision system. It is a 2D array of vectors to type
			gameobject_ptr.
			- Each iteration, the splitmap is updated. Each cell contains a list of the objects
			that intersect with it. This is then used to optimise collisions, as objects only
			need to test collisions with those other objects sharing cells.
	
	
	*/

	class Splitmap {

	private:
		int splitmap_width, splitmap_height, splitmap_cellsize;
		std::vector<gameobject_ptr> ***collision_splitmap;
		std::unordered_set<std::pair<gameobject_ptr, gameobject_ptr>> tested_objects;

	public:
		Splitmap(int cellsize, int width, int height);
		~Splitmap();

		void clear();
		void add(int cell_x, int cell_y, gameobject_ptr object); // Add object to specific cell
		void add(gameobject_ptr object); // Add using objects collision profile to test intersection
		int getWidth();
		int getHeight();
		int getCellSize();
		glm::vec2 convertRealWorldPositionToCell(int rpos_x, int rpos_y);
		
		// Get a copy of the vector of objects in a given cell.
		bool getCellObjects(int cell_x, int cell_y, std::vector<gameobject_ptr>& vector);
		
		// Perform all collision tests
		/*
			This will fire off collision events for the objects
		*/
		void performAllCollisions();
	};
}

#endif //MANAGER_H
