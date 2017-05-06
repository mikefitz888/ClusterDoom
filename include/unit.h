#ifndef UNIT_H
#define UNIT_H

#include "gamecore.h"
#include "gameobject.h"
#include "GameObjects\ResourceMine.h"

namespace unit {
    enum TYPE : unsigned int {BASE=0, BASIC, WIZARD, PIRATE};

    class Unit : public GameObject {
		graphics::AnimatedTexture* texture = nullptr;
		graphics::AnimatedTexture* deathTexture = nullptr;
		float textureSize = 1.0f;
        graphics::Texture* red;
        sf::Shader* shader;

        smartpointers::slave_ptr<ResourceMine> mine = nullptr;
        bool mineIsTarget = false;
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
        float glacial_effect_vis = 1.0f;
        size_t cooldown = 0;

		float animationProgress = 0.0f;

        void deliverWealth(size_t amt);
        graphics::Texture* unit_shadow, *unit_ice;

		bool dead = false;

    public:
        Unit(id_t id, TYPE unit_type, Manager* m);

		void renderAnimation(graphics::AnimatedTexture* texture, float size, float animationSpeed, int numFrames, float sizeMod, float rotMod);

		bool isDead();

        //IRenderable methods
        virtual void init() override;
        virtual void render() override;
        virtual void renderGUI() override;
        virtual void release() override;
        virtual void renderBegin() override;

        //Gameplay methods
        virtual void step();
        virtual void attack(tower_ptr tower);
        virtual void attacked(gameobject_ptr aggressor);
        virtual void attacked(gameobject_ptr aggressor, float damage);

        virtual void targetMine(smartpointers::slave_ptr<ResourceMine> t_mine);
    };
}

#endif //UNIT_H
