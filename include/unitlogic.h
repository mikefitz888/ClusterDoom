#ifndef UNITLOGIC_H
#define UNITLOGIC_H

#include <vector>
#include "smartpointers.h"
#include "unit.h"
#include "manager.h"

namespace manager {
    class Manager;
}

namespace unit {
    class Unit;
}

namespace unitlogic {
    using unit::Unit;
    using manager::Manager;
    using gameobject::id_t;

    typedef smartpointers::slave_ptr<Unit> unit_ptr;

    class UnitLogic {
        std::vector<unit_ptr> units;
        Manager* manager;
    public:
        inline UnitLogic(Manager* m) : manager(m) {};
        inline std::vector<unit_ptr> getUnits() const {    return units; }
        inline void clean() { units.erase(std::remove_if(units.begin(), units.end(), [](unit_ptr &x) {return !(x.valid()); }), units.end()); }
        Unit* createUnit(id_t key, unit::TYPE type) const;
        unit_ptr createUnit(unit::TYPE type) const;
        void removeUnit(unit_ptr unit);
        
        void giveSlavePtr(unit_ptr unit);
    };
}

#endif //UNITLOGIC_H