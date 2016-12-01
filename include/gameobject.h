#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "RenderManager.h"

namespace manager {
	class Manager;
}

namespace gameobject {
	using graphics::IRenderable;
	using manager::Manager;
	
	typedef size_t id_t;

	class GameObject : public IRenderable {
		const id_t id_;
		const Manager* manager;
	public:
		inline GameObject(id_t id, Manager* m) : id_(id), manager(m) {} //Very important to get key from manager (for memory management + networking)
		inline id_t getID() { return id_; }
		virtual void render() override {};
		virtual void init() override {};
		virtual void renderGUI() override {};
		virtual void release() override {};
		inline virtual ~GameObject() {};
	};
}

#endif //GAMEOBJECT_H