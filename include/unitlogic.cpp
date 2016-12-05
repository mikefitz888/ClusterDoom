#include "unitlogic.h"

namespace unitlogic {
	unit_ptr UnitLogic::createUnit(unit::TYPE type) const {
		return manager->createUnit(type);
	}

	Unit* UnitLogic::createUnit(id_t key, unit::TYPE type) const {
		return new Unit(key, manager);
	}

	void UnitLogic::giveSlavePtr(unit_ptr unit) {
		units.push_back(unit);
		unit->setX(490);
		unit->setY(200);
		//printf("Recieved slave pointer: %d\n", 0);
		std::cout << "got: -" << units.size() << "- units" << std::endl;
	}

	void UnitLogic::removeUnit(unit_ptr unit) {

	}
}
