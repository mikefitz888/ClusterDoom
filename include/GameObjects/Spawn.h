#ifndef _SPAWN_H
#define _SPAWN_H

#include <random>
#include "../gamecore.h"
#include "../../include/unit.h"
#include "../../include/gameobject.h"

class Spawn : public GameObject {

protected:
    int scenario = 0;
    int wave = 0;
    int time = 0;
    size_t spawn_id = 0;
    int units = 0;
    bool running = false;
    struct unit_spawn {
        int delay;
        unit::TYPE unit_type;
        unit_spawn(unit::TYPE type, int t = 0);
    };
    std::vector<unit_spawn> spawn_queue;

    std::mt19937 rng;
    std::uniform_int_distribution<uint32_t> random_unit = std::uniform_int_distribution<uint32_t>(1, 3);
    std::uniform_int_distribution<uint32_t> random_spawn = std::uniform_int_distribution<uint32_t>(0, 3);
    std::uniform_int_distribution<uint32_t> cluster_size[3] = { std::uniform_int_distribution<uint32_t>(1, 2), std::uniform_int_distribution<uint32_t>(1, 3), std::uniform_int_distribution<uint32_t>(1, 4)};
    std::uniform_int_distribution<uint32_t> noSpawn = std::uniform_int_distribution<uint32_t>(0, 1);
    std::uniform_int_distribution<uint32_t> spawn_jitter = std::uniform_int_distribution<uint32_t>(0, 50);
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
    void setSpawnID(size_t id);

    // Network events
    enum NetworkInteractionEvent { SPAWN_UNIT = 0 };
    void recvNetworkInteraction(int event_id, Buffer &buffer) override;
};

#endif
