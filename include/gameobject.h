#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "gamecore.h"

#include "RenderManager.h"
#include "smartpointers.h"


// This is already defined in the CSTDLib
//#define MAX(x,y) (x>y)?x:y
#define DISTANCE(x1, y1, x2, y2) sqrt(DIST_SQ(x1, y1, x2, y2))
#define DIST_SQ(x1, y1, x2, y2) ((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1))

namespace gameobject {

    /*
    The collision system is designed to be an attachable
    component to gameobjects.

    Collision provides a number of utility functions
    for testing collisions between itself and other collision
    instances.
    */


    class GameObject;
    typedef smartpointers::slave_ptr<tower::Tower> tower_ptr;
    typedef smartpointers::slave_ptr<unit::Unit> unit_ptr;
    typedef smartpointers::slave_ptr<GameObject> gameobject_ptr;
    
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

    enum CollisionType : unsigned int { NONE=0, BOX, CIRCLE };
    class Collision {

    protected:
        CollisionType collision_type;
        gameobject_ptr parent = nullptr;

        int radius;
        BoundingBox bounding_box = BoundingBox(0, 0, 0, 0);
        bool is_collidable;

    private:
        void calculateBoundingCircle();
        void calculateBoundingBox();

    public:
        //Collision();
        Collision(gameobject_ptr parent); // Empty constructor.
        Collision(gameobject_ptr parent, int radius); // Create collision of Type circle
        Collision(gameobject_ptr parent, int bbox_left, int bbox_right, int bbox_up, int bbox_down);
        Collision(gameobject_ptr parent, BoundingBox box);
        void setParent(gameobject_ptr parent);

        void setTypeNone();
        void setTypeCircle(int radius);
        void setTypeSquare(int bbox_left, int bbox_right, int bbox_up, int bbox_down);
        void setTypeSquare(BoundingBox box);
        void setCollidable(bool collidable);
        bool getCollidable();

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


    using graphics::IRenderable;
    using manager::Manager;
    using graphics::RenderManager;

    enum TYPE : unsigned int {TOWER=0, UNIT, OBJECT};
    enum OBJECT_TYPE : unsigned int { SPAWN = 0 };


    /*
        Gameobject abstract class

    */
    class GameObject : public IRenderable {
        const id_t id_;
        const id_t super_type_ = 0;
        const id_t sub_type_ = 0;
    protected:
        Manager* manager;
        RenderManager* render_manager = nullptr;
        Collision collision_profile = Collision(nullptr);

        Point<int> position = Point<int>(0, 0);
        Point<int> jitter_offset = Point<int>(0, 0);

        Point<int> render_position = Point<int>(0, 0);
        Point<int> render_facing = Point<int>(0, 0);


        gameobject_ptr self = nullptr;
        bool run_collision_event = true;


        int _destroySelf();
    public:
        int delete_queue = 0;
        GameObject(id_t id, TYPE super_type, id_t sub_type, Manager* m); //Very important to get key from manager (for memory management + networking)
        virtual ~GameObject();

        id_t getID() const;
        id_t getSuperType() const;
        id_t getSubType() const;
        void setSharedPtr(gameobject_ptr myself);
        gameobject_ptr getSharedPtr();
        void setup(); // The setup to be run for all gameobjects to configure default properties

        // EVENTS
        virtual void render() override;
        virtual void init() override;
        virtual void renderGUI() override;
        virtual void release() override;
        virtual void onCollision(gameobject_ptr other);
        virtual void step();

        // CONTORL & DATA
        int getX() const;
        int getY() const;
        int getXr() const;
        int getYr() const;
        Point<int> getPosition() const;
        int distanceTo(smartpointers::slave_ptr<GameObject> other) const;

        int distanceTo(Point<int> point) const;
        inline Collision* getCollision() { return &this->collision_profile; }

        inline void setX(int x_) { position.x = x_; }
        inline void setY(int y_) { position.y = y_; }
        inline void setPosition(int x, int y) { setX(x); setY(y); }
        inline void setJitter(int x, int y){jitter_offset.x=x; jitter_offset.y=y;};
        inline Point<int> getJitter(){return jitter_offset;};

        inline bool getRunCollisionEvent() { return run_collision_event; }
        inline void setRunCollisionEvent(bool run_collision_event) { this->run_collision_event = run_collision_event; }

        inline void demoDestroy() { _destroySelf(); return; }


        inline void testing(void* arg, void* ret) { std::cout << "Testing" << std::endl; }
        //inline void call(const std::string& fn_name, void* _arg = nullptr, void* _ret = nullptr) {
            /*
            ** Rather basic system to allow more dynamic control over function calls
            ** Only pre-specified methods may be called in this way so hidden methods are not exposed
            */
        /*	std::map<std::string, fn_ptr>::iterator fn = fn_hooks.find(fn_name);
            if (fn != fn_hooks.end()) {
                (this->*(fn->second))(_arg, _ret);
            } else {
                std::cout << "ERROR: Function is not exposed to call()." << std::endl;
            }
        }*/

    };


}

#define destroySelf();  _destroySelf(); return;

#endif //GAMEOBJECT_H
