#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "RenderManager.h"
#include "VertexBuffer.h"
#include "smartpointers.h"
#include "math.h"


namespace manager {
	class Manager;
}

namespace graphics {
	class RenderManager;
}
namespace tower {
	class Tower;
}

namespace unit {
	class Unit;
}

namespace gameobject {
	using graphics::IRenderable;
	using manager::Manager;
	using graphics::RenderManager;

	typedef smartpointers::slave_ptr<tower::Tower> tower_ptr;
	typedef smartpointers::slave_ptr<unit::Unit> unit_ptr;
	
	typedef size_t id_t;

	template <typename T>
	struct Point {
		inline Point(T x_, T y_) : x(x_), y(y_) {}
		T x;
		T y;
		inline T distanceTo(Point target) {return sqrt((target.x-x)*(target.x-x)+(target.y-y)*(target.y-y)); };
	};

	class GameObject : public IRenderable {
		const id_t id_;
		const id_t type_ = 0; // TODO: EACH OBJECT NEEDS ITS OWN TYPE
	protected:
		Manager* manager;
		RenderManager* render_manager = nullptr;
		Point<int> position = Point<int>(0, 0);
		int _destroySelf();
	public:
		inline GameObject(id_t id, Manager* m) : id_(id), manager(m) {} //Very important to get key from manager (for memory management + networking)
		inline id_t getID() const { return id_; }
		inline id_t getType() const { return type_; }
		virtual void render() override { }
		virtual void init() override { }
		virtual void renderGUI() override { }
		virtual void release() override { }
		inline virtual void step() { };
		inline virtual ~GameObject() { }

		inline int getX() const { return position.x; }
		inline int getY() const { return position.y; }
		inline Point<int> getPosition() const { return position; }
		int distanceTo(smartpointers::slave_ptr<GameObject> other) const;

		inline void setX(int x_) { position.x = x_; }
		inline void setY(int y_) { position.y = y_; }
		inline void setPosition(int x, int y) { setX(x); setY(y); }
	};
}

#define destroySelf();  _destroySelf(); return;

#endif //GAMEOBJECT_H
