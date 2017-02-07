#ifndef ASTAR_H
#define ASTAR_H

#include "gamecore.h"
void aStar(Point<int> start, Point<int> end, const int w, const int h, gamecontroller::GameController *gc, std::vector<Point<int>>& ret);

#endif
