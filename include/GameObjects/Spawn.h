#ifndef _SPAWN_H
#define _SPAWN_H

#include "../gamecore.h"

#include "../../include/gameobject.h"

class Spawn : public GameObject {

protected:

public:
	Spawn(id_t id, manager::Manager* m);

	virtual void init() override;
	virtual void render() override;
	virtual void renderGUI() override;
	virtual void release() override;
    virtual void step() override;

	void startSpawn(void* arg, void* ret);
	void continueSpawn(void* arg, void* ret);
};

#endif
