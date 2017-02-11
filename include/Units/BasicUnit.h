#ifndef BASICUNIT_H
#define BASICUNIT_H

#include "../gamecore.h"

#include "../tower.h"
#include "../unit.h"

namespace unit
{
    class BasicUnit : public Unit {
        graphics::Texture* texture = nullptr;
    public:
        BasicUnit(id_t key, Manager* m);
        void init() override;
        void render() override;
        void onCollision(gameobject_ptr other) override;
    };
}

#endif //BASICUNIT_H