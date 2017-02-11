#ifndef TOWER_H
#define TOWER_H

#include "gamecore.h"

#include "gameobject.h"
//#include "manager.h"

namespace tower {
    using graphics::IRenderable;
    using gameobject::GameObject;
    //using gameobject::id_t;
    using gameobject::Point;
    using gameobject::tower_ptr;
    using gameobject::unit_ptr;
    using manager::Manager;

    enum TYPE : unsigned int {BASE=0, BASIC};

    class Tower : public GameObject {
        graphics::Texture* texture;
        graphics::Texture* red;
        sf::Shader* shader;
        graphics::VertexBuffer* vbuff;
        graphics::VertexBuffer* hpbar_buff;
    protected:
        float max_health = 10000;
        float health=max_health;
    public:
        Tower(id_t id, TYPE tower_type, Manager* m);
        virtual void init() override;
        virtual void render() override;
        virtual void renderGUI() override;
        virtual void release() override;

        virtual void step();
        //Gameplay methods
        float getHealth() const;

        virtual void attack(unit_ptr tower); //Tower attacks unit
        virtual void attacked(gameobject_ptr aggressor); //Unit attacks tower
    };
}

#endif //TOWER_H
