#include "../include/unit.h"
#include "../include/aStar.h"

// TOM: namespace pollution... either `using` the things you need or std qualify
using gameobject::Point;

//bool isObstacle(x, y)

struct Node
{
	//x,y position of node in grid
	int x;
	int y;

	//distance from start to node
	int g;

	//start to end distance (distance to node + estimate to end)
	int f;

	Node(int x, int y, int  g, int f) {
		this->x = x;
		this->y = y;
		this->g = g;
		this->f = f;
	}

	// TOM: ints are the same size as references, as such, references are more expensive
	// only use references for bigger types
	inline void updateF(int xEnd, int yEnd) {
		// Why 10? what's it's name?
		f = g + heuristic(xEnd, yEnd) * 10;
	}

	// As above
	inline void updateG(int d) {
		/*if (d % 2 == 0) {
			g += 10;
		}
		else {
			g += 14;
		}*/
		// Magic numbers... should probably have definitions!
		g += d % 2 == 0 ? 10 : 14;
	}

	// As above
	// TOM: note, we have a macro for distance in gameobject.h
	// also, you returned an int& from this function, that's bad, dist goes out of scope immediately and it is destroyed!
	// unless you can be sure the reference will live (i.e. it is dynamically allocated or in some struct that will live)
	// don't return references! (same goes for pointers)
	inline int heuristic(int xEnd, int yEnd) {
		//int xe = xEnd - x;
		//int ye = yEnd - y;

		//int dist = static_cast<int>(sqrt((xe*xe) + (ye*ye)));
		return (int) DISTANCE(x, y, xEnd, yEnd);
	}

	//TOM: Moved this into the class, no need for it to be a function
	//p.s. a bit misleading no, < is really >. the priority queue could be given
	//std::greater<Node> instead (and all that's needed is to change the less to greater in my namespace std stuff)
	// and then priority_queue<Node, std::vector<Node>, std::greater<Node>>, but I'll admit it doesn't look as nice...
	//for comparing nodes
	inline bool operator<(const Node& other) const
	{
		return f > other.f;
	}
};

// defining std::less<Node> for the queues
namespace std
{
	// empty template, we know the type
	template <> struct less<Node>
	{
		bool operator()(const Node& lhs, const Node& rhs)
		{
			return lhs < rhs;
		}
	};
}

