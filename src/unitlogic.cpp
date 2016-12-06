#include "unitlogic.h"

namespace unitlogic {
	unit_ptr UnitLogic::createUnit(unit::TYPE type) const {
		return manager->createUnit(type);
	}

	Unit* UnitLogic::createUnit(id_t key, unit::TYPE type) const {
		return new Unit(key, manager);
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
