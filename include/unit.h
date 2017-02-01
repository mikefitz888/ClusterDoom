#ifndef UNIT_H
#define UNIT_H

#include "gamecore.h"

#include <vector>
#include "gameobject.h"
#include "gamecontroller.h"

void aStar(gameobject::Point<int> start, gameobject::Point<int> end, const int w, const int h, gamecontroller::GameController *gc, std::vector<gameobject::Point<int>>& ret);

namespace manager {
    class Manager;
}

namespace unit {
    using gameobject::id_t;
    using gameobject::GameObject;
    using gameobject::tower_ptr;
    using gameobject::unit_ptr;
    using gameobject::Point;
    using manager::Manager;
    //using containers::optional;

    enum TYPE : unsigned int {BASE=0, BASIC};

    class Unit : public GameObject {
        graphics::Texture* texture;
        graphics::Texture* red;
        sf::Shader* shader;
        graphics::VertexBuffer* vbuff;
        graphics::VertexBuffer* hpbar_buff;
    protected:
        tower_ptr getNearestTower() const;
        Point<float> velocity = Point<float>(0, 0);
        float health = 1000;
		std::vector<Point<int>> path;

    public:
        inline Unit(id_t id, TYPE unit_type, Manager* m) : GameObject(id, gameobject::TYPE::UNIT, unit_type, m) {}
        //IRenderable methods
        virtual void init() override;
        virtual void render() override;
        virtual void renderGUI() override;
        virtual void release() override;
		void getPath(Point<int> target);

        //Gameplay methods
        virtual void step() override;
        virtual void attack(tower_ptr tower);
        virtual void attacked(GameObject& aggressor);
    };
}

#endif //UNIT_H
