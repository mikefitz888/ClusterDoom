#include "../include/gameobject.h"
#include "../include/manager.h"
#include "../include/VertexBuffer.h"

namespace gameobject {

    //// GAME OBJECT
    GameObject::GameObject(id_t id, TYPE super_type, id_t sub_type, Manager* m) :
        id_(id),
        super_type_(super_type),
        sub_type_(sub_type),
        manager(m),
        game_controller(m->getGameController()) {}

    GameObject::~GameObject() {}

    id_t GameObject::getID() const {
        return id_;
    }

    id_t GameObject::getSuperType() const {
        return super_type_;
    }

    id_t GameObject::getSubType() const {
        return sub_type_;
    }

    void GameObject::setSharedPtr(gameobject_ptr myself) {
        this->self = myself;
    }

    gameobject_ptr GameObject::getSharedPtr() {
        return this->self;
    }

    void GameObject::setReady(bool ready) {
        this->is_ready = ready;
    }

    bool GameObject::getReady() {
        return this->is_ready;
    }

    bool GameObject::getNetworkSend() {
        return !this->dont_send;
    }

    void GameObject::renderBegin() {}
    void GameObject::render() {}
    void GameObject::init() {}
    void GameObject::renderGUI() {}
    void GameObject::release() {}
    void GameObject::onCollision(gameobject_ptr other) {}
    void GameObject::step() {
        this->navigationStep();
        this->MotionComponent::motionStep();
    }
    void GameObject::writeNetworkUpdate(int event_id, Buffer &buffer) {}
    void GameObject::recvNetworkInteraction(int event_id, Buffer &buffer) {}

    int GameObject::_destroySelf() {
        manager->destroy(this);
        return 0;
    }

    /* int GameObject::getX() const { return position.x; }
    int GameObject::getY() const { return position.y; }
    int GameObject::getXr() const { return render_position.x; }
    int GameObject::getYr() const { return render_position.y; }
    Point<int> GameObject::getPosition() const { return position; }*/

    int GameObject::distanceTo(smartpointers::slave_ptr<GameObject> other) const {
        return (int)glm::distance(other->getPosition(), position);

    }

    int GameObject::distanceTo(Point<float> point) const {
        return (int)glm::distance(vec2(point.x, point.y), position);
    }

    int GameObject::distanceTo(glm::vec2 point) const {
        return (int)glm::distance(point, position);
    }

    void GameObject::setNetworkSync(bool sync) {
        this->dont_send = !sync;
    }

    // Setup
    /*
    A script that runs before init to setup gameobject specific properties
    such as collision profiles.
    (This then allows a gameobject to modify the collision profile
    within its init funtion).
    */
    void GameObject::setup() {
        this->collision_profile.setParent(this->getSharedPtr());
    }


    //// COLLISION HANDLING
    BoundingBox::BoundingBox(int bbox_left, int bbox_right, int bbox_up, int bbox_down) {
        this->bbox_left = bbox_left;
        this->bbox_right = bbox_right;
        this->bbox_up = bbox_up;
        this->bbox_down = bbox_down;
    }

    // Create new empty collision profile
    Collision::Collision(gameobject_ptr parent) {
        //std::cout << "PARENT: " << parent << std::endl;
        this->parent = parent;
        this->setTypeNone();
    }

    // Creates a new collision circle
    Collision::Collision(gameobject_ptr parent, int radius) {
        this->parent = parent;
        this->setTypeCircle(radius);
    }

    // Creates a new collision bounding box
    Collision::Collision(gameobject_ptr parent, int bbox_left, int bbox_right, int bbox_up, int bbox_down) {
        this->parent = parent;
        this->setTypeSquare(bbox_left, bbox_right, bbox_up, bbox_down);
    }
    Collision::Collision(gameobject_ptr parent, BoundingBox box) {
        this->parent = parent;
        this->setTypeSquare(box);
    }

