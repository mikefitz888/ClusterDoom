#ifndef ELECTRICTOWER_H
#define ELECTRICTOWER_H

#include "../gamecore.h"
#include "../tower.h"
#include "../GameObjects/Projectiles.h"

namespace tower
{
    class ElectricTower : public Tower {
        graphics::Texture* texture = nullptr;
        size_t cooldown = 20;
        size_t timer = 0;
        size_t cost_per_attack = 10;
        float damage = 1.0f;
    public:
        ElectricTower(id_t key, Manager* m);
        void init() override;
        void render() override;
        void step() override;
    };
}

#endif //ELECTRICTOWER_H