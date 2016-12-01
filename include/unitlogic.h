#ifndef UNITLOGIC_H
#define UNITLOGIC_H

#include <vector>
#include "unit.h"
#include "smartpointers.h"

namespace manager {
	class Manager;
}

namespace unitlogic {
	using unit::Unit;
	using manager::Manager;
	typedef smartpointers::slave_ptr<Unit> unit_ptr;

	class UnitLogic {
		Manager* manager;
	public:
		inline UnitLogic(Manager* m) : manager(m) {};
		inline std::vector<unit_ptr> getUnits() const {return std::vector<unit_ptr>();}
		inline Unit* createUnit(int key) {return new Unit(key, NULL);}
		//inline master_ptr<GameObject> createUnit(int key) { return make_master<GameObject, Unit>(key); }
		inline Unit* removeUnit(Unit& unit) {return new Unit(0, NULL);} //placehodor
	};
}

#endif //UNITLOGIC_H