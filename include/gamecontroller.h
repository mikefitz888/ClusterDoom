#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "gameobject.h"
#include "manager.h"
#include "../include/network/Buffer.h"
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <vector>

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
    using gameobject::Point;
    using manager::Manager;
    class GameController {
        Manager* manager;
        std::vector<Point<int>> cvList;
		
		// CV Network
		int                port;
		sf::TcpListener   *listener;
		Buffer            *recv_buffer;
		sf::TcpSocket     *client;
		bool              cvConnectionEstablished;

		void startCVServer();
		void cvNetworkStep();

        int create_count = 0;

    public:
        GameController(Manager* m);
        GameObject* createObject(id_t key);
        void init();
        void step();
        void restart() const;

        tower_ptr spawnTowerAt(int x, int y) const;
        tower_ptr spawnTowerAt(Point<int> position) const;
        unit_ptr spawnUnitAt(int x, int y) const;
        unit_ptr spawnUnitAt(Point<int> position) const;
        void moveTower(tower_ptr tower, Point<int> point) const;

		void clearTowers() const;
		void spawnTowers(std::vector<Point<int>> tower_list) const;
		std::vector<tower_ptr> findNearestTowers(Point<int> point);
		void parseCVList(std::vector<Point<int>> list);
    };
}

#endif //GAMECONTROLLER_H