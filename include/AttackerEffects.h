#ifndef ATTACKER_EFFECTS_H
#define ATTACKER_EFFECTS_H

#include "gamecore.h"
#include "gameobject.h"

class EMPEffect : public GameObject
{
    const static unsigned int DURATION;

    unsigned int ticks;

public:
    const static double MAX_RANGE;
    EMPEffect(id_t id, manager::Manager* manager);

    virtual void render() override;
    virtual void step() override;
};

class DisruptionEffect : public GameObject
{
    const static double EFFICIENCY_MODIFIER;
    const static unsigned int DURATION;

    unsigned int ticks;

public:
    DisruptionEffect(id_t id, manager::Manager* manager);

    virtual void render() override;
    virtual void step() override;
};

class UnitHealEffect : public GameObject
{
    const static double MAX_RANGE;
    const static int HEAL_STRENGTH;
    const int healing_timer_max = 480;
    int healing_timer = healing_timer_max;
    float fadeout = 0.0f;
public:
    UnitHealEffect(id_t id, manager::Manager* manager);

    virtual void render() override;
    virtual void step() override;
};

#endif