#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include "smartpointers.h"
#include "unitlogic.h"
#include "towerlogic.h"
#include "gamecontroller.h"
#include <iostream>
#include "RenderManager.h"
#include "WorldRenderer.h"

namespace worldrenderer {
	class WorldRenderer;
}

namespace unit {
	class Unit;
}

namespace tower {
	class Tower;
}

namespace gameobject {
	class GameObject;
}

namespace towerlogic {
	class TowerLogic;
}

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

	typedef size_t id_t;

	class Manager {
		//std::vector<master_ptr<GameObject>> game_object_pool;
		std::vector<master_ptr<GameObject>> game_object_pool;
		std::vector<id_t> free_id_list;

		TowerLogic tower_logic;
		UnitLogic unit_logic;
		GameController game_logic; //IRenderable

		RenderManager* render_manager = NULL;
		WorldRenderer* world_renderer = NULL;

		void addToPool(GameObject* game_object);
		void removeFromPool(slave_ptr<GameObject> game_object);
		id_t getFreePoolKey();

	public:
		Manager();
		//Tower Methods
		slave_ptr<Tower> createTower();
		void destroyTower(slave_ptr<Tower> tower);
		std::vector<slave_ptr<Tower>> getTowers() const;

		//Unit Methods
		slave_ptr<Unit> createUnit();
		void destroyUnit(slave_ptr<Unit>& unit);
		std::vector<slave_ptr<Unit>> getUnits() const;

		//Game Controller Methods (World Logic)
		slave_ptr<GameObject> createObject();
		void destroyObject(slave_ptr<GameObject>& obj);

		//Network Methods
		//void sendEvent();
		//void receiveEvent();

		//Manager Methods
		void initRenderManager(RenderManager &rm);
		bool render();
		RenderManager* getRenderManager() const;
		void releaseRender();
		

		void step();
	};
}

#endif //MANAGER_H
