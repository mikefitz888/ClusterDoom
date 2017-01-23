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
	class GameObject;

    using graphics::IRenderable;
    using manager::Manager;
    using graphics::RenderManager;

    enum TYPE : unsigned int {TOWER=0, UNIT, OBJECT};
	enum OBJECT_TYPE : unsigned int { SPAWN = 0 };

    typedef smartpointers::slave_ptr<tower::Tower> tower_ptr;
    typedef smartpointers::slave_ptr<unit::Unit> unit_ptr;
	typedef smartpointers::slave_ptr<GameObject> gameobject_ptr;
    
    typedef size_t id_t;

    template <typename T>
    struct Point {
        inline Point(T x_, T y_) : x(x_), y(y_) {}
        T x;
        T y;
        inline T distanceTo(Point target) const {return sqrt((target.x-x)*(target.x-x)+(target.y-y)*(target.y-y)); };
    };

    class GameObject : public IRenderable {
        const id_t id_;
        const id_t super_type_ = 0;
        const id_t sub_type_ = 0;
    protected:
        Manager* manager;
        RenderManager* render_manager = nullptr;
        Point<int> position = Point<int>(0, 0);
        Point<int> jitter_offset = Point<int>(0, 0);
        Point<int> render_position = Point<int>(0, 0);

        int delete_queue = 0;
        int _destroySelf();
    public:
        inline GameObject(id_t id, TYPE super_type, id_t sub_type, Manager* m) : id_(id), super_type_(super_type), sub_type_(sub_type), manager(m) {} //Very important to get key from manager (for memory management + networking)
        inline id_t getID() const { return id_; }
        inline id_t getSuperType() const { return super_type_; }
        inline id_t getSubType() const { return sub_type_; }
        virtual void render() override { }
        virtual void init() override { }
        virtual void renderGUI() override { }
        virtual void release() override { }
        inline virtual void step() { };
        inline virtual ~GameObject() { }

        inline int getX() const { return position.x; }
        inline int getY() const { return position.y; }
        inline int getXr() const { return render_position.x; }
        inline int getYr() const { return render_position.y; }
        inline Point<int> getPosition() const { return position; }
        float distanceTo(smartpointers::slave_ptr<GameObject> other) const;
		float distanceTo(Point<int> point) const;

        inline void setX(int x_) { position.x = x_; }
        inline void setY(int y_) { position.y = y_; }
        inline void setPosition(int x, int y) { setX(x); setY(y); }
        inline void setJitter(int x, int y){jitter_offset.x=x; jitter_offset.y=y;};
        inline Point<int> getJitter(){return jitter_offset;};

		inline void demoDestroy() { _destroySelf(); return; }
    };
}

#define destroySelf();  _destroySelf(); return;

#endif //GAMEOBJECT_H
