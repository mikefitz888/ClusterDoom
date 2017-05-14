#include "../../include/GameObjects/SpecialToken.h"
#include "../../include/manager.h"
#include "../../include/ResourceManager.h"
#include "../../include/gamecontroller.h"
#include "../../include/tower.h"
#include "../../include/towers/SpecialTower.h"
#include "../../include/smartpointers.h"
#include "../../include/RenderUtils.h"

const int SpecialToken::MAX_PICKUP_RANGE = 50;

using namespace graphics;

SpecialToken::SpecialToken(id_t id, manager::Manager* m, gameobject::OBJECT_TYPE type)
    : GameObject(id, gameobject::TYPE::OBJECT, type, m) {
    this->type = type;
    this->setNetworkSync(true);
    this->setDepth(-10);
}

void SpecialToken::init() {

	cooldown = maxCooldown;
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

	cooldown--;
	if (cooldown <= 0) {
		destroySelf();
	}
    // Find nearby special towers
    auto towers = this->manager->getGameController()->getTowersInRange(this->position, MAX_PICKUP_RANGE);
    for (auto tower : towers)
    {
        if (tower && tower->getSubType() == tower::TYPE::SPECIAL)
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
    this->tokenTexture->render((int) this->getX(), (int) this->getY());

    RenderUtils::render_circular_health((int)getXr(), (int)getYr(), (int)cooldown, (int)maxCooldown,
        RenderUtils::colour_blend(Colour(0, 255, 0, 255), Colour(255, 0, 0, 255), (float)cooldown / maxCooldown),0.5f
    );
}

void SpecialToken::render() {}
void SpecialToken::release() {}