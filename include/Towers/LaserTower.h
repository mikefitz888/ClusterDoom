#ifndef LASERTOWER_H
#define LASERTOWER_H

#include "../gamecore.h"
#include "../tower.h"
#include "../GameObjects/Projectiles.h"

namespace tower
{
    class LaserTower : public Tower {
        graphics::Texture* texture = nullptr;
        size_t cooldown = 5;
        size_t timer = 0;
        size_t cost_per_attack = 5;
        size_t max_range = 200;
        float damage = 4.0f;
        unit_ptr current_target = nullptr;
		bool leftFire = true;
    public:
        LaserTower(id_t key, Manager* m);
        void init() override;
        void render() override;
        void step() override;
    };
}

#endif //LASERTOWER_H