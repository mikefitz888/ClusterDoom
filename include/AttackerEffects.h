#ifndef ATTACKER_EFFECTS_H
#define ATTACKER_EFFECTS_H

#include "gamecore.h"
#include "gameobject.h"

// This class represents the EMP Attacker Effect, the effect is
// performed by the game controller, this class' step just ensures
// the effect dies after the allotted time
class EMPEffect : public GameObject
{
    const static unsigned int DURATION;

    int ticks;
    float fadeout = 0.0f;

public:
    const static double MAX_RANGE;
    EMPEffect(id_t id, manager::Manager* manager);

    virtual void render() override;
    virtual void step() override;
};

// This class represents the Disruption Effect.
// Its step sets a flag in game controller if the effect
// is active, and disables it upon effect timeout.
class DisruptionEffect : public GameObject
{
    const static double EFFICIENCY_MODIFIER;
    const static unsigned int DURATION;
    float fadeout = 0.0f;

    int ticks;

public:
    DisruptionEffect(id_t id, manager::Manager* manager);

    virtual void render() override;
    virtual void step() override;
};

// This class represents the Unit Heal Attacker Effect
// Its step method will find all units in range and heal
// them by the amount in the HEAL_STRENGTH variable
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