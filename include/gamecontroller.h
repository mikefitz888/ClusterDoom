#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "gamecore.h"
#include "smartpointers.h"
#include "PathFinder.h"
#include "aStar.h"
#include "../include/network/Buffer.h"

#define NO_MATCH -1

#define TILE_W 20
#define TILE_H 20

namespace gamecontroller {
    const float time_per_scenario = 20.f;
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
        unordered_map<tower_ptr, Point<int>> matches;
        std::vector<tower_ptr> deleted_towers;
        std::vector<Point<int>> new_towers;
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

        std::map<int, std::vector<Point<int>>> cvList;

        // CV Network
        int                port;
        sf::TcpListener*  listener;
        Buffer*           recv_buffer;
        sf::TcpSocket*    client;
        bool              cvConnectionEstablished;

        void startCVServer();
        void cvNetworkStep();

        int create_count = 0;
        int frame_clock = 0;

        int wave = 0;
        int scenario = -1;
        size_t wealth = 0;

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
        void restart() const; //Not yet implemented, clears towers + units
        Matching stableMatching(std::vector<Point<int>>& detections);
        int getWeight(int x, int y);

        //GameState Modifiers
        GameState getGameState();
        GameState stopGame();
        GameState startGame();
        GameState winGame();
        GameState loseGame();

        tower_ptr  spawnTowerAt(int x, int y, tower::TYPE type) const;
        tower_ptr  spawnTowerAt(Point<int> position, tower::TYPE type) const;
        unit_ptr   spawnUnitAt(int x, int y, unit::TYPE type) const;
        unit_ptr   spawnUnitAt(Point<int> position, unit::TYPE type) const;
        gameobject_ptr spawnObjectAt(gameobject::OBJECT_TYPE type, int x, int y) const;
        gameobject_ptr spawnObjectAt(gameobject::OBJECT_TYPE type, Point<int> position) const;
        void moveTower(tower_ptr tower, Point<int> point) const;

        void runScenario(int scenario);

        void clearTowers() const;
        void spawnTowers(std::vector<std::pair<Point<int>, int>> tower_list) const;
        std::vector<tower_ptr> findNearestTowers(Point<int> point);
        std::vector<unit_ptr> getUnitsInRange(glm::vec2 position, int radius);
        std::vector<std::pair<float, unit_ptr>> getNNearestUnits(glm::vec2 position, int N, int maxrange);
        tower_ptr& getBase();
        void parseCVList(std::vector<std::pair<Point<int>, int>> list);
        

        bool getPath(ivec2 start, ivec2 end, std::vector<vec2>& ret_path);

        int getScreenWidth();
        int getScreenHeight();

        void increaseWealth(size_t amt);
        int requestWealth(size_t amt); //returns the min(amt, wealth)

        size_t availableWealth();
        sf::Time timeUntilNextWave();
        std::vector<std::string> getWarnings();
    };
}

#endif //GAMECONTROLLER_H