void aStar(Point<int> start, Point<int> end, const int w, const int h, gamecontroller::GameController *gc, std::vector<Point<int>>& ret) {
	//w and h refer to  width and height of grid. object is 2d array indicating colliders (1 for object 0 for not)
	int xStart = start.x;
	int yStart = start.y;

	int xEnd = end.x;
	int yEnd = end.y;
	//initialise working queue and index toggle
	// TOM: this priority queue will not prioritise anything, it doesn't know how
	// to sort on Nodes... it uses a thing called std::less<Node>... I've now
	// made one of those, so it will now work
	std::priority_queue<Node> workingNodes[2];
	int workingQueueToggle = 0;

	//gets change in x and y for different directions
	const int dir = 8;
	int dx[dir] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	int dy[dir] = { 0, 1, 1, 1, 0, -1, -1, -1 };

	//initialise open, closed sets and parent node map
	int** closedNodes = new int*[w];
	int** openNodes = new int*[w];
	int** dirMap = new int*[w];

	for (int x = 0; x < w; x++) {
		closedNodes[x] = new int[h];
		openNodes[x] = new int[h];
		dirMap[x] = new int[h];
		for (int y = 0; y < h; y++) {
			openNodes[x][y] = 0;
			closedNodes[x][y] = 0;
		}
	}

	//create start node and place in working and open sets
	// TOM: You have a queue of Nodes, why are you dynamically allocating Node*?
	// You can simply write the below code, it's faster, more memory efficient
	//Node* n = new Node(xStart, yStart, 0, 0);
	Node n(xStart, yStart, 0, 0);
	n.updateF(xEnd, yEnd);
	workingNodes[workingQueueToggle].push(n);
	openNodes[w-1][h-1] = n.f;

	while (!workingNodes[workingQueueToggle].empty()) {

		//get node with larget f value, take it off the working and open sets and add to the closed set
		//n = new Node(workingNodes[workingQueueToggle].top().x, workingNodes[workingQueueToggle].top().y, workingNodes[workingQueueToggle].top().g, workingNodes[workingQueueToggle].top().f);
		// TOM: erm, any reason you create a new node here?
		//n = Node(workingNodes[workingQueueToggle].top().x, workingNodes[workingQueueToggle].top().y, workingNodes[workingQueueToggle].top().g, workingNodes[workingQueueToggle].top().f);
		const Node& n = workingNodes[workingQueueToggle].top(); //This is the node that's in the queue, NOT a copy, confirm this with me later

		int x = n.x;
		int y = n.y;

		workingNodes[workingQueueToggle].pop();
		openNodes[n.x][y] = 0;
		closedNodes[n.x][y] = 1;

		//if at the end retrace path to start and output
		if (x == xEnd && y == yEnd) {

			std::vector<Point<int>> path;
			int thisDir = dirMap[x][y];
			path.insert(path.begin(), Point<int>(x, y));
			int lastDir = thisDir;

			while (x != xStart || y != yStart) {

				thisDir = dirMap[x][y];
				if (thisDir != lastDir) {
					path.insert(path.begin(), Point<int>(x, y));
				}

				x += dx[thisDir];
				y += dy[thisDir];

				lastDir = thisDir;
			}

			//TOM: cleaning up memory
			// Really, these never change in size, could probably keep them around
			// This would mean making a full class out of this, and then have aStar as a method
			for (int i = 0; i < w; i++)
			{
				delete [] closedNodes[i];
				delete [] openNodes[i];
				delete [] dirMap[i];
			}
			delete [] closedNodes;
			delete [] openNodes;
			delete [] dirMap;

			ret = path;
		}

		//check all surrounding nodes (children)
		for (int i = 0; i < dir; i++) {
			int xdx = x + dx[i];
			int ydy = y + dy[i];

			//if outside bounds, in closed set or an object ignore
			if (!(xdx < 0 || ydy < 0 || xdx > w-1 || ydy > h-1 || gc->getWeight(xdx,ydy) < 0 || closedNodes[xdx][ydy] == 1)) {

				// TOM: Ditto for earlier
				//Node* m = new Node(xdx, ydy, n.g, n.f);
				Node m(xdx, ydy, n.g, n.f);
				m.updateG(i);
				m.updateF(xEnd, yEnd);

				// Toggle is basically a boolean value, x = !x is preferable to x = 1 - x, more clear

				//add to open, working and direction sets if not already there
				if (openNodes[xdx][ydy] == 0) {
					openNodes[xdx][ydy] = m.f;
					workingNodes[workingQueueToggle].push(m);
					dirMap[xdx][ydy] = (i + dir / 2) % dir;
				}
				else if (openNodes[xdx][ydy] > m.f) {
					openNodes[xdx][ydy] = m.f;
					dirMap[xdx][ydy] = (i + dir / 2) % dir;

					//find child node in working set
					while (!(workingNodes[workingQueueToggle].top().x == xdx && workingNodes[workingQueueToggle].top().y == ydy)) {
						workingNodes[!workingQueueToggle].push(workingNodes[workingQueueToggle].top());
						workingNodes[workingQueueToggle].pop();
					}
					workingNodes[workingQueueToggle].pop();

					//reset working set
					if (workingNodes[workingQueueToggle].size() > workingNodes[!workingQueueToggle].size()) {
						workingQueueToggle = !workingQueueToggle;
					}
					while (!workingNodes[workingQueueToggle].empty()) {
						workingNodes[!workingQueueToggle].push(workingNodes[workingQueueToggle].top());
						workingNodes[workingQueueToggle].pop();
					}
					workingQueueToggle = !workingQueueToggle;

					//add closer node
					workingNodes[workingQueueToggle].push(m);
				}
			}
		}
	}

	//TOM: cleaning up memory
	// Really, these never change in size, could probably keep them around
	// This would mean making a full class out of this, and then have aStar as a method
	for (int i = 0; i < w; i++)
	{
		delete [] closedNodes[i];
		delete [] openNodes[i];
		delete [] dirMap[i];
	}
	delete [] closedNodes;
	delete [] openNodes;
	delete [] dirMap;

	//TOM: made the equivalent for now, just to show it can be done
	//std::vector<Point<int>> empty;
	//return empty;
}
