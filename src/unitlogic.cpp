#include "../include/unitlogic.h"
#include "../include/Units/BasicUnit.h"
#include "../include/Units/Wizard.h"
#include "../include/Units/Pirate.h"
#include "../include/manager.h"

namespace unitlogic
{
    UnitLogic::UnitLogic(Manager* m) :
        manager(m) {}

    std::vector<unit_ptr> UnitLogic::getUnits() const {
        return units;
    }

    //Remove invalid ptrs from the cache
    void UnitLogic::clean() {
        units.erase(std::remove_if(units.begin(), units.end(), [](unit_ptr &x)
        {
            return !(x.valid());
        }), units.end());
    }

    unit_ptr UnitLogic::createUnit(unit::TYPE type, float x, float y) const {
        return manager->createUnit(type, x, y);
    }

    //Factory method for unit construction
    Unit* UnitLogic::createUnit(id_t key, unit::TYPE type) const {
        switch(type)
        {
        case unit::TYPE::BASIC:
            return new unit::BasicUnit(key, manager);
        case unit::TYPE::WIZARD:
            return new unit::Wizard(key, manager);
		case unit::TYPE::PIRATE:
			return new unit::Pirate(key, manager);
        case unit::TYPE::BASE:
        default:
            std::cout << "FATAL ERROR! INCORRECT INSTANCE, nullptr RETURNED" << std::endl;
            return nullptr;
        }
    }

    //Share slave_ptrs with the unit they correspond to, this is only for convenience with other features
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
