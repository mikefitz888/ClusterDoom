#ifndef UNITLOGIC_H
#define UNITLOGIC_H

#include "gamecore.h"

namespace unitlogic {
    using unit::Unit;
    using manager::Manager;

    class UnitLogic {
        std::vector<unit_ptr> units;
        Manager* manager;
    public:
        UnitLogic(Manager* m);
        std::vector<unit_ptr> getUnits() const;
        void clean();
        Unit* createUnit(id_t key, unit::TYPE type) const;
        unit_ptr createUnit(unit::TYPE type, float x, float y) const;
        void removeUnit(unit_ptr unit);

        void giveSlavePtr(unit_ptr unit);
    };
}

#endif //UNITLOGIC_H
