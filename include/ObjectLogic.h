#ifndef _OBJECT_LOGIC_H
#define _OBJECT_LOGIC_H

#include "gamecore.h"
/*
    ObjectLogic: Provides an interfaces with manager for creating/destroying objects.
*/
class ObjectLogic {
    std::vector<gameobject_ptr> game_objects;
    manager::Manager* manager;

public:
    ObjectLogic(manager::Manager* m);
    GameObject* createObject(id_t key, gameobject::OBJECT_TYPE type);
    gameobject_ptr createObject(gameobject::OBJECT_TYPE type, float x, float y);
    void           removeObject(gameobject_ptr obj);
};


#endif
