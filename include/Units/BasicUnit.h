#ifndef BASICUNIT_H
#define BASICUNIT_H
#include "../tower.h"

namespace unit
{
    class BasicUnit : public Unit {
        graphics::Texture* texture = nullptr;
    public:
        BasicUnit(id_t key, Manager* m);
        void init() override;
        void render() override;
    };
}

#endif //BASICUNIT_H