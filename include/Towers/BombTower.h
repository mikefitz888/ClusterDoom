#ifndef BOMBTOWER_H
#define BOMBTOWER_H

#include "../gamecore.h"
#include "../tower.h"
#include "../GameObjects/Projectiles.h"

namespace tower
{
    class BombTower : public Tower {
        graphics::Texture* texture = nullptr;
        size_t cooldown = 600;
        size_t timer = 0;
        size_t cost_per_attack = 10;
        size_t max_range = 200;
        float damage = 1.0f;
        unit_ptr current_target = nullptr;
        std::vector<gameobject_ptr> bomb_queue;
    public:
        BombTower(id_t key, Manager* m);
        void init() override;
        void render() override;
        void step() override;
    };
}

#endif //BOMBTOWER_H