#include "../include/AttackerEffects.h"
#include "../include/manager.h"
#include "../include/unit.h"
#include "../include/tower.h"
#include "../include/gamecontroller.h"
#include "../include/ResourceManager.h"

const unsigned int EMPEffect::DURATION = 1000;
const unsigned int DisruptionEffect::DURATION = 1000;
const double EMPEffect::MAX_RANGE = 150;
const double DisruptionEffect::EFFICIENCY_MODIFIER = 0.75;
const double UnitHealEffect::MAX_RANGE = 120;
const int UnitHealEffect::HEAL_STRENGTH = 8000;

EMPEffect::EMPEffect(id_t id, manager::Manager* manager) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::EFFECT_EMP, manager)
{
    ticks = DURATION;
    this->setDepth(20);
}

void EMPEffect::render()
{
    float scale = MAX_RANGE / 150.0f;
    this->manager->getRenderManager()->setActiveColour(255, 255, 255, (int)glm::clamp(255.0f * fadeout*0.50f, 0.0f, 255.0f));
    this->manager->getResourceManager()->getTexture("emp_outer")->render((int) getX(), (int) getY(), scale, scale, 0.0f);

    float rot = (float)ticks*0.01f;
    this->manager->getResourceManager()->getTexture("emp_inner")->render((int) getX(), (int) getY(), scale, scale, rot);
    this->manager->getResourceManager()->getTexture("emp_inner")->render((int) getX(), (int) getY(), scale, scale, -rot);
   // this->manager->getResourceManager()->getTexture("emp_inner")->render(getX(), getY(), scale, scale, rot + glm::pi<float>()/2);
   // this->manager->getResourceManager()->getTexture("emp_inner")->render(getX(), getY(), scale, scale, -rot + glm::pi<float>() / 2);
    this->manager->getRenderManager()->setActiveColour(255, 255, 255, 255);
}

void EMPEffect::step()
{
    ticks--;
    if (ticks > 0)
    {
        if (fadeout < 1.0f) {
            fadeout += 0.01f;
        }
    } else {
        if (fadeout >= 0.0f) {
            fadeout -= 0.01f;
        } else {
            destroySelf();
        }
    }
}

DisruptionEffect::DisruptionEffect(id_t id, manager::Manager* manager) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::EFFECT_DISRUPTION, manager)
{
    ticks = DURATION;
    this->setDepth(-10);
}

void DisruptionEffect::render()
{
    this->manager->getRenderManager()->setActiveColour(255, 255, 255, (int)glm::clamp(255.0f * fadeout*0.20f, 0.0f, 255.0f));
    this->manager->getResourceManager()->getTexture("red")->render(0, 0, this->manager->getRenderManager()->getWindowWidth(), this->manager->getRenderManager()->getWindowHeight(), 0.0f);
    this->manager->getRenderManager()->setActiveColour(255, 255, 255, 255);
}

void DisruptionEffect::step()
{
    manager->getGameController()->setEfficiencyModifier(EFFICIENCY_MODIFIER);
    ticks--;
    if (ticks <= 0)
    {
        manager->getGameController()->setEfficiencyModifier(1.0);

        if (fadeout >= 0.0f) {
            fadeout -= 0.01f;
        } else {
            destroySelf();
        }
    } else {
        if (fadeout < 1.0f) {
            fadeout += 0.01f;
        }
    }
}

UnitHealEffect::UnitHealEffect(id_t id, manager::Manager* manager) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::EFFECT_HEAL, manager)
{
    this->setDepth(20);
}

void UnitHealEffect::render()
{
    float scale = MAX_RANGE / 120.0f;
    this->manager->getRenderManager()->setActiveColour(255, 255, 255, (int)glm::clamp(255.0f * fadeout*0.5f, 0.0f, 255.0f));
    this->manager->getResourceManager()->getTexture("healing_aura")->render((int) getX(), (int) getY(), scale, scale, 0.0f);
    this->manager->getRenderManager()->setActiveColour(255, 255, 255, 255);
}

void UnitHealEffect::step()
{
    // We'll keep a step function, just in case 
    healing_timer--;
    if (healing_timer > 0) {

        if (fadeout < 1.0f) {
            fadeout += 0.01f;
        }
        for (auto unit : manager->getGameController()->getUnitsInRange(getPosition(), MAX_RANGE)) {
            if (unit) unit->heal(ceil((float)HEAL_STRENGTH/(float)healing_timer_max));
        }
    } else {
        if (fadeout >= 0) {
            fadeout -= 0.01f;
        } else {
            destroySelf();
        }
    }
}