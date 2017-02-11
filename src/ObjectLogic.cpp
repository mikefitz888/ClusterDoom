#include "../include/ObjectLogic.h"
#include "../include/GameObjects/Spawn.h"
#include "../include/manager.h"
#include "../include/gameobject.h"

GameObject* ObjectLogic::createObject(id_t key, gameobject::OBJECT_TYPE type) {
    switch (type) {
    case gameobject::OBJECT_TYPE::SPAWN:
        return new Spawn( key, manager);
        break;
    default:
        std::cout << "FATAL ERROR! INCORRECT OBJECT INSTANCE, nullptr RETURNED" << std::endl;
        return nullptr;
    }
}

ObjectLogic::ObjectLogic(manager::Manager* m) :
    manager(m) {}

gameobject_ptr ObjectLogic::createObject(gameobject::OBJECT_TYPE type) {
    return manager->createObject(type);
}

void ObjectLogic::removeObject(gameobject_ptr obj) {

}
