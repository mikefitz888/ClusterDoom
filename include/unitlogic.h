#ifndef UNITLOGIC_H
#define UNITLOGIC_H

#include <vector>
#include "unit.h"
#include "smartpointers.h"

namespace unitlogic {
	using unit::Unit;
	typedef smartpointers::slave_ptr<Unit> unit_ptr;

	class UnitLogic {
	public:
		inline std::vector<unit_ptr> getUnits() const {return std::vector<unit_ptr>();}
		inline Unit* createUnit(int key) {return new Unit(key);}
		//inline master_ptr<GameObject> createUnit(int key) { return make_master<GameObject, Unit>(key); }
		inline Unit* removeUnit(Unit& unit) {return new Unit(0);} //placehodor
	private:
	};
}

#endif //UNITLOGIC_H