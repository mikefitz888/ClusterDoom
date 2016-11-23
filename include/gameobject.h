#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "RenderManager.h"

namespace gameobject {
	using graphics::IRenderable;
	
	typedef size_t id_t;

	class GameObject : public IRenderable {
		const id_t id_;
	public:
		GameObject(id_t id); //Very important to get key from manager (for memory management + networking)
		id_t getID();
	};

	GameObject::GameObject(id_t id) : id_(id) {}
	id_t GameObject::getID() { return id_; }
}

#endif //GAMEOBJECT_H