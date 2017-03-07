#include "../include/gamecontroller.h"
#include "../include/manager.h"
#include "../include/network/Buffer.h"
#include "../include/unit.h"
#include "../include/tower.h"
#include "../include/GameObjects/Spawn.h"
#include "../include/GameObjects/Projectiles.h"

namespace gamecontroller {

    /*
        Generate grid linkage
    
    */
    void GameController::preparePathfindingGrid() {
        for (int i = 0; i < TILE_W * TILE_H; i++) {

            // Clear node links:
            nodes[i].clearNodes();
            

            int x = i % TILE_W;
            int y = (i) / TILE_W;

            // Set position of node
            nodes[i].setPosition(x, y);

            if (x > 0) {
                nodes[i].addNode(&nodes[y * TILE_W + x - 1]);
            }

            if (y > 0) {
                nodes[i].addNode(&nodes[(y - 1) * TILE_W + x]);
            }

            if (x < TILE_W - 1) {
                nodes[i].addNode(&nodes[y * TILE_W + x + 1]);
            }

            if (y < TILE_H - 1) {
                nodes[i].addNode(&nodes[(y + 1) * TILE_W + x]);
            }
        }
    }


    GameController::GameController(Manager* m) : manager(m) {
        preparePathfindingGrid();
    }

    int GameController::getWeight(int x, int y){
        //Return negative for an obstacle
        return 0;
    }

    unit_ptr GameController::spawnUnitAt(int x, int y, unit::TYPE type) const {
        auto unit = manager->createUnit(type);
        unit->setPosition(x, y);
        return unit;
    }

    unit_ptr GameController::spawnUnitAt(Point<int> position, unit::TYPE type) const {
        return spawnUnitAt(position.x, position.y, type);
    }


    tower_ptr GameController::spawnTowerAt(int x, int y, tower::TYPE type) const {
        auto tower = manager->createTower(type);
        //printf("Tower spawned at: %d %d\n", x, y);
        tower->setPosition(x, y);
        return tower;
    }

    tower_ptr GameController::spawnTowerAt(Point<int> position, tower::TYPE type) const {
        tower_ptr tower = spawnTowerAt( (int) (round(((float)position.x)/64.0f)*64.0f), (int) (round(((float)position.y)/64.0f)*64.0f), type );
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

    bool GameController::step() {

        frame_clock++;
        increaseWealth(std::log10(frame_clock));
        if (frame_clock % 100 == 0) {
            std::cout << wealth << "\n";
        }
        //In general, step() should be frame-based.
        


        // Perform 
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (!spawned) {

                sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                if (mouse_pos.x >= 0 && mouse_pos.x <= manager->getRenderManager()->getWindowWidth() &&
                    mouse_pos.y >= 0 && mouse_pos.y <= manager->getRenderManager()->getWindowHeight()) {
                    spawnUnitAt(mouse_pos.x, mouse_pos.y, unit::TYPE::BASIC);
                }
               // spawned = true;
            }
        } else 
        // TEMP: Spawn bomb
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
            sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
            if (mouse_pos.x >= 0 && mouse_pos.x <= manager->getRenderManager()->getWindowWidth() &&
                mouse_pos.y >= 0 && mouse_pos.y <= manager->getRenderManager()->getWindowHeight()) {
                spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_BOMB, Point<int>(mouse_pos.x, mouse_pos.y));
            }

