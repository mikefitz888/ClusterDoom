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
		inline std::vector<unit_ptr> getUnits() const { return units; }
		Unit* createUnit(id_t key) const;
		unit_ptr createUnit() const;
		void removeUnit(unit_ptr unit);
		
		void giveSlavePtr(unit_ptr unit);
	};
}

#endif //UNITLOGIC_H