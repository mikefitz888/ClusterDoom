#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "gamecore.h"
#include "PathFinder.h"
#include "AStar.h"

#include "smartpointers.h"
//#include "gameobject.h"
//#include "GameObjects/Spawn.h"
//#include "manager.h"
//#include "../include/network/Buffer.h"
//#include "../include/tower.h"
//#include "../include/unit.h"

#define NO_MATCH -1

#define TILE_H 20
#define TILE_W 20

namespace gamecontroller {
    using gameobject::GameObject;
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

	class TileNode : public AStarNode {
		inline float distanceTo(AStarNode* node) const override {
			return abs( (float)(node->getX() - m_x) ) + abs( (float)(node->getY() - m_y) );
		};
	public:
		inline TileNode() : AStarNode() {}
		inline void addNode(TileNode* node) {
			addChild(node, 1.0f);
		}
	};

    class GameController {
        Manager* manager;
		PathFinder<TileNode> path_finder; //Path results are std::vector<TileNode*>
		//Fixed tile dimensions

		TileNode nodes[TILE_W * TILE_H];

        vector<Point<int>> cvList;
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

        bool game_started = false;
        int wave = 0;
        int scenario = 0;

        std::vector<smartpointers::slave_ptr<Spawn>> spawn_points = std::vector<smartpointers::slave_ptr<Spawn>>();
        // TEMP
        bool spawned = false;

    public:
        GameController(Manager* m);
       // GameObject* createObject(id_t key);
        float getElapsedTime() const;
        void resetClock();
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

		inline bool getPath(Point<int> start, Point<int> end, std::vector<Point<int>>& ret_path) {
			ret_path.clear();
			std::vector<TileNode*> path;
			TileNode& node1 = nodes[start.y*TILE_W + start.x];
			TileNode& node2 = nodes[end.y*TILE_W + end.x];
			path_finder.setStart(node1);
			path_finder.setGoal(node2);
			if (path_finder.findPath<AStar>(path)) {
				for (auto node : path) {
					ret_path.emplace_back(node->getX(), node->getY());
				}
			}
		}

        int getScreenWidth();
        int getScreenHeight();
    };
}

#endif //GAMECONTROLLER_H
