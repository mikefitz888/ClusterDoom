#ifndef _SPAWN_H
#define _SPAWN_H

#include "../../include/gameobject.h"

using gameobject::GameObject;
using gameobject::Point;
using gameobject::TYPE;
using gameobject::id_t;

class Spawn : public GameObject {
	Point<int> position;

public:
	Spawn(int x, int y, id_t id, Manager* m);
	Spawn(Point<int> position, id_t id, Manager* m);
	int getX();
	int getY();
	Point<int> getPosition();

	virtual void init() override;
	virtual void render() override;
	virtual void renderGUI() override;
	virtual void release() override;
    virtual void step() override;
};

#endif