#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include "smartpointers.h"
#include "unitlogic.h"
#include "towerlogic.h"
#include "gamecontroller.h"
#include "RenderManager.h"
#include "VertexBuffer.h"
#include "WorldRenderer.h"
#include "network/Network.h"
//#include "ResourceManager.h"

namespace network {
	class NetworkManager;
	class NetworkClient;
}
namespace worldrenderer {
	class WorldRenderer;
}

namespace unit {
	class Unit;
	enum TYPE : unsigned int;
}

namespace tower {
	class Tower;
	enum TYPE : unsigned int;
}

namespace gameobject {
	class GameObject;
}

namespace towerlogic {
	class TowerLogic;
}

namespace unitlogic {
	class UnitLogic;
}

namespace gamecontroller {
	class GameController;
}

namespace manager {
	class Manager;
}

class ResourceManager {
public:
	ResourceManager(manager::Manager* manager);
};

namespace manager {
	using gameobject::GameObject;
	using smartpointers::master_ptr;
	using smartpointers::slave_ptr;
	using smartpointers::static_pointer_cast;
	using towerlogic::TowerLogic;
	using unitlogic::UnitLogic;
	using gamecontroller::GameController;
	using unit::Unit;
	using tower::Tower;
	using graphics::RenderManager;
	using worldrenderer::WorldRenderer;
	using network::NetworkManager;

	typedef size_t id_t;

	class Manager {
		//std::vector<master_ptr<GameObject>> game_object_pool;
		std::vector<master_ptr<GameObject>> game_object_pool;
		std::vector<id_t> free_id_list;

		TowerLogic* tower_logic;
		UnitLogic* unit_logic;
		GameController* game_controller; //IRenderable

		RenderManager*  render_manager    = nullptr;
		WorldRenderer*  world_renderer    = nullptr;
		NetworkManager* network_manager   = nullptr;
		ResourceManager* resource_manager = nullptr;

		void addToPool(GameObject* game_object);
		void removeFromPool(id_t id);
		id_t getFreePoolKey();

	public:
		Manager();
		inline GameController* getGameController() const { return game_controller; }
		//Tower Methods
		slave_ptr<Tower> createTower(tower::TYPE type);
		void destroyTower(slave_ptr<Tower> tower);
		std::vector<slave_ptr<Tower>>& getTowers() const;

		//Unit Methods
		slave_ptr<Unit> createUnit(unit::TYPE type);
		void destroyUnit(slave_ptr<Unit>& unit);
		std::vector<slave_ptr<Unit>> getUnits() const;

		//Game Controller Methods (World Logic)
		slave_ptr<GameObject> createObject();
		void destroyObject(slave_ptr<GameObject>& obj);


		//GameObject Methods
		void destroy(GameObject* obj);

		//Network Methods
		void sendAllInstancesToClient(network::NetworkClient *network_client);

		//void sendEvent();
		//void receiveEvent();

		//Manager Methods
		void init() const;
		void initRenderManager(RenderManager &rm);
		bool render() const;
		RenderManager*   getRenderManager() const;
		ResourceManager* getResourceManager() const;
		void release();
		void renderAll();
		void stepAll();
		

		bool step();
	};
}

#endif //MANAGER_H
