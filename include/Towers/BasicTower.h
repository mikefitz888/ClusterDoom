#ifndef BASICTOWER_H
#define BASICTOWER_H

#include "../gamecore.h"
#include "../tower.h"

namespace tower
{
    class BasicTower : public Tower {
        graphics::Texture* texture = nullptr;
    public:
        BasicTower(id_t key, Manager* m);
        void init() override;
        void render() override;
    };
}

#endif //BASICTOWER_H