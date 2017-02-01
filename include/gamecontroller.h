#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "gamecore.h"

#include "gameobject.h"
#include "GameObjects\Spawn.h"
#include "manager.h"
#include "../include/network/Buffer.h"
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <vector>


#include <iostream>
#include <deque>
#include <stack>
#include <unordered_map>
#include <cmath>
#include <algorithm>

#define NO_MATCH -1

namespace tower {
	enum TYPE : unsigned int;
}

namespace gameobject {
    class GameObject;
}

namespace manager {
    class Manager;
}

namespace gamecontroller {
    using gameobject::GameObject;
    using gameobject::id_t;
    using gameobject::tower_ptr;
    using gameobject::unit_ptr;
	using gameobject::gameobject_ptr;
    using gameobject::Point;
    using manager::Manager;

    using std::cout;
    using std::endl;
    using std::vector;
    using std::deque;
    using std::stack;
    using std::unordered_map;
    using std::tuple;


    struct Matching
    {
        unordered_map<tower_ptr, Point<int>> matches;
        vector<tower_ptr> deleted_towers;
        vector<Point<int>> new_towers;
    };

    class GameController {
        Manager* manager;
        vector<Point<int>> cvList;
		// CV Network
		int                port;
		sf::TcpListener   *listener;
		Buffer            *recv_buffer;
		sf::TcpSocket     *client;
		bool              cvConnectionEstablished;

		void startCVServer();
		void cvNetworkStep();

        int create_count = 0;
		int frame_clock = 0;

		bool game_started = false;
		int wave = 0;
		int scenario = 0;

		std::vector<smartpointers::slave_ptr<Spawn>> spawn_points = std::vector<smartpointers::slave_ptr<Spawn>>();
		// TEMP
		bool spawned = false;

    public:
        GameController(Manager* m);
       // GameObject* createObject(id_t key);
		inline float getElapsedTime() {	return (float)frame_clock / 60.0f; }
		inline void resetClock() { frame_clock = 0; };
        void init();
        void step();
        void restart() const;
        Matching stableMatching(vector<Point<int>>& detections);
        int getWeight(int x, int y);

        tower_ptr  spawnTowerAt(int x, int y, tower::TYPE type) const;
        tower_ptr  spawnTowerAt(Point<int> position, tower::TYPE type) const;
        unit_ptr   spawnUnitAt(int x, int y) const;
        unit_ptr   spawnUnitAt(Point<int> position) const;
		gameobject_ptr spawnObjectAt(gameobject::OBJECT_TYPE type, int x, int y) const;
		gameobject_ptr spawnObjectAt(gameobject::OBJECT_TYPE type, Point<int> position) const;
        void moveTower(tower_ptr tower, Point<int> point) const;

		void runScenario(int scenario);

		void clearTowers() const;
		void spawnTowers(std::vector<Point<int>> tower_list) const;
		std::vector<tower_ptr> findNearestTowers(Point<int> point);
		void parseCVList(std::vector<Point<int>> list);

		int getScreenWidth();
		int getScreenHeight();
    };
}

#endif //GAMECONTROLLER_H