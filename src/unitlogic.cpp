#include "../include/unitlogic.h"
#include "../include/Units/BasicUnit.h"

namespace unitlogic
{
    UnitLogic::UnitLogic(Manager* m) :
        manager(m) {}

    std::vector<unit_ptr> UnitLogic::getUnits() const {
        return units;
    }

    void UnitLogic::clean() {
        units.erase(std::remove_if(units.begin(), units.end(), [](unit_ptr &x)
        {
            return !(x.valid());
        }), units.end());
    }

    unit_ptr UnitLogic::createUnit(unit::TYPE type) const {
        return manager->createUnit(type);
    }

    Unit* UnitLogic::createUnit(id_t key, unit::TYPE type) const {
        switch(type)
        {
        case unit::TYPE::BASIC:
            return new unit::BasicUnit(key, manager);
        case unit::TYPE::BASE:
        default:
            std::cout << "FATAL ERROR! INCORRECT INSTANCE, nullptr RETURNED" << std::endl;
            return nullptr;
        }
    }

    void UnitLogic::giveSlavePtr(unit_ptr unitptr) {
        //Look for free place in units pool
        for (auto unit = units.begin(); unit != units.end(); ++unit) {
            if (!(*unit)) {
                units.emplace(unit, unitptr);
                return;
            }
        }
        units.push_back(unitptr);
    }

    void UnitLogic::removeUnit(unit_ptr unit) {

    }
}
