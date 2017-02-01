#ifndef _SPAWN_H
#define _SPAWN_H

#include "../gamecore.h"

#include "../../include/gameobject.h"

using gameobject::GameObject;
using gameobject::Point;
using gameobject::TYPE;

class Spawn : public GameObject {

protected:
	
public:
	Spawn(gameobject::id_t id, Manager* m);

	virtual void init() override;
	virtual void render() override;
	virtual void renderGUI() override;
	virtual void release() override;
    virtual void step() override;

	void startSpawn(void* arg, void* ret);
	void continueSpawn(void* arg, void* ret);
};

#endif