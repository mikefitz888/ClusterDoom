#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "gamecore.h"
#include "smartpointers.h"
#include "PathFinder.h"
#include "aStar.h"
//#include "../include/tower.h"
#include "../include/network/Buffer.h"

#define NO_MATCH -1

#define TILE_W 20
#define TILE_H 20

namespace gamecontroller {
    const float time_per_scenario = 60.f;
    const int waves_per_scenario = 3;

    using gameobject::GameObject;
    using gameobject::tower_ptr;
    using gameobject::unit_ptr;
    using gameobject::gameobject_ptr;
    using gameobject::Point;
    using manager::Manager;

    using std::cout;
    using std::endl;
    using std::deque;
    using std::stack;
    using std::unordered_map;
    using std::tuple;

    enum GameState : unsigned int { START = 0, RUNNING, WIN, LOSE };

    struct Matching
    {
        unordered_map<tower_ptr, Point<float>> matches;
        std::vector<tower_ptr> deleted_towers;
        std::vector<Point<float>> new_towers;
    };

    class TileNode : public paths::AStarNode {
        float distanceTo(AStarNode* node) const override;
    public:
        TileNode();
        void addNode(TileNode* node);
        void clearNodes(); // Remove links to other nodes
    };

    class GameController {
        Manager* manager;
        paths::PathFinder<TileNode> path_finder; //Path results are std::vector<TileNode*>
        //Fixed tile dimensions

        TileNode nodes[TILE_W * TILE_H];

        
        GameState current_state = GameState::START;

        //std::map<int, std::vector<Point<int>>> cvList;
        std::vector<Point<float>> cvList[tower::TYPE::num_types];

        // CV Network
        int               port;
        sf::TcpListener*  listener;
        Buffer*           recv_buffer;
        sf::TcpSocket*    client;
        bool              cvConnectionEstablished;

        void startCVServer();
        void cvNetworkStep();

        int create_count = 0;
        int frame_clock = 0;
        bool initial_spawns_occurred = false;

        int wave = 0;
        int scenario = -1;
        float tower_efficiency = 0;

        // Resource spawning
        const int resource_spawn_timer_max = 1500;
        const int resource_spawn_timer_min = 600;
        const int token_timer_max = 35*60;
        const int token_timer_min = 20*60;
        const int max_resource_on_map = 3;
        int resource_spawn_timer = resource_spawn_timer_min;
        int token_timer = token_timer_min;

        std::vector<smartpointers::slave_ptr<Spawn>> spawn_points = std::vector<smartpointers::slave_ptr<Spawn>>();
        // TEMP
        bool spawned = false;

        // Prepare pathfinding grid
        void preparePathfindingGrid();


    public:
        GameController(Manager* m);
       // GameObject* createObject(id_t key);
        float getElapsedTime() const;
        void resetClock();
        void init();
        bool step();
        void restart(); //Not yet implemented, clears towers + units
        Matching stableMatching(tower::TYPE type, std::vector<Point<float>>& detections);
        int getWeight(int x, int y);

        //GameState Modifiers
        GameState getGameState();
        GameState stopGame();
        GameState startGame();
        GameState winGame();
        GameState loseGame();

        tower_ptr  spawnTowerAt(float x, float y, tower::TYPE type) const;
        tower_ptr  spawnTowerAt(Point<float> position, tower::TYPE type) const;
        unit_ptr   spawnUnitAt(float x, float y, unit::TYPE type) const;
        unit_ptr   spawnUnitAt(Point<float> position, unit::TYPE type) const;
        gameobject_ptr spawnObjectAt(gameobject::OBJECT_TYPE type, float x, float y) const;
        gameobject_ptr spawnObjectAt(gameobject::OBJECT_TYPE type, Point<float> position) const;
        void moveTower(tower_ptr tower, Point<float> point) const;

        void runScenario(int scenario);

        void clearTowers() const;
        void spawnTowers(std::vector<std::pair<Point<float>, int>> tower_list) const;
        std::vector<tower_ptr> findNearestTowers(Point<float> point);
        std::vector<unit_ptr> getUnitsInRange(glm::vec2 position, int radius);
        std::vector<tower_ptr> getTowersInRange(glm::vec2 position, int radius);
        std::vector<std::pair<float, unit_ptr>> getNNearestUnits(glm::vec2 position, int N, int maxrange);
        tower_ptr& getBase();
        void parseCVList(std::vector<std::pair<Point<float>, int>> list);
        

        bool getPath(ivec2 start, ivec2 end, std::vector<vec2>& ret_path);

        int getScreenWidth();
        int getScreenHeight();

        void addEfficiency(unsigned int amt = 1);
        float towerEfficiency();
        void diminishEfficiency();

        sf::Time timeUntilNextWave();
        std::vector<std::string> getWarnings();
        float getBaseHealth();
        float getBaseMaxHealth();

        void unitTargetMine(id_t unit_id, id_t mine_id);
    };
}

#endif //GAMECONTROLLER_H