            spawned = true;
        } else
        // TEMP: TESTING SPAWNING OF ELECTRICITY
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            if (!spawned) {
                sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                if (mouse_pos.x >= 0 && mouse_pos.x <= manager->getRenderManager()->getWindowWidth() &&
                    mouse_pos.y >= 0 && mouse_pos.y <= manager->getRenderManager()->getWindowHeight()) {

                    auto units = getNNearestUnits(glm::vec2(mouse_pos.x, mouse_pos.y), 1000, 1000);
                    if (units.size() > 1) {
                       // std::cout << "ELECTRICITY!!" << std::endl;
                        gameobject_ptr obj = spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_ELECTRICITY, Point<int>(mouse_pos.x, mouse_pos.y));
                        smartpointers::slave_ptr<ProjectileElectricity> elec = smartpointers::static_pointer_cast<ProjectileElectricity>(obj);
                        elec->setForkParent(units[0].second->getSharedPtr());
                        elec->setTargetObject(units[1].second);
                    }
                }

                spawned = true;
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) || sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) || sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
            if (!spawned) {
                sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                if (mouse_pos.x >= 0 && mouse_pos.x <= manager->getRenderManager()->getWindowWidth() &&
                    mouse_pos.y >= 0 && mouse_pos.y <= manager->getRenderManager()->getWindowHeight()) {

                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) spawnTowerAt(mouse_pos.x, mouse_pos.y, tower::TYPE::BASIC);
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) spawnTowerAt(mouse_pos.x, mouse_pos.y, tower::TYPE::ELECTRIC);
                }

                spawned = true;
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
            //TODO: clear towers/units

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
                //Exit Game
                std::cout << "EXITING GAME (SUPPOSEDLY)\n";
                return false;
            }
            manager->clearTowers();
            return true;
        }

        if (!game_started) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
                game_started = true; //Start game
                if (manager->getTowers().size()) {
                    std::cout << "ERROR! There should not be any towers existing at this point, before base has spawned." << std::endl;
                }
                
                tower_ptr t = spawnTowerAt(getScreenWidth()/2, getScreenHeight()/2, tower::TYPE::BASE); /* !!!VERY IMPORTANT: DO NOT SPAWN ANY TOWERS BEFORE THIS LINE */
                spawnTowerAt(200, 200, tower::TYPE::ELECTRIC);
                frame_clock = 0;
            }
            else {
                return true;
            }
        }

        cvNetworkStep();
        manager->stepAll();

        //Run scenarios for 10 minutes
        if (getElapsedTime() < 60*10) {
            int scenario = (int) (getElapsedTime()/(float)time_per_scenario);
            runScenario(scenario);
        }
        else { //Ending sequence

        }
        return true;
    }

    void GameController::runScenario(int scenario) {
        //std::cout << scenario << " " << wave << " " << getElapsedTime() << "\n";
        if (scenario > this->scenario) {
            //New stage
            this->scenario = scenario;
            this->wave = 0;
            printf("Scenario %d, wave 0.\n", scenario);
            //Start wave 0
            //spawn short burst of troops from each spawn point
            for (auto spawn : spawn_points) {
                spawn->startScenario(scenario);
            }
        }
        else {
            //Continue current scenario
            int wave = (int)(getElapsedTime() * waves_per_scenario / time_per_scenario) % waves_per_scenario;
            if (wave > this->wave) {
                this->wave = wave;
                //Start wave
                printf("Scenario %d, wave %d.\n", scenario, wave);
                //spawn short burst of troops from each spawn point
                for (auto spawn : spawn_points) {
                    spawn->startWave(wave);
                }
            }
            else {
                //Continue wave
                //for (auto spawn : spawn_points) {

                //}
            }
        }
    }

    std::vector<tower_ptr> GameController::findNearestTowers(Point<int> point) {
        //Clone tower list
        std::vector<tower_ptr> points(manager->getTowers());
        std::sort(begin(points), end(points), [point](const tower_ptr& lhs, const tower_ptr& rhs) { return lhs->distanceTo(point) < rhs->distanceTo(point); });
        return points;
    }

    // Returns a list of the units within range
    std::vector<unit_ptr> GameController::getUnitsInRange(glm::vec2 position, int radius) {
        std::vector<unit_ptr> units_in_range;
        units_in_range.clear();
        for (unit_ptr unit : manager->getUnits()) {
            if (unit && glm::distance(unit->getPosition(), position) <= radius) {
                units_in_range.push_back(unit);
            }
        }
        return units_in_range;
    }

    // Returns a list of distance-unit pairs for the N nearest objects
    //  - If there are less than N objects within that range, it ignores them.
    //  - List returned is distance ordered
    std::vector<std::pair<float, unit_ptr>> GameController::getNNearestUnits(glm::vec2 position, int N, int maxrange) {
        std::vector<std::pair<float, unit_ptr>> distance_unit_pairs;

        for (unit_ptr unit : manager->getUnits()) {
            if (unit) {
                float distance = glm::distance(unit->getPosition(), position);
                if (distance <= maxrange) {
                    distance_unit_pairs.push_back(std::pair<float, unit_ptr>(distance, unit));
                }
            }
        }

        // Sort based on distance (closest first)
        std::sort(distance_unit_pairs.begin(), distance_unit_pairs.end(), 
                  [](const std::pair<float, unit_ptr> & a, const std::pair<float, unit_ptr> & b) -> bool {
                        return a.first < b.first;
                  });

        // Trim so only keep first N
        std::vector<std::pair<float, unit_ptr>> keep;
        int count = 0;
        for (auto pair : distance_unit_pairs) {
            if (count >= N) {
                break;
            }
            keep.push_back(pair);
            count++;
        }
        return keep;
    }

    tower_ptr& GameController::getBase() {
        return manager->getTowers()[0];
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

    Matching GameController::stableMatching(std::vector<Point<int>>& detections)
    {
        std::vector<deque<int>> point_prefs;
        std::vector<std::vector<int>> tower_prefs;
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

        size_t tower_count = towers.size(); //base exists and is first tower
        int** dists = new int*[tower_count];
        for (size_t i = 0; i < tower_count; i++)
        {
            dists[i] = new int[detections.size()];
            for (size_t j = 0; j < detections.size(); j++)
            {
                dists[i][j] = (int) towers[i]->distanceTo(detections[j]);
            }
        }

        for (size_t i = 0; i < tower_count; i++)
        {
            tower_prefs.push_back({});
            auto cmp = [&dists, i](int& pref, const int& point)
            {
                return dists[i][pref] < dists[i][point];
            };
            for (size_t j = 0; j < detections.size(); j++)
            {
                auto pos = std::lower_bound(tower_prefs[i].begin(), tower_prefs[i].end(), j, cmp);
                tower_prefs[i].insert(pos, j);
            }
        }

        for (size_t j = 0; j < detections.size(); j++)
        {
            point_prefs.push_back({});
            auto cmp = [&dists, j](int& pref, const int& tower)
            {
                return dists[pref][j] < dists[tower][j];
            };
            for (size_t i = 0; i < tower_count; i++)
            {
                auto pos = std::lower_bound(point_prefs[j].begin(), point_prefs[j].end(), i, cmp);
                point_prefs[j].insert(pos, i);
            }
        }

        for (size_t i = 0; i < tower_count; i++) delete [] dists[i];
        delete [] dists;

        stack<int> free;
        std::vector<Point<int>> news;
        std::vector<int> matches;

        for (size_t j = 0; j < detections.size(); j++) free.push(j);
        for (size_t i = 0; i < tower_count; i++) matches.push_back(NO_MATCH);

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
        for (size_t i = 0; i < matches.size(); i++)
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

    void GameController::increaseWealth(size_t amt) {
        wealth += amt;
    }
    
    int GameController::requestWealth(size_t amt){
        int ret = std::min(amt, wealth);
        wealth -= ret;
        return ret;
    }

    bool GameController::getPath(ivec2 start, ivec2 end, std::vector<vec2>& ret_path) {
        if (start.x < 0 || start.x >= manager->getRenderManager()->getWindowWidth() ||
            start.y < 0 || start.y >= manager->getRenderManager()->getWindowHeight() ||
            end.x < 0 || end.x >= manager->getRenderManager()->getWindowWidth() ||
            end.y < 0 || end.y >= manager->getRenderManager()->getWindowHeight()
            ) {
            return false;
        }
        ret_path.clear();
        std::vector<TileNode*> path;

        int tile_width  = manager->getRenderManager()->getWindowWidth() / TILE_W;
        int tile_height = manager->getRenderManager()->getWindowHeight() / TILE_H;

        start /= vec2(tile_width, tile_height);
        end /= vec2(tile_width, tile_height);

        if (start.x < 0 || start.x >= TILE_W || start.y < 0 || start.y >= TILE_H) { return false; }
        if (end.x < 0 || end.x >= TILE_W || end.y < 0 || end.y >= TILE_H) { return false; }

        std::cout << "PATHING START ID: " << start.x << " END ID: " << end.x << std::endl;

        TileNode& node1 = nodes[start.y*TILE_W + start.x];
        TileNode& node2 = nodes[end.y*TILE_W + end.x];

        path_finder.setStart(node1);
        path_finder.setGoal(node2);
        if (path_finder.findPath<paths::AStar>(path)) {
            for (auto node : path) {
                std::cout << "      " <<node->getX() << " " << node->getY() << std::endl;
                ret_path.emplace_back(node->getX()*tile_width, node->getY()*tile_height);
            }
            return true;
        }
        
        return false;
    }

    float TileNode::distanceTo(AStarNode* node) const 
    {
        int distance = (node->getX() - this->getX())*(node->getX() - this->getX()) + (node->getY() - this->getY())*(node->getY() - this->getY());
        return distance;
    }
    
    TileNode::TileNode() : AStarNode() {}
        
    void TileNode::addNode(TileNode* node) 
    {
        addChild(node, 1.0f);
    }

    void TileNode::clearNodes() {
        this->clearChildren();
    }
}
