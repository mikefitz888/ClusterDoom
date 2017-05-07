#include "../include/AttackerEffects.h"
#include "../include/manager.h"
#include "../include/unit.h"
#include "../include/tower.h"
#include "../include/gamecontroller.h"

const unsigned int EMPEffect::DURATION = 1000;
const unsigned int DisruptionEffect::DURATION = 1000;
const double EMPEffect::MAX_RANGE = 120;
const double DisruptionEffect::EFFICIENCY_MODIFIER = 0.75;
const double UnitHealEffect::MAX_RANGE = 120;
const int UnitHealEffect::HEAL_STRENGTH = 250;

EMPEffect::EMPEffect(id_t id, manager::Manager* manager) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::EFFECT_EMP, manager)
{
    ticks = DURATION;
}

void EMPEffect::render()
{

}

void EMPEffect::step()
{
    ticks--;
    if (ticks <= 0)
    {
        destroySelf();
    }
}

DisruptionEffect::DisruptionEffect(id_t id, manager::Manager* manager) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::EFFECT_DISRUPTION, manager)
{
    ticks = DURATION;
}

void DisruptionEffect::render()
{

}

void DisruptionEffect::step()
{
    manager->getGameController()->setEfficiencyModifier(EFFICIENCY_MODIFIER);
    ticks--;
    if (ticks <= 0)
    {
        manager->getGameController()->setEfficiencyModifier(1.0);
        destroySelf();
    }
}

UnitHealEffect::UnitHealEffect(id_t id, manager::Manager* manager) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::EFFECT_HEAL, manager)
{
}

void UnitHealEffect::render()
{

}

void UnitHealEffect::step()
{
    // We'll keep a step function, just in case 
    for (auto unit : manager->getGameController()->getUnitsInRange(getPosition(), MAX_RANGE))
    {
        unit->heal(HEAL_STRENGTH);
    }
    destroySelf();
}