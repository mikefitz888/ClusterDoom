#ifndef UNITLOGIC_H
#define UNITLOGIC_H

#include "gamecore.h"
#include "smartpointers.h"
#include "unit.h"
#include "manager.h"

namespace unitlogic {
    using unit::Unit;
    using manager::Manager;
    using gameobject::id_t;

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
