#ifndef _SCENARIO_HEADER
#define _SCENARIO_HEADER

#include "gamecore.h"
#include <queue>
#include <list>

namespace scenario {
	class Scenario {
	private:
		std::priority_queue<int> point;

	public:
		Scenario();
		void getTimestep(int timestep, std::list<int> *events);
	};

}
#endif