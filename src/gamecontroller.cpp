#include "../include/gamecontroller.h"
#include <math.h>

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
    	tower_ptr tower = spawnTowerAt( round(((float)position.x)/64.0f)*64.0f, round(((float)position.y)/64.0f)*64.0f );
    	tower->setJitter(position.x - tower->getX(), position.y - tower->getY());
    	return tower;
    }

    void GameController::restart() const {
        for(auto obj : manager->getTowers()){
            //destroy
        }
        for(auto obj : manager->getUnits()){
            //destroy
        }
    }

	void GameController::spawnTowers(std::vector<Point<int>> tower_list) const {
		for (auto location : tower_list) {
			spawnTowerAt(location);
		}
	}

	void GameController::moveTower(tower_ptr tower, Point<int> point) const {
		Point<int> jitter = tower->getJitter();
		int x = point.x - jitter.x;
		int y = point.y - jitter.y;
		tower->setPosition(x, y);
	}

	void GameController::clearTowers() const {
		manager->clearTowers();
	}


    void GameController::init() {
        /*spawnTowerAt(400, 200);
        spawnTowerAt(800, 200);
        spawnTowerAt(800, 600);
        spawnTowerAt(400, 600);
        spawnUnitAt(100, 50);*/

		startCVServer();
    }
    
    void GameController::step() {
        manager->stepAll();
		cvNetworkStep();
    }

	std::vector<tower_ptr> GameController::findNearestTowers(Point<int> point) {
		//Clone tower list
		std::vector<tower_ptr> points(manager->getTowers());
		std::sort(begin(points), end(points), [point](const tower_ptr& lhs, const tower_ptr& rhs) { return lhs->distanceTo(point) < rhs->distanceTo(point); });
		return points;
	}

	void GameController::parseCVList(std::vector<Point<int>> list) {
		// For Demo Only
		clearTowers();
		std::cout << "Towers cleared!" << std::endl;
		spawnTowers(list);

		//Order of Operations: Move > Create > Delete

		//For move
			//Set delete_queue = 0
		
		//For creation:
			//If tower to create:
				//Increment create_queue
				//If > n then actually spawn

		//For deletion:
			//Increase tower->delete_queue
			//If > n then actually delete
		
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
			/*
				In this process, the gamecontroller checks the incoming buffer for any messages
				from cvInterface. If data exists, it reads the first 4 bytes (int) from the stream to
				determine the number of data points received.
				- it then reads out a series of points into cvList. 

				(NOTE: This list is cleared when data is received).
				- At this stage, we can do something with the new list.

				-------------------------------------------
				ADDITIONAL COMMENT:
				- This is just a communication interface. If instead you want the tower moving/creation/deletion to be done on the cv thread,
				then it can be done there, and this can be adjusted to receive the resultant data after that has happened.
				- It is currently setup the way it is as this way, we only need a one-way data exchange with the tower position data being sent from the 
				webcam feed thread to the main app.
			*/
			client->setBlocking(false);
			std::size_t message_size;
			sf::TcpSocket::Status st = client->receive((void*)recv_buffer->getPtr(), (std::size_t)recv_buffer->maxSize(), message_size);
			switch (st) {
				case sf::TcpSocket::Done:
				{
					// Data received
					std::cout << "[CV NETWORK] Data received from CV interface. Size: " << message_size << " bytes!" << std::endl;

					// Clear cv list
					cvList.clear();

					// Read packet:
					int number_of_positions;
					Point<int> point(0,0);

					recv_buffer->seek(0);
					(*recv_buffer) >> number_of_positions;
					for (int c = 0; c < number_of_positions; c++) {
						(*recv_buffer) >> point;
						cvList.push_back(point);
						//std::cout << "\tReceived Point (" << point.x << "," << point.y << ")" << std::endl;
					}


					// TODO: Do something with the new list.
					Matching match = stableMatching(cvList);
					//matches, deleted_towers, new_towers
				}
				break;
			}
		}
	}

	Matching GameController::stableMatching(vector<Point<int>>& detections)
	{
	    vector<deque<int>> point_prefs;
	    vector<vector<int>> tower_prefs;
	    auto& towers = manager->getTowers();
	    int tower_count = towers.size();
	    int** dists = new int*[tower_count];
	    for (size_t i = 0; i < tower_count; i++)
	    {
	        dists[i] = new int[detections.size()];
	        for (size_t j = 0; j < detections.size(); j++)
	        {
	            dists[i][j] = towers[i]->distanceTo(detections[j]);
	            //cout << dists[i][j] << " ";
	        }
	        //cout << "\n";
	    }
	    //cout << "\n";

	    for (int i = 0; i < tower_count; i++)
	    {
	        tower_prefs.push_back({});
	        auto cmp = [&dists, i](int& pref, const int& point)
	        {
	            return dists[i][pref] < dists[i][point];
	        };
	        for (int j = 0; j < detections.size(); j++)
	        {
	            auto pos = std::lower_bound(tower_prefs[i].begin(), tower_prefs[i].end(), j, cmp);
	            tower_prefs[i].insert(pos, j);
	        }
	        //for (int pref : tower_prefs[i]) cout << pref << " ";
	        //cout << "\n";
	    }
	    //cout << "\n";

	    for (int j = 0; j < detections.size(); j++)
	    {
	        point_prefs.push_back({});
	        auto cmp = [&dists, j](int& pref, const int& tower)
	        {
	            return dists[pref][j] < dists[tower][j];
	        };
	        for (int i = 0; i < tower_count; i++)
	        {
	            auto pos = std::lower_bound(point_prefs[j].begin(), point_prefs[j].end(), i, cmp);
	            point_prefs[j].insert(pos, i);
	        }
	        //for (int pref : point_prefs[j]) cout << pref << " ";
	        //cout << "\n";
	    }
	    //cout << "\n";

	    for (int i = 0; i < tower_count; i++) delete [] dists[i];
	    delete [] dists;

	    stack<int> free;
	    vector<Point<int>> news;
	    vector<int> matches;

	    for (int j = 0; j < detections.size(); j++) free.push(j);
	    for (int i = 0; i < tower_count; i++) matches.push_back(NO_MATCH);

	    while (!free.empty())
	    {
	        int p = free.top();
	        if (point_prefs[p].empty())
	        {
	            // Clearly, this is a new tower
	            news.push_back(detections[p]);
	            free.pop();
	            continue;
	        }
	        int t = point_prefs[p].front();
	        point_prefs[p].pop_front();
	        int p_ = matches[t];
	        if (p_ == NO_MATCH)
	        {
	            matches[t] = p;
	            free.pop();
	        }
	        //TODO: Binary Search on both
	        else for (int x : tower_prefs[t])
	        {
	            if (x == p_) break;
	            else if (x == p)
	            {
	                free.pop();
	                free.push(p_);
	                matches[t] = p;
	                break;
	            }
	        }
	    }

	    Matching match;
	    match.new_towers = news;
	    for (int i = 0; i < matches.size(); i++)
	    {
	        if (matches[i] == NO_MATCH)
	        {
	            match.deleted_towers.push_back(towers[i]);
	        }
	        else
	        {
	            match.matches.emplace(towers[i]->getID(), detections[matches[i]]);
	        }
	    }
	    return match;
	}
}