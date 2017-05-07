#include "../include/ObjectLogic.h"
#include "../include/GameObjects/Spawn.h"
#include "../include/GameObjects/Projectiles.h"
#include "../include/GameObjects/ResourceMine.h"
#include "../include/manager.h"
#include "../include/gameobject.h"
#include "../include/GameObjects/GameStateNotifier.h"
#include "../include/GameObjects/SpecialToken.h"
#include "../include/AttackerEffects.h"

GameObject* ObjectLogic::createObject(id_t key, gameobject::OBJECT_TYPE type) {
    switch (type) {
    case gameobject::OBJECT_TYPE::SPAWN:
        return new Spawn( key, manager);

    case gameobject::OBJECT_TYPE::PROJECTILE_BOMB:
        return new ProjectileBomb(key, manager);

    case gameobject::OBJECT_TYPE::PROJECTILE_LASER:
        return new ProjectileLaser(key, manager);

    case gameobject::OBJECT_TYPE::PROJECTILE_ELECTRICITY:
        return new ProjectileElectricity(key, manager);

    case gameobject::OBJECT_TYPE::GAME_STATE_NOTIFIER:
        return new GameStateNotifier(key, manager);

    case gameobject::OBJECT_TYPE::RESOURCE_MINE:
        return new ResourceMine(key, manager);

    case gameobject::OBJECT_TYPE::TOKEN_MAGNETIC:
        return new SpecialToken(key, manager, gameobject::OBJECT_TYPE::TOKEN_MAGNETIC);

    case gameobject::OBJECT_TYPE::TOKEN_GLACIAL:
        return new SpecialToken(key, manager, gameobject::OBJECT_TYPE::TOKEN_GLACIAL);

    case gameobject::OBJECT_TYPE::TOKEN_WINDY:
        return new SpecialToken(key, manager, gameobject::OBJECT_TYPE::TOKEN_WINDY);

    case gameobject::OBJECT_TYPE::EFFECT_EMP:
        return new EMPEffect(key, manager);

    case gameobject::OBJECT_TYPE::EFFECT_DISRUPTION:
        return new DisruptionEffect(key, manager);

    case gameobject::OBJECT_TYPE::EFFECT_HEAL:
        return new UnitHealEffect(key, manager);

    default:
        std::cout << "FATAL ERROR! INCORRECT OBJECT INSTANCE, nullptr RETURNED" << std::endl;
        return nullptr;
    }
}

ObjectLogic::ObjectLogic(manager::Manager* m) :
    manager(m) {}

gameobject_ptr ObjectLogic::createObject(gameobject::OBJECT_TYPE type, float x, float y) {
    return manager->createObject(type, x, y);
}

void ObjectLogic::removeObject(gameobject_ptr obj) {

}