    // Set the parent instance
    void Collision::setParent(gameobject_ptr parent) {
        this->parent = parent;
    }

    // Changes the type of collision to a circle
    void Collision::setTypeCircle(int radius) {
        this->radius = radius;
        this->collision_type = CollisionType::CIRCLE;
        this->calculateBoundingBox();
        is_collidable = true;
    }

    // Changes the type of a collision to a bounding box
    void Collision::setTypeSquare(int bbox_left, int bbox_right, int bbox_up, int bbox_down) {
        this->setTypeSquare(BoundingBox(bbox_left, bbox_right, bbox_up, bbox_down));
    }

    void Collision::setTypeSquare(BoundingBox box) {
        this->bounding_box = box;
        this->calculateBoundingCircle();
        this->is_collidable = true;
    }

    void Collision::setTypeNone() {
        this->is_collidable = false;
        this->bounding_box = BoundingBox(0, 0, 0, 0);
        this->radius = 0;
        this->collision_type = CollisionType::NONE;
    }


    // Calculates the bounding circle of a bounding box
    /*
    This function along with the one to calculate bounding box
    ensure that every collision object always has a clear bounds
    defined in two forms.
    - This is because collision optimised collision structures
    may require a quick samping of this information.
    */
    // Optimisation; why are we calculating 4 square roots here? might as well
    // do one square root!
    // ***was 4 DISTANCE calls, now 4 DIST_SQ calls***
    void Collision::calculateBoundingCircle() {
        this->radius = (int)sqrt(MAX(
            MAX(
                DIST_SQ(0, 0, this->bounding_box.bbox_left, this->bounding_box.bbox_up),
                DIST_SQ(0, 0, this->bounding_box.bbox_left, this->bounding_box.bbox_down)
            ),
            MAX(
                DIST_SQ(0, 0, this->bounding_box.bbox_right, this->bounding_box.bbox_up),
                DIST_SQ(0, 0, this->bounding_box.bbox_right, this->bounding_box.bbox_down)
            )
        ));
    }

    // Calculates the bounding box of a collision
    void Collision::calculateBoundingBox() {
        this->bounding_box.bbox_left = -this->radius;
        this->bounding_box.bbox_right = this->radius;
        this->bounding_box.bbox_up = -this->radius;
        this->bounding_box.bbox_down = this->radius;
    }

    // Toggles whether the object is collidable or not.
    void Collision::setCollidable(bool collidable) {
        this->is_collidable = collidable;
    }

    bool Collision::getCollidable() {
        return this->is_collidable;
    }

    // Returns the bounding radius
    /*
    If collision type circle, this matches the set radius.
    If collision type is square, it'll be a circle that
    encompasses the entire bounding box
    */
    int Collision::getBoundingRadius() {
        return this->radius;
    }

    // Returns the bounding box
    /*
    If collision type is square, this will return the same
    bounding box as the one set.
    If collision type is circle, this will return a bounding
    box that encompasses the circle
    */
    BoundingBox Collision::getBoundingBox() {
        return this->bounding_box;
    }

    // Return parent gameobject_ptr
    gameobject_ptr Collision::getParent() {
        return this->parent;
    }

    //// COLLISION FUNCTIONS;
    bool Collision::intersects(Collision* collision) {
        return intersects(
            collision,
            (int) parent->getX(),
            (int) parent->getY(),
            (int) collision->getParent()->getX(),
            (int) collision->getParent()->getY());
    }

    /*
    Performs a collision by virtually moving own collision profile
    to (x,y) and testing against the collision passed in.
    */
    bool Collision::intersects(Collision* collision, int x, int y) {
        return intersects(
            collision,
            x,
            y,
            (int) collision->getParent()->getX(),
            (int) collision->getParent()->getY());
    }

