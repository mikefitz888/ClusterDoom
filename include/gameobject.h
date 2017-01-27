#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "RenderManager.h"
#include "VertexBuffer.h"
#include "smartpointers.h"
#include "math.h"

#define MAX(x,y) (x>y)?x:y
#define DISTANCE(x1, y1, x2, y2) sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1))


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
		Collision* collision_profile = nullptr;

        Point<int> position = Point<int>(0, 0);
        Point<int> jitter_offset = Point<int>(0, 0);

        Point<int> render_position = Point<int>(0, 0);
        Point<int> render_facing = Point<int>(0, 0);

        int _destroySelf();
    public:
        int delete_queue = 0;
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

	/*
		The collision system is designed to be an attachable
		component to gameobjects.

		Collision provides a number of utility functions
		for testing collisions between itself and other collision
		instances.
	
	
	
	*/
	struct BoundingBox {
		int bbox_left, bbox_up, bbox_down, bbox_right;
		BoundingBox(int bbox_left, int bbox_right, int bbox_up, int bbox_down);
	};

	enum CollisionType { NONE, BOX, CIRCLE };
	class Collision {

	protected:
		CollisionType collision_type;
		gameobject_ptr parent = nullptr;

		int radius;
		BoundingBox bounding_box = BoundingBox(0,0,0,0);
		bool is_collidable;

	private:
		void calculateBoundingCircle();
		void calculateBoundingBox();

	public:
		Collision(gameobject_ptr parent); // Empty constructor.
		Collision(gameobject_ptr parent, int radius); // Create collision of Type circle
		Collision(gameobject_ptr parent, int bbox_left, int bbox_right, int bbox_up, int bbox_down);
		Collision(gameobject_ptr parent, BoundingBox box);

		void setTypeNone();
		void setTypeCircle(int radius);
		void setTypeSquare(int bbox_left, int bbox_right, int bbox_up, int bbox_down);
		void setTypeSquare(BoundingBox box);
		void setCollidable(bool collidable);

		bool intersects(Collision* collision);
		bool intersects(Collision* collision, int x, int y);
		bool intersects(Collision* collision, int x, int y, int other_x, int other_y);
		int  getBoundingRadius();
		BoundingBox getBoundingBox();
		gameobject_ptr getParent();

		// General collision utility functions
		static bool circle_intersects(int x1, int y1, int r1, int x2, int y2, int r2);
		static bool box_intersects(int x1, int y1, BoundingBox a, int x2, int y2, BoundingBox b);
		static bool circle_box_intersects(int cx, int cy, int radius, int bx, int by, BoundingBox box);
	};
}

#define destroySelf();  _destroySelf(); return;

#endif //GAMEOBJECT_H
