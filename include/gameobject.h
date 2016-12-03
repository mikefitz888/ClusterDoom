#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "RenderManager.h"
#include "VertexBuffer.h"

namespace manager {
	class Manager;
}

namespace graphics {
	class RenderManager;
}

namespace gameobject {
	using graphics::IRenderable;
	using manager::Manager;
	using graphics::RenderManager;
	
	typedef size_t id_t;

	class GameObject : public IRenderable {
		const id_t id_;
	protected:
		Manager* manager;
		RenderManager* render_manager = NULL;
		int x = 0;
		int y = 0;
	public:
		inline GameObject(id_t id, Manager* m) : id_(id), manager(m) {} //Very important to get key from manager (for memory management + networking)
		inline id_t getID() { return id_; }
		virtual void render() override {};
		virtual void init() override {};
		virtual void renderGUI() override {};
		virtual void release() override {};
		inline virtual ~GameObject() {};
		inline int getX() {return x;};
		inline int getY() {return y;};
	};
}

#endif //GAMEOBJECT_H