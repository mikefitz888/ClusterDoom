#ifndef LASERTOWER_H
#define LASERTOWER_H

#include "../gamecore.h"
#include "../tower.h"
#include "../GameObjects/Projectiles.h"

/*
    LaserTower: Shoots bullets with piercing effect, i.e. can hit a few units in a line
*/

namespace tower
{
    class LaserTower : public Tower {
        graphics::Texture* texture = nullptr;
        size_t cooldown = 10;//5;
        size_t timer = 0;
        size_t cost_per_attack = 5;
        size_t max_range = 200;
        //float damage = 4.0f;
        unit_ptr current_target = nullptr;
		bool leftFire = true;
        //bool oth_snd = false;
    public:
        LaserTower(id_t key, Manager* m);
        void init() override;
        void render() override;
        void step() override;
    };
}

#endif //LASERTOWER_H