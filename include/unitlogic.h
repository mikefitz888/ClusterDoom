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
		std::vector<unit_ptr> getUnits() const;
		Unit* createUnit(int key);
		Unit* removeUnit(Unit& unit);
		void step();
		void draw();
	private:
	};
}

#endif //UNITLOGIC_H