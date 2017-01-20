#include "../include/gamecontroller.h"

namespace gamecontroller {
    GameController::GameController(Manager* m) : manager(m) {}

    GameObject* GameController::createObject(id_t key){
        return nullptr;
    }

    unit_ptr GameController::spawnUnitAt(int x, int y) const {
        auto unit = manager->createUnit(unit::TYPE::BASIC);
        unit->setPosition(x, y);
        return unit;
    }

    unit_ptr GameController::spawnUnitAt(Point<int> position) const {
        return spawnUnitAt(position.x, position.y);
    }


    tower_ptr GameController::spawnTowerAt(int x, int y) const {
        auto tower = manager->createTower(tower::TYPE::BASIC);
        tower->setPosition(x, y);
        return tower;
    }

    tower_ptr GameController::spawnTowerAt(Point<int> position) const {
        return spawnTowerAt(position.x, position.y);
    }

    void GameController::restart() const {
        for(auto obj : manager->getTowers()){
            //destroy
        }
        for(auto obj : manager->getUnits()){
            //destroy
        }
    }


    void GameController::init() {
        spawnTowerAt(400, 200);
        spawnTowerAt(800, 200);
        spawnTowerAt(800, 600);
        spawnTowerAt(400, 600);
        spawnUnitAt(100, 50);

		startCVServer();
    }
    
    void GameController::step() {
        manager->stepAll();
		cvNetworkStep();
    }

	////////////////////////////////////////////////////
	// CV Server logic
	void GameController::startCVServer() {
		listener = new sf::TcpListener();
		port     = 31654;

		sf::Socket::Status st = listener->listen(port);
		if (st != sf::Socket::Done) {
			std::cout << "FAILED TO INITIALISE CV INTERFACE SERVER ON" << port << std::endl;
			delete listener;
		}
		else {

			// Success!
			listener->setBlocking(false);
			client = new sf::TcpSocket();
			cvConnectionEstablished = false;
			std::cout << "[CV SERVER] CV Interface Server Started on port: " << port << std::endl;
		}

		// Create buffer
		this->recv_buffer = new Buffer();
	}

	void GameController::cvNetworkStep() {
		if (!cvConnectionEstablished) {
			// Listen for connection from cv Interface:
			sf::Socket::Status st = listener->accept(*client);
			if (st == sf::Socket::Done) {
				std::cout << "Connection to CV Interface established: " << client->getRemotePort() << std::endl;
				cvConnectionEstablished = true;
			} else if (st == sf::Socket::Error || st == sf::Socket::Disconnected) {
				std::cout << "FAILED TO ESTABLISH CONNECTION WITH CV INTERFACE " << st << std::endl;
			}
		} else {
			// Listen for messages from cv interface
			client->setBlocking(false);
			std::size_t message_size;
			sf::TcpSocket::Status st = client->receive((void*)recv_buffer->getPtr(), (std::size_t)recv_buffer->maxSize(), message_size);
			switch (st) {
				case sf::TcpSocket::Done:
				{
					// Data received
					std::cout << "[CV NETWORK] Data received from CV interface. Size: " << message_size << " bytes!" << std::endl;

					// Read packet:
					recv_buffer->seek(0);

					/// .. TODO: Read data sent from cv interface
				}
				break;
			}
		}
	}
}