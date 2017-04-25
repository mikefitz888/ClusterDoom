#include "../../include/GameObjects/SpecialToken.h"
#include "../../include/manager.h"
#include "../../include/ResourceManager.h"
#include "../../include/gamecontroller.h"
#include "../../include/tower.h"
#include "../../include/towers/SpecialTower.h"
#include "../../include/smartpointers.h"

const int SpecialToken::MAX_PICKUP_RANGE = 50;

SpecialToken::SpecialToken(id_t id, manager::Manager* m, gameobject::OBJECT_TYPE type)
    : GameObject(id, gameobject::TYPE::OBJECT, type, m) {
    this->type = type;
    this->setNetworkSync(true);
    this->setDepth(-10);
}

void SpecialToken::init() {
    sf::String texture;
    switch (type)
    {
    case gameobject::OBJECT_TYPE::TOKEN_GLACIAL:
        texture = "token_glacial";
        break;
    case gameobject::OBJECT_TYPE::TOKEN_MAGNETIC:
        texture = "token_magnetic";
        break;
    case gameobject::OBJECT_TYPE::TOKEN_WINDY:
        texture = "token_windy";
        break;
    }
    this->tokenTexture = this->manager->getResourceManager()->getTexture(texture);
    this->setDepth(5);
}

void SpecialToken::step() {
    // Find nearby special towers
    auto towers = this->manager->getGameController()->getTowersInRange(this->position, MAX_PICKUP_RANGE);
    for (auto tower : towers)
    {
        if (tower->getSubType() == tower::TYPE::SPECIAL)
        {
            auto specialTower = smartpointers::static_pointer_cast<tower::SpecialTower>(tower);
            switch (type)
            {
            case gameobject::OBJECT_TYPE::TOKEN_GLACIAL:
                specialTower->changeEffect(tower::SPECIAL_TYPE::GLACIAL);
                break;
            case gameobject::OBJECT_TYPE::TOKEN_MAGNETIC:
                specialTower->changeEffect(tower::SPECIAL_TYPE::MAGNETIC);
                break;
            case gameobject::OBJECT_TYPE::TOKEN_WINDY:
                specialTower->changeEffect(tower::SPECIAL_TYPE::WINDY);
                break;
            }
            destroySelf();
            break;
        }
    }
}

void SpecialToken::renderGUI() {
    this->tokenTexture->render(this->getX(), this->getY()/*, scale, scale, 0.0f*/);
}

void SpecialToken::render() {}
void SpecialToken::release() {}