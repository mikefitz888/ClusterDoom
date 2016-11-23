#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "RenderManager.h"

namespace gameobject {
	using graphics::IRenderable;
	
	typedef size_t id_t;

	class GameObject : public IRenderable {
		const id_t id_;
	public:
		inline GameObject(id_t id) : id_(id) {} //Very important to get key from manager (for memory management + networking)
		inline id_t getID() { return id_; }
		inline virtual ~GameObject() {};
	};
}

#endif //GAMEOBJECT_H