#include "../include/gamecontroller.h"
#include "../include/manager.h"
#include "../include/network/Buffer.h"
#include "../include/unit.h"
#include "../include/GameObjects/Spawn.h"
#include "../include/GameObjects/Projectiles.h"
#include "../include/Towers/Base.h"
#include "../include/Towers/SpecialTower.h"
#include "../include/AttackerEffects.h"

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
        efficiencyModifier = 1.0;
        preparePathfindingGrid();
    }

    int GameController::getWeight(int x, int y) {
        //Return negative for an obstacle
        return 0;
    }

    unit_ptr GameController::spawnUnitAt(float x, float y, unit::TYPE type) const {
        auto unit = manager->createUnit(type, x, y);
        //unit->setPosition(x, y);
        return unit;
    }

    unit_ptr GameController::spawnUnitAt(Point<float> position, unit::TYPE type) const {
        return spawnUnitAt(position.x, position.y, type);
    }


    tower_ptr GameController::spawnTowerAt(float x, float y, tower::TYPE type) const {
        auto tower = manager->createTower(type, x, y);
        //printf("Tower spawned at: %d %d\n", x, y);
        //tower->setPosition(x, y);
        return tower;
    }

    tower_ptr GameController::spawnTowerAt(Point<float> position, tower::TYPE type) const {
        tower_ptr tower = spawnTowerAt(round((position.x) / 64.0f)*64.0f, round((position.y) / 64.0f)*64.0f, type);
        tower->setJitter((int)(position.x - tower->getX()), (int)(position.y - tower->getY()));
        return tower;
    }

    gameobject_ptr GameController::spawnObjectAt(gameobject::OBJECT_TYPE type, float x, float y) const {
        // TODO: Make spawn
        gameobject_ptr game_object = manager->createObject(type, x, y);
        return game_object;
    }

    gameobject_ptr GameController::spawnObjectAt(gameobject::OBJECT_TYPE type, Point<float> position) const {
        return spawnObjectAt(type, position.x, position.y);
    }

    void GameController::restart()  {

        // Destroy all game objects
        for (auto& obj : manager->getObjects()) {
            obj->demoDestroy();
        }

        // Clean manager
        manager->reset();


        // Reset spawn flags to trigger respawn
        initial_spawns_occurred = false;
    }

    void GameController::spawnTowers(std::vector<std::pair<Point<float>, int>> tower_list) const {
        for (auto location : tower_list) {
            switch (location.second) {
                case 0:
                case 1:
                    spawnTowerAt(location.first, tower::TYPE::BASIC);
                    break;
                case 2:
                case 3:
                case 4:
                    spawnTowerAt(location.first, tower::TYPE::ELECTRIC);
                    break;

            }
        }
    }

    void GameController::moveTower(tower_ptr tower, Point<float> point) const {
        Point<int> jitter = tower->getJitter();
        float x = point.x - jitter.x;
        float y = point.y - jitter.y;
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

    GameState GameController::getGameState() { return current_state; }
    GameState GameController::startGame() {
        create_count = 0; //Reset game variables
        frame_clock = 0;
        wave = 0;
        scenario = -1;
        tower_efficiency = 1;
        return current_state = GameState::RUNNING;
    }
    GameState GameController::stopGame() { return current_state = GameState::START; }
    GameState GameController::winGame() { return current_state = GameState::WIN; }
    GameState GameController::loseGame() { return current_state = GameState::LOSE; }


    void GameController::init() { //Recommend not touching any game objects in this function
        startCVServer();
    }

    bool GameController::step() {
        srand(time(NULL));
       
        //In general, step() should be frame-based.

        switch (getGameState()) {
#

            /*
                START game state
                -------------------
                This is the gamestate just before the game starts.
                Pressing enter will start the game
            
            */
            case GameState::START:
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
                    startGame();

                    if (!initial_spawns_occurred) {
                        initial_spawns_occurred = true;
                        if (manager->getTowers().size()) {
                            std::cout << "ERROR! There should not be any towers existing at this point, before base has spawned." << std::endl;
                        }

                        tower_ptr t = spawnTowerAt(manager->getRenderManager()->getWindowWidth() / 2, manager->getRenderManager()->getWindowHeight() / 2, tower::TYPE::BASE); /* !!!VERY IMPORTANT: DO NOT SPAWN ANY TOWERS BEFORE THIS LINE */

                        // Ensure spawn list is clear
                        spawn_points.clear();

                        // Create spawns                                                                                                                                     //Adding Spawn Points
                        spawn_points.push_back(smartpointers::static_pointer_cast<Spawn>(spawnObjectAt(gameobject::OBJECT_TYPE::SPAWN, 0, 0)));
                        spawn_points.back()->setSpawnID(0);
                        spawn_points.push_back(smartpointers::static_pointer_cast<Spawn>(spawnObjectAt(gameobject::OBJECT_TYPE::SPAWN, 1232, 0)));
                        spawn_points.back()->setSpawnID(1);
                        spawn_points.push_back(smartpointers::static_pointer_cast<Spawn>(spawnObjectAt(gameobject::OBJECT_TYPE::SPAWN, 0, 672)));
                        spawn_points.back()->setSpawnID(2);
                        spawn_points.push_back(smartpointers::static_pointer_cast<Spawn>(spawnObjectAt(gameobject::OBJECT_TYPE::SPAWN, 1232, 672)));
                        spawn_points.back()->setSpawnID(3);

                        // Add notifier
                        spawnObjectAt(gameobject::OBJECT_TYPE::GAME_STATE_NOTIFIER, 0, 0);
                    }

                    frame_clock = 0;
                } else {
                    return true;
                }
            }

            /*
                RUNNING STATE
                -----------------------------------
                This is the game state for when the game is running. 
                All gameobject simulation, processing and rendering should be done when this state is live.
            */
            case GameState::RUNNING:
            {
                // Perform clock:
                frame_clock++;

                // Efficiency management
                diminishEfficiency();
                if (frame_clock % 100 == 0) {
                    std::cout << "Efficiency = " << tower_efficiency << "\n";
                }

                // Resource spawning
                resource_spawn_timer--;
                if (resource_spawn_timer <= 0) {
                    resource_spawn_timer = glm::linearRand(resource_spawn_timer_min, resource_spawn_timer_max);

                    // Spawn a bit of resource
                    glm::vec2 rand_pos = glm::linearRand(glm::vec2(100.0f, 100.0f), glm::vec2(manager->getRenderManager()->getWindowWidth() - 100.0f, manager->getRenderManager()->getWindowHeight() - 100.0f));

                    // Query number of mines
                    int count = 0;
                    auto objects = manager->getObjects();
                    for (auto obj : objects) {
                        if (obj && obj->getSuperType() == gameobject::TYPE::OBJECT && obj->getSubType() == gameobject::OBJECT_TYPE::RESOURCE_MINE) {
                            count++;
                        }
                    }
                    if (count < max_resource_on_map) {
                        spawnObjectAt(gameobject::OBJECT_TYPE::RESOURCE_MINE, rand_pos.x, rand_pos.y);
                    }
                }

                // Token Spawning
                token_timer--;
                if (token_timer <= 0) {
                    token_timer = glm::linearRand(token_timer_min, token_timer_max);

                    glm::vec2 rand_pos = glm::linearRand(glm::vec2(75.0f, 75.0f), glm::vec2(manager->getRenderManager()->getWindowWidth() - 75.0f, manager->getRenderManager()->getWindowHeight() - 75.0f));

                    bool canspawn = true;
                    for (auto obj : manager->getObjects()) {
                        if (obj && obj->getSuperType() == gameobject::TYPE::OBJECT) {
                            if (obj->getSubType() == gameobject::OBJECT_TYPE::TOKEN_GLACIAL
                                || obj->getSubType() == gameobject::OBJECT_TYPE::TOKEN_MAGNETIC
                                || obj->getSubType() == gameobject::OBJECT_TYPE::TOKEN_WINDY) {
                                canspawn = false;
                                break;
                            }
                        }
                    }

                    if (canspawn) {
                        gameobject::OBJECT_TYPE type = (gameobject::OBJECT_TYPE) glm::linearRand<int>(gameobject::OBJECT_TYPE::TOKEN_MAGNETIC, gameobject::OBJECT_TYPE::TOKEN_WINDY);
                        spawnObjectAt(type, rand_pos.x, rand_pos.y);
                    }

                }

                // ------------------------------------------------------------------------------------ //
                // DEBUG CONTROLS
                if (this->manager->getRenderManager()->getWindow()->hasFocus()) {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)) {
                        if (!spawned) {

                            sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                            if (mouse_pos.x >= 0 && mouse_pos.x <= manager->getRenderManager()->getWindowWidth() &&
                                mouse_pos.y >= 0 && mouse_pos.y <= manager->getRenderManager()->getWindowHeight()) {
                                spawnUnitAt((float)mouse_pos.x, (float)mouse_pos.y, unit::TYPE::BASIC);
                            }
                            //spawned = true;
                        }
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                        if (!spawned) {

                            sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                            if (mouse_pos.x >= 0 && mouse_pos.x <= manager->getRenderManager()->getWindowWidth() &&
                                mouse_pos.y >= 0 && mouse_pos.y <= manager->getRenderManager()->getWindowHeight()) {
                                spawnObjectAt(gameobject::OBJECT_TYPE::RESOURCE_MINE, mouse_pos.x, mouse_pos.y);
                            }
                            spawned = true;
                        }
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
                        sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                        if (mouse_pos.x >= 0 && mouse_pos.x <= manager->getRenderManager()->getWindowWidth() &&
                            mouse_pos.y >= 0 && mouse_pos.y <= manager->getRenderManager()->getWindowHeight()) {
                            spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_BOMB, Point<float>((float)mouse_pos.x, (float)mouse_pos.y));
                        }

                        spawned = true;
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
                        sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                        if (mouse_pos.x >= 0 && mouse_pos.x <= manager->getRenderManager()->getWindowWidth() &&
                            mouse_pos.y >= 0 && mouse_pos.y <= manager->getRenderManager()->getWindowHeight()) {
                            auto obj = spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_LASER, Point<float>((float)mouse_pos.x, (float)mouse_pos.y));

                            // Find nearest object
                            auto objs = this->manager->getGameController()->getNNearestUnits(obj->getPosition(), 1, 1000);
                            if (objs.size() > 0) {
                                glm::vec2 dir = glm::normalize(objs[0].second->getPosition() - obj->getPosition());
                                obj->setVelocity(dir*7.0f);
                            }
                        }

                        spawned = true;
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
                        if (!spawned) {
                            sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                            if (mouse_pos.x >= 0 && mouse_pos.x <= manager->getRenderManager()->getWindowWidth() &&
                                mouse_pos.y >= 0 && mouse_pos.y <= manager->getRenderManager()->getWindowHeight()) {

                                auto units = getNNearestUnits(glm::vec2(mouse_pos.x, mouse_pos.y), 1000, 1000);
                                if (units.size() > 1) {
                                    // std::cout << "ELECTRICITY!!" << std::endl;
                                    gameobject_ptr obj = spawnObjectAt(gameobject::OBJECT_TYPE::PROJECTILE_ELECTRICITY, Point<float>((float)mouse_pos.x, (float)mouse_pos.y));
                                    smartpointers::slave_ptr<ProjectileElectricity> elec = smartpointers::static_pointer_cast<ProjectileElectricity>(obj);
                                    elec->setForkParent(units[0].second->getSharedPtr());
                                    elec->setTargetObject(units[1].second);
                                }
                            }

                            spawned = true;
                        }
                    }
                    // Attacker Effects
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                    {
                        if (!spawned) {
                            sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                            if (mouse_pos.x >= 0 && mouse_pos.x <= manager->getRenderManager()->getWindowWidth() &&
                                mouse_pos.y >= 0 && mouse_pos.y <= manager->getRenderManager()->getWindowHeight()) {
                                spawnObjectAt(gameobject::OBJECT_TYPE::EFFECT_HEAL, Point<float>((float)mouse_pos.x, (float)mouse_pos.y));

                            }

                            spawned = true;
                        }
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)
                               || sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)
                               || sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)
                               || sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)
                               || sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                            for (auto tower : manager->getTowers()) {
                                if (tower->getSubType() == tower::TYPE::SPECIAL) {
                                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
                                        smartpointers::static_pointer_cast<tower::SpecialTower>(tower)->changeEffect(tower::SPECIAL_TYPE::NOEFFECT);
                                        std::cout << "Special towers set to NOEFFECT" << std::endl;
                                    }
                                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
                                        smartpointers::static_pointer_cast<tower::SpecialTower>(tower)->changeEffect(tower::SPECIAL_TYPE::MAGNETIC);
                                        std::cout << "Special towers set to MAGNETIC" << std::endl;
                                    }
                                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
                                        smartpointers::static_pointer_cast<tower::SpecialTower>(tower)->changeEffect(tower::SPECIAL_TYPE::GLACIAL);
                                        std::cout << "Special towers set to GLACIAL" << std::endl;
                                    }
                                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
                                        smartpointers::static_pointer_cast<tower::SpecialTower>(tower)->changeEffect(tower::SPECIAL_TYPE::WINDY);
                                        std::cout << "Special towers set to WINDY" << std::endl;
                                    }
                                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
                                        tower->demoDestroy();
                                    }
                                }
                            }
                        } else if (!spawned) {
                            sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                            if (mouse_pos.x >= 0 && mouse_pos.x <= manager->getRenderManager()->getWindowWidth() &&
                                mouse_pos.y >= 0 && mouse_pos.y <= manager->getRenderManager()->getWindowHeight()) {

                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) spawnTowerAt((float)mouse_pos.x, (float)mouse_pos.y, tower::TYPE::BASIC);
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) spawnTowerAt((float)mouse_pos.x, (float)mouse_pos.y, tower::TYPE::ELECTRIC);
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) spawnTowerAt((float)mouse_pos.x, (float)mouse_pos.y, tower::TYPE::BOMB);
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) spawnTowerAt((float)mouse_pos.x, (float)mouse_pos.y, tower::TYPE::LASER);
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
                                    tower_ptr special = nullptr;
                                    for (auto tower : manager->getTowers()) {
                                        if (tower && tower->getSubType() == tower::TYPE::SPECIAL) {
                                            special = tower;
                                            break;
                                        }
                                    }
                                    if (special == nullptr) spawnTowerAt((float)mouse_pos.x, (float)mouse_pos.y, tower::TYPE::SPECIAL);
                                    else special->setPosition(glm::vec2((float)mouse_pos.x, (float)mouse_pos.y));
                                }
                            }

                            spawned = true;
                        }
                    } else {
                        spawned = false;
                    }
                }

                /////////////////////////////////////////////////////////////////////////
                //Run scenarios for 5 minutes
                if (getElapsedTime() < 60 * 5) {
                    int scenario = (int)(getElapsedTime() / (float)time_per_scenario);
                    runScenario(scenario);
                } else { //Ending sequence
                    if (!manager->getUnits().size()) {
                        winGame();
                    }
                }

                /////////////////////////////////////////////////////////////////////////
                // PERFORM MANAGER STEP AND COLLISIONS
                cvNetworkStep();
                manager->stepAll();
                manager->collisionAll();
                manager->stepAll();
            }
            break;


            /*
                Menu State
                -----------------------------------
                The main menu state is simply used for visiting other menu screens.
            */
            case GameState::MAIN_MENU:

                if (this->manager->getRenderManager()->getWindow()->hasFocus()) {

                    // Get mouse
                    float width = (float)this->manager->getRenderManager()->getWindowWidth();
                    float height = (float)this->manager->getRenderManager()->getWindowHeight();
                    float sf = width / 1920.0f;
                    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                    mouse_pos.x = width - mouse_pos.x;

                    // Btn presses
                    bool hoverbtn1 = false;
                    bool hoverbtn2 = false;
                    if (mouse_pos.x >= 244.0f * sf && mouse_pos.x <= 644.0f * sf && mouse_pos.y >= 577.0f * sf && mouse_pos.y <= 977.0f * sf) {
                        hoverbtn1 = true;
                    }
                    if (mouse_pos.x >= 1164.0f * sf && mouse_pos.x <= 1564.0f * sf && mouse_pos.y >= 577.0f * sf && mouse_pos.y <= 977.0f * sf) {
                        hoverbtn2 = true;
                    }

                    // Mouse press
                    bool mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
                    if (hoverbtn1) {
                        std::cout << "O:" << std::endl;
                    }
                    // Hotkeys
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) || (hoverbtn1 && mouse_pressed)) {

                        // GOTO Single player lobby
                        this->current_state = GameState::MENU_LOBBY_SP;

                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) || (hoverbtn2 && mouse_pressed)) {

                        // GOTO Multiplayer lobby
                        this->current_state = GameState::MENU_LOBBY_MP;
                    }
                }
            break;

            /*
                Single-player lobby
                -----------------------------------
                Single player lobby. Nothing much needs to happen here. This will configure the game for
                single-player mode. The game will not be start-able until the CV interface is connected.
            */
            case GameState::MENU_LOBBY_SP:

                
                if (cvConnectionEstablished) {
                    if (this->manager->getRenderManager()->getWindow()->hasFocus() &&
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {

                        // Set gamemode to single player
                        current_gamemode = GameMode::SINGLE_PLAYER;

                        // Move into start state:
                        this->current_state = GameState::START;

                    }
                }
            break;

            /*
                Multi-player lobby
                -----------------------------------
                Multiplayer lobby. Will display other connected players.
            */
            case GameState::MENU_LOBBY_MP:


                if (cvConnectionEstablished) {
                    if (this->manager->getRenderManager()->getWindow()->hasFocus() && 
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {

                        // Set gamemode to multiplayer
                        current_gamemode = GameMode::MULTI_PLAYER;

                        // Move into start state:
                        this->current_state = GameState::START;
                    }
                }
            break;


        }

        // ---------------- CODE FOR ALL STATES ----------------------
        //Assumes 60 frames = 1000ms roughly... it's not but we'll work with that timing
        float delta = getElapsedTime();

        // Check for restart
        if (this->manager->getRenderManager()->getWindow()->hasFocus() &&  
            sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            //Restart Game
            stopGame();
            wave = 0;
            scenario = 0;
            //TODO: clear towers/units

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
                //Exit Game
                std::cout << "EXITING GAME (SUPPOSEDLY)\n";
                return false;
            }
            restart();

            // Return to menu
            current_state = GameState::MAIN_MENU;

            return true;
        }


        // ---------------- PROCESS CONNECTION TO CV INTERFACE ----------------------
        cvNetworkStep();


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
        } else {
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
            } else {
                //Continue wave
                //for (auto spawn : spawn_points) {

                //}
            }
        }
    }

    std::vector<tower_ptr> GameController::findNearestTowers(Point<float> point) {
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
            if (unit && glm::distance(unit->getPosition(), position) <= radius && !unit->isDead()) {
                units_in_range.push_back(unit);
            }
        }
        return units_in_range;
    }

    // Returns a list of the towers within range
    std::vector<tower_ptr> GameController::getTowersInRange(glm::vec2 position, int radius) {
        std::vector<tower_ptr> towers_in_range;
        towers_in_range.clear();
        for (tower_ptr tower : manager->getTowers()) {
            if (tower && glm::distance(tower->getPosition(), position) <= radius) {
                towers_in_range.push_back(tower);
            }
        }
        return towers_in_range;
    }

    // Returns a list of distance-unit pairs for the N nearest objects
    //  - If there are less than N objects within that range, it ignores them.
    //  - List returned is distance ordered
    std::vector<std::pair<float, unit_ptr>> GameController::getNNearestUnits(glm::vec2 position, int N, int maxrange) {
        std::vector<std::pair<float, unit_ptr>> distance_unit_pairs;

        for (unit_ptr unit : manager->getUnits()) {
            if (unit && !unit->isDead()) {
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

    void GameController::parseCVList(std::vector<std::pair<Point<float>, int>> list) {
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
        port = 31654;

        sf::Socket::Status st = listener->listen(port);
        if (st != sf::Socket::Done) {
            std::cout << "FAILED TO INITIALISE CV INTERFACE SERVER ON" << port << std::endl;
            delete listener;
        } else {

            // Success!
            listener->setBlocking(false);
            client = new sf::TcpSocket();
            cvConnectionEstablished = false;
            std::cout << "[CV SERVER] CV Interface Server Started on port: " << port << std::endl;
        }

        // Create buffer
        this->recv_buffer = new Buffer();
        this->send_buffer = new Buffer();
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
                delete listener;
                delete this->recv_buffer;
                delete this->send_buffer;
                startCVServer();

            }
        } else {

            // Ping CV Interface (to check if still connected)
            ping_timer--;
            if (ping_timer <= 0) {
                ping_timer = 20;

                send_buffer->seek(0);
                (*send_buffer) << (int)0;

                size_t data;
                sf::Socket::Status status = client->send((void*)send_buffer->getPtr(), (std::size_t)send_buffer->tell(), data);
                if (status == sf::Socket::Disconnected || status == sf::Socket::Error) {

                    // Disconnect client
                    client->disconnect();
                    delete client;
                    client = new sf::TcpSocket();
                    cvConnectionEstablished = false;
                }
            }
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
                    for (auto& vec : cvList) {
                        vec.clear();
                    }

                    // Read packet:
                    int number_of_positions;
                    int x, y, marker_type;

                    recv_buffer->seek(0);
                    (*recv_buffer) >> number_of_positions;
                    for (int c = 0; c < number_of_positions; c++) {
                        (*recv_buffer) >> x;
                        (*recv_buffer) >> y;
                        (*recv_buffer) >> marker_type;
                        cvList[marker_type].push_back(Point<float>(x, y));
                        //std::cout << "\tReceived Point (" << x << "," << y << "): " << marker_type << std::endl;
                    }

                    // Separate cvList by marker_type, run stableMatching on each, merge matchings
                    std::vector<tower_ptr> delete_list;
                    int ccc = create_count;
                    for (int type = 0; type < tower::TYPE::num_types; type++) {
                        //For move
                        Matching match = stableMatching((tower::TYPE) type, cvList[type]);
                        for (auto& result : match.matches) {
                            result.first->setPosition((result.second).x, (result.second).y);
                            result.first->delete_queue = 0;
                        }

                        //For creation:
                        //If tower to create:
                        //Increment create_queue
                        //If > n then actually spawn
                        if (match.new_towers.size() > 0) {
                            create_count++;
                            if (create_count > 10) {
                                spawnTowerAt(match.new_towers[0], (tower::TYPE) type);
                            }
                        }

                        //Increase a tally for each tower, only delete a tower if it's in none of the sets
                        for (auto& tower : match.deleted_towers) {
                            delete_list.push_back(tower);
                        }
                    }

                    if (ccc == create_count) create_count = 0;

                    //For deletion:
                    //Increase tower->delete_queue
                    //If > n then actually delete
                    for (auto& tower : delete_list) {
                        tower->delete_queue++;
                        if (tower->delete_queue > 10) {
                            tower->demoDestroy();
                        }
                    }
                    break;
                }
            }
        }
    }

    Matching GameController::stableMatching(tower::TYPE type, std::vector<Point<float>>& detections) {
        std::vector<deque<int>> point_prefs;
        std::vector<std::vector<int>> tower_prefs;
        auto& _towers = manager->getTowers();

        std::vector<tower_ptr> towers;
        if (_towers.size() > 0) {
            if (_towers[0]->getSubType() != tower::TYPE::BASE) {
                std::cout << "ERROR! 1st tower is not base" << std::endl;
            }
            /*auto begin = _towers.begin();
            // Just because
            auto last = _towers.end();//_towers.begin() + _towers.size();
            towers = std::vector<tower_ptr>(begin+1, last); //skip first tower without needing to change Jamie's algorithm*/
            for (size_t i = 1; i < _towers.size(); i++) {
                if (_towers[i]->getSubType() == type) {
                    towers.push_back(_towers[i]);
                }
            }
        } else {
            // To skip initial phase... rip
            return Matching();
        }
        // Not necessary
        /*else {
            towers = std::vector<tower_ptr>();
        }*/

        size_t tower_count = towers.size(); //base exists and is NOT first in tower, it was explicitly removed
        int** dists = new int*[tower_count];
        for (size_t i = 0; i < tower_count; i++) {
            dists[i] = new int[detections.size()];
            for (size_t j = 0; j < detections.size(); j++) {
                dists[i][j] = (int)towers[i]->distanceTo(detections[j]);
            }
        }

        for (size_t i = 0; i < tower_count; i++) {
            tower_prefs.push_back({});
            auto cmp = [&dists, i](int& pref, const int& point) {
                return dists[i][pref] < dists[i][point];
            };
            for (size_t j = 0; j < detections.size(); j++) {
                auto pos = std::lower_bound(tower_prefs[i].begin(), tower_prefs[i].end(), j, cmp);
                tower_prefs[i].insert(pos, j);
            }
        }

        for (size_t j = 0; j < detections.size(); j++) {
            point_prefs.push_back({});
            auto cmp = [&dists, j](int& pref, const int& tower) {
                return dists[pref][j] < dists[tower][j];
            };
            for (size_t i = 0; i < tower_count; i++) {
                auto pos = std::lower_bound(point_prefs[j].begin(), point_prefs[j].end(), i, cmp);
                point_prefs[j].insert(pos, i);
            }
        }

        for (size_t i = 0; i < tower_count; i++) delete[] dists[i];
        delete[] dists;

        stack<int> free;
        std::vector<Point<float>> news;
        std::vector<int> matches;

        for (size_t j = 0; j < detections.size(); j++) free.push(j);
        for (size_t i = 0; i < tower_count; i++) matches.push_back(NO_MATCH);

        while (!free.empty()) {
            int p = free.top();
            if (point_prefs[p].empty()) {
                // Clearly, this is a new tower
                news.push_back(detections[p]);
                free.pop();
                continue;
            }
            int t = point_prefs[p].front();
            point_prefs[p].pop_front();
            int p_ = matches[t];
            if (p_ == NO_MATCH) {
                matches[t] = p;
                free.pop();
            } else for (int x : tower_prefs[t]) {
                if (x == p_) break;
                else if (x == p) {
                    free.pop();
                    free.push(p_);
                    matches[t] = p;
                    break;
                }
            }
        }

        Matching match;
        match.new_towers = news;
        for (size_t i = 0; i < matches.size(); i++) {
            if (matches[i] == NO_MATCH) {
                match.deleted_towers.push_back(towers[i]);
            } else {
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

    void GameController::addEfficiency(unsigned int amt) {
        const float efficiency_factor = 1.f;
        tower_efficiency += amt * efficiency_factor;
    }

    void GameController::diminishEfficiency() {
        //Tower efficiency will drop towards 0.5
        const float diminishing_factor = 0.10;
        size_t tower_count = manager->getTowers().size();
        tower_efficiency -= 1.0f * diminishing_factor * tower_count;
        if (tower_efficiency < 0.f) tower_efficiency = 0.f;
    }

    float GameController::towerEfficiency() {
        const float eff_thresh = 0.f;
        const float eff_factor = 50.f;
        return (0.5f + std::fmin(tower_efficiency / eff_factor, 0.5f)) * efficiencyModifier;
    }
    
    float GameController::towerEfficiency(glm::vec2 position){
        const float eff_thresh = 0.f;
        const float eff_factor = 50.f;
        for (auto obj : manager->getObjects())
        {
            if (obj && obj->getSuperType() == gameobject::TYPE::OBJECT && obj->getSubType() == gameobject::OBJECT_TYPE::EFFECT_EMP)
            {
                if (obj->distanceTo(position) < EMPEffect::MAX_RANGE)
                {
                    return 0.0f;
                }
            }
        }
        return (0.5f + std::fmin(tower_efficiency / eff_factor, 0.5f)) * efficiencyModifier;
    }

    void GameController::setEfficiencyModifier(float mod)
    {
        efficiencyModifier = mod;
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

        int tile_width = manager->getRenderManager()->getWindowWidth() / TILE_W;
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
                std::cout << "      " << node->getX() << " " << node->getY() << std::endl;
                ret_path.emplace_back(node->getX()*tile_width, node->getY()*tile_height);
            }
            return true;
        }

        return false;
    }

    float TileNode::distanceTo(AStarNode* node) const {
        //int distance = (node->getX() - this->getX())*(node->getX() - this->getX()) + (node->getY() - this->getY())*(node->getY() - this->getY());
        float distance = std::hypotf((float)(node->getX() - this->getX()), (float)(node->getY() - this->getY()));
        return distance;
    }

    TileNode::TileNode() : AStarNode() {}

    void TileNode::addNode(TileNode* node) {
        addChild(node, 1.0f);
    }

    void TileNode::clearNodes() {
        this->clearChildren();
    }


    ///Display Methods

    sf::Time GameController::timeUntilNextWave() {
        //const float time_per_scenario = 20.f;
        //const int waves_per_scenario = 3;
        //int t = (int)getElapsedTime() % (int)(time_per_scenario / waves_per_scenario);
        return sf::Time(sf::seconds(0));
    }

    std::vector<std::string> GameController::getWarnings() {
        return std::vector<std::string>();
    }

    float GameController::getBaseHealth() {
        return getBase()->getHealth();
    }

    float GameController::getBaseMaxHealth() {
        return smartpointers::static_pointer_cast<tower::Base>(getBase())->max_health;
    }

    void GameController::unitTargetMine(id_t unit_id, id_t mine_id) {
        auto unit = smartpointers::static_pointer_cast<unit::Unit>(manager->getObjectById(unit_id));
        auto mine = smartpointers::static_pointer_cast<ResourceMine>(manager->getObjectById(mine_id));
        if (unit && mine) {
            unit->targetMine(mine);
        }
    }

    bool GameController::getCVReady() {
        return this->cvConnectionEstablished;
    }

    GameMode GameController::getCurrentGameMode() {
        return this->current_gamemode;
    }
}
