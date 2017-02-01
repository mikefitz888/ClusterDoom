#ifndef _OBJECT_LOGIC_H
#define _OBJECT_LOGIC_H

#include "gamecore.h"

#include "gameobject.h"

class ObjectLogic {
	using gameobject::gameobject_ptr;
	using gameobject::GameObject;
	std::vector<gameobject_ptr> game_objects;
	Manager *manager;

public:
	inline ObjectLogic(Manager *m) : manager(m) {};
	GameObject* createObject(gameobject::id_t key, gameobject::OBJECT_TYPE type);
	gameobject_ptr createObject(gameobject::OBJECT_TYPE type);
	void           removeObject(gameobject_ptr obj);
};


#endif