    /*
    Performs a collision by moving own collision profile to (x,y)
    AND moving passed in collision profile to (other_x, other_y)
    and testing for a collision in that given location.

    (This function is the collision function where all the work
    gets done).
    */
    bool Collision::intersects(Collision* collision, int x, int y, int other_x, int other_y) {

        if (!this->is_collidable) { return false; }
        if (!collision->is_collidable) { return false; }
        // Switch on type of collision profile:
        switch (this->collision_type) {

            // Uses circular collisions:
        case CollisionType::CIRCLE:

            // Switch on other collision type:
            switch (collision->collision_type) {

                // Circle-circle intersection
            case CollisionType::CIRCLE:
                return circle_intersects(x, y, this->radius,
                    other_x, other_y,
                    collision->getBoundingRadius());

                //Circle-box intersection
            case CollisionType::BOX:
                return circle_box_intersects(x, y, this->radius,
                    other_x, other_y,
                    collision->getBoundingBox());

                // No collision profile for other
            case CollisionType::NONE:
                return false;
            }
            break;

            // Uses box collisions:
        case CollisionType::BOX:
            // Switch on other collision type:
            switch (collision->collision_type) {

                // Box-circle intersection
            case CollisionType::CIRCLE:
                return circle_box_intersects(
                    other_x, other_y,
                    collision->getBoundingRadius(),
                    x, y, this->bounding_box);

                //box-box intersection
            case CollisionType::BOX:
                return box_intersects(
                    x, y, this->bounding_box,
                    other_x, other_y,
                    collision->getBoundingBox());

                // No collision profile for other
            case CollisionType::NONE:
                return false;
            }
            break;

            // No collision profile
        case CollisionType::NONE:
            return false;
        }
        return false;
    }

    //// Collision Utility functions
    bool Collision::circle_intersects(int x1, int y1, int r1, int x2, int y2, int r2) {

        double dist = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
        if (dist <= r1 + r2) {
            return true;
        }

        return false;
    }
    bool Collision::box_intersects(int x1, int y1, BoundingBox a, int x2, int y2, BoundingBox b) {
        return (abs(a.bbox_left - b.bbox_left) * 2 < (a.bbox_right - a.bbox_left + b.bbox_right - b.bbox_left)) &&
            (abs(a.bbox_up - b.bbox_up) * 2 < (a.bbox_down - a.bbox_up + b.bbox_down - b.bbox_up));
    }
    bool Collision::circle_box_intersects(int cx, int cy, int radius, int bx, int by, BoundingBox box) {
        return false;
    }

    // ****************************************************************************** //
    // GameObject motion component
    void MotionComponent::motionStep() {
        if (frozen) return;
        position += velocity;
        velocity *= friction;

        render_position += (position - render_position)*smoothing_rate;
    }

    void MotionComponent::setPosition(float x, float y) {
        this->position = glm::vec2(x, y);
        this->render_position = this->position;
    }

    void MotionComponent::setPosition(glm::vec2 position) {
        this->setPosition(position.x, position.y);
    }


    void MotionComponent::setPositionSmooth(int x, int y) {
        this->position = glm::ivec2(x, y);
    }

    void MotionComponent::setPositionSmooth(glm::vec2 position) {
        this->setPositionSmooth((int)position.x, (int)position.y);
    }

    void MotionComponent::setX(float x) {
        this->position.x = x;
    }

    void MotionComponent::setY(float y) {
        this->position.y = y;
    }

    void MotionComponent::setVelocity(float x, float y) {
        this->velocity = vec2(x, y);
    }

    void MotionComponent::setVelocity(glm::vec2 velocity) {
        this->velocity = velocity;
    }

    void MotionComponent::setSmoothingRate(float rate) {
        this->smoothing_rate = rate;
    }

    void MotionComponent::setFriction(vec2 friction) {
        this->friction = friction;
    }

    void MotionComponent::setFrozen(bool state) {
        frozen = state;
    }

    void MotionComponent::addVelocity(vec2 vel) {
        this->velocity += vel;
    }

    float MotionComponent::getX() const {
        return this->position.x;
    }

    float MotionComponent::getY() const {
        return this->position.y;
    }

