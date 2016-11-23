#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include "smartpointers.h"
#include "unitlogic.h"
#include "towerlogic.h"
#include "gameobject.h"
#include "gamecontroller.h"
#include "unit.h"
#include "tower.h"
#include <iostream>

namespace manager {
	using smartpointers::master_ptr;
	using smartpointers::slave_ptr;
	using smartpointers::static_pointer_cast;
	using towerlogic::TowerLogic;
	using unitlogic::UnitLogic;
	using gamecontroller::GameController;
	using unit::Unit;
	using tower::Tower;
	using gameobject::GameObject;

	typedef size_t id_t;

	class Manager {
		//std::vector<master_ptr<GameObject>> game_object_pool;
		std::vector<master_ptr<GameObject>> game_object_pool;
		std::vector<id_t> free_id_list;

		TowerLogic tower_logic;
		UnitLogic unit_logic;
		GameController game_logic; //IRenderable

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
		void init();
		void render();
		void renderGUI();
		void release();

		void step();
	};
}

#endif //MANAGER_H