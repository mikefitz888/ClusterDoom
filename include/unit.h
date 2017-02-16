#ifndef UNIT_H
#define UNIT_H

#include "gamecore.h"
#include "gameobject.h"

namespace unit {
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

    public:
        Unit(id_t id, TYPE unit_type, Manager* m);
        //IRenderable methods
        virtual void init() override;
        virtual void render() override;
        virtual void renderGUI() override;
        virtual void release() override;

        //Gameplay methods
        virtual void step();
        virtual void attack(tower_ptr tower);
        virtual void attacked(gameobject_ptr aggressor);
    };
}

#endif //UNIT_H
