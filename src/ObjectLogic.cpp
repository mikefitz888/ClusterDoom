#include "../include/ObjectLogic.h"
#include "../include/GameObjects/Spawn.h"
#include "../include/GameObjects/Projectiles.h"
#include "../include/GameObjects/ResourceMine.h"
#include "../include/manager.h"
#include "../include/gameobject.h"
#include "../include/GameObjects/GameStateNotifier.h"

GameObject* ObjectLogic::createObject(id_t key, gameobject::OBJECT_TYPE type) {
    switch (type) {
    case gameobject::OBJECT_TYPE::SPAWN:
        return new Spawn( key, manager);
        break;

    case gameobject::OBJECT_TYPE::PROJECTILE_BOMB:
        return new ProjectileBomb(key, manager);
        break;

    case gameobject::OBJECT_TYPE::PROJECTILE_LASER:
        return new ProjectileLaser(key, manager);
        break;

    case gameobject::OBJECT_TYPE::PROJECTILE_ELECTRICITY:
        return new ProjectileElectricity(key, manager);
        break;

    case gameobject::OBJECT_TYPE::GAME_STATE_NOTIFIER:
        return new GameStateNotifier(key, manager);
        break;

    case gameobject::OBJECT_TYPE::RESOURCE_MINE:
        return new ResourceMine(key, manager);
        break;

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