    float MotionComponent::getXr() const {
        return this->render_position.x;
    }

    float MotionComponent::getYr() const {
        return this->render_position.y;
    }

    vec2 MotionComponent::getPosition() const {
        return this->position;
    }

    float MotionComponent::getVelocityX() const {
        return this->velocity.x;
    }

    float MotionComponent::getVelocityY() const {
        return this->velocity.y;
    }

    vec2 MotionComponent::getVelocity() const {
        return this->velocity;
    }

    float MotionComponent::getSmoothingRate()const {
        return this->smoothing_rate;
    }

    vec2 MotionComponent::getFriction() const {
        return this->friction;
    }
    // ****************************************************************************** //

    // ****************************************************************************** //
    // GameObject navigation component
    void NavigationComponent::navigationStep() {

        // Follow path
        if (this->is_following_path && !this->path_complete) {

            // Check if we have arrived at our destination
            if (this->getAtDestination()) {

                // Check if there are more nodes:
                if ((unsigned)current_target_node_id < path.size() - 1) {
                    current_target_node_id++;
                    this->setDestination(path[current_target_node_id], this->speed);
                    this->path_complete = false;
                }
                else {
                    this->path_complete = true; // We have finished the path
                }

            }
            else {
                // Force destination to next node
                this->setDestination(path[current_target_node_id], this->speed);
                this->path_complete = false;
            }
        }

        // Process destination
        if (this->has_destination) {

            // If we are not at our destination, navigate towards.
            /*
            This is done by setting the velocity of the object
            */
            if (!this->getAtDestination()) {
                vec2 direction = glm::normalize(this->current_destination - this->position);
                this->setVelocity(direction * this->speed);

            }
            else {
                this->setVelocity(vec2(0));
            }
        }
    }


    void NavigationComponent::setPath(Path path, float speed) {
        if (path.size() > 0) {
            this->path = path;
            this->current_target_node_id = 0;
            this->setDestination(path[this->current_target_node_id], speed);
            this->is_following_path = true;
            this->path_complete = false;
        }
    }

    void NavigationComponent::setDestination(int x, int y, float speed) {
        this->setDestination(vec2(x, y), speed);
    }

    void NavigationComponent::setDestination(vec2 destination, float speed) {
        this->current_destination = destination;
        this->speed = speed;
        this->has_destination = true;
        this->at_destination = (glm::distance(this->current_destination, this->position) <= this->distance_threshold);
    }

    void NavigationComponent::setDestination() {
        this->clearDestination();
    }

    void NavigationComponent::setDistanceThreshold(int distance) {
        this->distance_threshold = distance;
    }

    void NavigationComponent::clearDestination() {
        this->has_destination = false;
    }

    void NavigationComponent::clearPath() {
        if (this->is_following_path) {
            this->is_following_path = false;
            this->path.clear();
            this->clearDestination();
        }
    }

    void NavigationComponent::restartPath() {
        if (this->is_following_path) {
            this->setPath(this->path, this->speed);
        }
    }

    bool NavigationComponent::getFollowingPath() {
        return this->is_following_path;
    }

    bool NavigationComponent::getPathComplete() {
        return this->path_complete;
    }

    int  NavigationComponent::getPathnodeID() {
        return this->current_target_node_id;
    }

    int  NavigationComponent::getPathLength() {
        return (is_following_path) ? this->path.size() : 0;
    }

    vec2 NavigationComponent::getDestination() {
        return this->current_destination;
    }

    int  NavigationComponent::getDistanceThreshold() {
        return this->distance_threshold;
    }

    bool NavigationComponent::getHasDestination() {
        return this->has_destination;
    }

    bool NavigationComponent::getAtDestination() {
        this->at_destination = glm::distance(this->current_destination, this->position) <= this->distance_threshold;
        return this->at_destination;
    }

    Path NavigationComponent::getPath() {
        return this->path;
    }

    // ****************************************************************************** //
}