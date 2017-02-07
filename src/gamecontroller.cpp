#include "../include/gamecontroller.h"

namespace gamecontroller {
    GameController::GameController(Manager* m) : manager(m) {}

    /*GameObject* GameController::createObject(id_t key){
        return nullptr;
    }*/

    int GameController::getWeight(int x, int y){
        //Return negative for an obstacle
        return 0;
    }

    unit_ptr GameController::spawnUnitAt(int x, int y) const {
        auto unit = manager->createUnit(unit::TYPE::BASIC);
        unit->setPosition(x, y);
        return unit;
    }

    unit_ptr GameController::spawnUnitAt(Point<int> position) const {
        return spawnUnitAt(position.x, position.y);
    }


    tower_ptr GameController::spawnTowerAt(int x, int y, tower::TYPE type) const {
        auto tower = manager->createTower(type);
        tower->setPosition(x, y);
        return tower;
    }

    tower_ptr GameController::spawnTowerAt(Point<int> position, tower::TYPE type) const {
        tower_ptr tower = spawnTowerAt( round(((float)position.x)/64.0f)*64.0f, round(((float)position.y)/64.0f)*64.0f, type );
        tower->setJitter(position.x - tower->getX(), position.y - tower->getY());
        return tower;
    }

    gameobject_ptr GameController::spawnObjectAt(gameobject::OBJECT_TYPE type, int x, int y) const {
        // TODO: Make spawn
        gameobject_ptr game_object = manager->createObject(type);
        game_object->setPosition(x, y);
        return game_object;
    }

    gameobject_ptr GameController::spawnObjectAt(gameobject::OBJECT_TYPE type, Point<int> position) const {
        return spawnObjectAt(type, position.x, position.y);
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
            spawnTowerAt(location, tower::TYPE::BASIC);
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

    float GameController::getElapsedTime() const {
        return (float)frame_clock / 60.0f;
    }

    void GameController::resetClock() {
        frame_clock = 0;
    }

    void GameController::init() {
        /*spawnTowerAt(400, 200);
        spawnTowerAt(800, 200);
        spawnTowerAt(800, 600);
        spawnTowerAt(400, 600);*/
        //spawnUnitAt(100, 50);

        //Gotta love smartpointers, look how clean this is :^)
        spawn_points.push_back(smartpointers::static_pointer_cast<Spawn>(spawnObjectAt(gameobject::OBJECT_TYPE::SPAWN, 0, 0)));
        spawn_points.push_back(smartpointers::static_pointer_cast<Spawn>(spawnObjectAt(gameobject::OBJECT_TYPE::SPAWN, 1232, 0)));
        spawn_points.push_back(smartpointers::static_pointer_cast<Spawn>(spawnObjectAt(gameobject::OBJECT_TYPE::SPAWN, 0, 672)));
        spawn_points.push_back(smartpointers::static_pointer_cast<Spawn>(spawnObjectAt(gameobject::OBJECT_TYPE::SPAWN, 1232, 672)));


        startCVServer();
    }

    void GameController::step() {

		frame_clock++;
		//In general, step() should be frame-based.
        


		// Perform 
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			if (!spawned) {
				sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
				spawnUnitAt(mouse_pos.x, mouse_pos.y);
				//spawned = true;
			}
		} else {
			spawned = false;
		}

		//In general, step() should be frame-based.
		cvNetworkStep();
        manager->stepAll();
        manager->collisionAll();

        //Assumes 60 frames = 1000ms roughly... it's not but we'll work with that timing
        float delta = getElapsedTime();

        //Scenario: 10 stages, 3 waves per stage. 1 minute per stage
        //TODO: implement Stage class
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            //Restart Game
            game_started = false;
            wave = 0;
            scenario = 0;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
                //Exit Game
            }
            return;
        }

        if (!game_started) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
                game_started = true; //Start game
                if (manager->getTowers().size()) {
                    std::cout << "ERROR! There should not be any towers existing at this point, before base has spawned." << std::endl;
                }
                tower_ptr t = spawnTowerAt(400, 200, tower::TYPE::BASE); /* !!!VERY IMPORTANT: DO NOT SPAWN ANY TOWERS BEFORE THIS LINE */
                frame_clock = 0;
            }
            else {
                return;
            }
        }

        cvNetworkStep();
        manager->stepAll();

        //Run scenarios for 10 minutes
        if (getElapsedTime() < 60*10) {
            int scenario = getElapsedTime()/60.0f;
            runScenario(scenario);
        }
        else { //Ending sequence

        }
    }

    void GameController::runScenario(int scenario) {
        if (scenario > this->scenario) {
            //New stage
            this->scenario = scenario;
            this->wave = 0;
            //Start wave 0
            //spawn short burst of troops from each spawn point
            for (auto spawn : spawn_points) {

            }
        }
        else {
            //Continue current scenario
            int wave = ((int)getElapsedTime() % 60)/3;
            if (wave > this->wave) {
                this->wave = wave;
                //Start wave
                //spawn short burst of troops from each spawn point
                for (auto spawn : spawn_points) {

                }
            }
            else {
                //Continue wave
                for (auto spawn : spawn_points) {

                }
            }
        }
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
                    //std::cout << "[CV NETWORK] Data received from CV interface. Size: " << message_size << " bytes!" << std::endl;

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
                    //Order of Operations: Move > Create > Delete

                    //For move

                    for(auto& result : match.matches){
                        result.first->setPosition((result.second).x, (result.second).y);
                        result.first->delete_queue = 0;
                    }

                    //For creation:
                        //If tower to create:
                            //Increment create_queue
                            //If > n then actually spawn
                    if(match.new_towers.size() > 0){
                        create_count++;
                        if(create_count > 10){
                            spawnTowerAt(match.new_towers[0], tower::TYPE::BASIC);
                        }
                    }else{
                        create_count = 0;
                    }

                    //For deletion:
                        //Increase tower->delete_queue
                        //If > n then actually delete
                    for(auto tower : match.deleted_towers){
                        tower->delete_queue++;
                        if(tower->delete_queue > 200){
                            tower->demoDestroy();
                            break; //Have to break I think to prevent seg-fault
                        }
                    }
                }
                break;
            }
        }
    }

    Matching GameController::stableMatching(vector<Point<int>>& detections)
    {
        vector<deque<int>> point_prefs;
        vector<vector<int>> tower_prefs;
        auto& _towers = manager->getTowers();

        std::vector<tower_ptr> towers;
        if (_towers.size()) {
            if (_towers[0]->getSubType() != tower::TYPE::BASE) {
                std::cout << "ERROR! 1st tower is not base" << std::endl;
            }
            std::vector<tower_ptr>::const_iterator begin = _towers.begin();
            std::vector<tower_ptr>::const_iterator last = _towers.begin() + _towers.size();
            towers = std::vector<tower_ptr>(begin+1, last); //skip first tower without needing to change Jamie's algorithm
        }
        else {
            towers = std::vector<tower_ptr>();
        }

        int tower_count = towers.size(); //base exists and is first tower
        int** dists = new int*[tower_count];
        for (size_t i = 0; i < tower_count; i++)
        {
            dists[i] = new int[detections.size()];
            for (size_t j = 0; j < detections.size(); j++)
            {
                dists[i][j] = towers[i]->distanceTo(detections[j]);
            }
        }

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
        }

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
        }

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
                match.matches.emplace(towers[i], detections[matches[i]]);
            }
        }
        return match;
    }

    int GameController::getScreenWidth() {
        return sf::VideoMode::getDesktopMode().width;
    }

    int GameController::getScreenHeight() {
        return sf::VideoMode::getDesktopMode().height;
    }
}
