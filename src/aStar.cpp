#include <queue>
#include <string>
#include <math.h>

using namespace std;

const int w = 50; //width of grid
const int h = 50; //height of grid

int object[w][h]; //1 if area contains collider, 0 otherwise

int closedNodes[w][h];
int openNodes[w][h];

const int dir = 8;
int dirMap[w][h];

int dx[dir] = { 1, 1, 0, -1, -1, -1, 0, 1 };
int dy[dir] = { 0, 1, 1, 1, 0, -1, -1, -1 };

class node {

	//x,y position of node in grid
	int x;
	int y;

	//distance from start to node
	int gCost;

	//start to end distance (distance to node + estimate to end)
	int fCost;

	public:
		node(int xx, int yy, int  gg, int ff) {
			x = xx;
			y = yy;
			gCost = gg;
			fCost = ff;
		}
		int getX() const{
			return x;
		}
		int getY() const{
			return y;
		}
		int getG() const{
			return gCost;
		}
		int getF() const{
			return fCost;
		}

		void updateF(int & xEnd, int & yEnd) {
			fCost = gCost + getH(xEnd, yEnd) * 10;
		}

		void updateG(int & d) {
			if (d % 2 == 0) {
				gCost += 10;
			}
			else {
				gCost += 14;
			}
		}

		int & getH(int & xEnd, int & yEnd) {
			int xe = xEnd - x;
			int ye = yEnd - y;

			int dist = static_cast<int>(sqrt((xe*xe) + (ye*ye)));
			return dist;
		}
};

//for comparing nodes
bool operator<(const node & a, const node & b)
{
	return a.getF() > b.getF();
}

string aStar(int & xStart, int & yStart, int & xEnd, int & yEnd) {

	//initialise working queue and index toggle
	priority_queue<node> workingNodes[2];
	int wTog = 0;

	node* n;
	node* m;

	char c;

	int x;
	int y;
	
	//initialise open and closed sets to 0
	for (x = 0; x < w; x++) {
		for (y = 0; y < w; y++) {
			openNodes[x][y] = 0;
			closedNodes[x][y] = 0;
		}
	}

	//create start node and place in working and open sets
	n = new node(xStart, yStart, 0, 0);
	n->updateF(xEnd, yEnd);
	workingNodes[wTog].push(*n);
	openNodes[x][y] = n->getF();

	while (!workingNodes[wTog].empty()) {

		//get node with larget f value, take it off the working and open sets and add to the closed set
		n = new node(workingNodes[wTog].top().getX(), workingNodes[wTog].top().getY(), workingNodes[wTog].top().getG(), workingNodes[wTog].top().getF());

		x = n->getX();
		y = n->getY();

		workingNodes[wTog].pop();
		openNodes[x][y] = 0;
		closedNodes[x][y] = 1;

		//if at the end retrace path to start and output
		if (x == xEnd && y == yEnd) {

			string path = "";
			while (!(x == xEnd && y == yEnd)){

				int thisDir = dirMap[x][y];
				c = '0' + thisDir;
				path = c + path;

				x += dx[thisDir];
				y += dy[thisDir];
			}
			return path;
		}

		//check all surrounding nodes (children)
		for (int i = 0; i < dir; i++) {
			int xdx = x + dx[i];
			int ydy = y + dy[i];

			//if outside bounds, in closed set or an object ignore
			if (!(xdx<0 || ydy<0 || xdx>w - 1 || ydy>h - 1 || object[xdx][ydy] == 1 || closedNodes[xdx][ydy] == 1)) {
				
				m = new node(xdx, ydy, n->getG(), n->getF());
				m->updateG(i);
				m->updateF(xEnd, yEnd);

				//add to open, working and direction sets if not already there
				if (openNodes[xdx][ydy] == 0) {
					openNodes[xdx][ydy] = m->getF();
					workingNodes[wTog].push(*m);
					dirMap[xdx][ydy] = (i + dir / 2) % dir;
				}
				else if (openNodes[xdx][ydy] > m->getF()) {
					openNodes[xdx][ydy] = m->getF();
					dirMap[xdx][ydy] = (i + dir / 2) % dir;

					//find child node in working set
					while (!(workingNodes[wTog].top().getX() == xdx && workingNodes[wTog].top().getY() == ydy)) {
						workingNodes[1 - wTog].push(workingNodes[wTog].top());
						workingNodes[wTog].pop();
					}
					workingNodes[wTog].pop();

					//reset working set
					if (workingNodes[wTog].size() > workingNodes[1 - wTog].size()) {
						wTog = 1 - wTog;
					}
					while (!workingNodes[wTog].empty()) {
						workingNodes[1 - wTog].push(workingNodes[wTog].top());
						workingNodes[wTog].pop();
					}
					wTog = 1 - wTog;

					//add closer node
					workingNodes[wTog].push(*m);
				}
			}
		}
	}
	return "";
}