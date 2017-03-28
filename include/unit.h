#ifndef UNIT_H
#define UNIT_H

#include "gamecore.h"
#include "gameobject.h"

namespace unit {
    enum TYPE : unsigned int {BASE=0, BASIC, WIZARD};

    class Unit : public GameObject {
        graphics::Texture* texture;
        graphics::Texture* red;
        sf::Shader* shader;
        //graphics::VertexBuffer* vbuff;
        //graphics::VertexBuffer* hpbar_buff;

    protected:
        tower_ptr getNearestTower() const;
        Point<float> velocity = Point<float>(0, 0);
        float health = 1000;
		float maxHealth = 1000;
        float health_timer = 0;
        float health_alpha = 0;
        float health_visual = maxHealth;
		float unitSpeed = 1.0f;
        size_t cooldown = 0;

        void deliverWealth(size_t amt);

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
        virtual void attacked(gameobject_ptr aggressor, float damage);
    };
}

#endif //UNIT_H
