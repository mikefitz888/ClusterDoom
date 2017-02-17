#ifndef _SPAWN_H
#define _SPAWN_H

#include "../gamecore.h"
#include "../../include/unit.h"
#include "../../include/gameobject.h"

class Spawn : public GameObject {

protected:
    int scenario = 0;
    int wave = 0;
    int time = 0;
    int units = 0;
    int spawn_rate = 0;
    bool running = false;
    struct unit_spawn {
        const int time;
        const unit::TYPE unit_type;
        unit_spawn(unit::TYPE type, int t = 0);
    };
    std::vector<unit_spawn> spawn_queue;
public:
	Spawn(id_t id, manager::Manager* m);

	virtual void init() override;
	virtual void render() override;
	virtual void renderGUI() override;
	virtual void release() override;
    virtual void step() override;

    virtual void beginWave();

    void startScenario(int scenario);
    void startWave(int wave);
};

#endif
