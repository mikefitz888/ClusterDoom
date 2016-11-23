#ifndef UNIT_H
#define UNIT_H

#include "gameobject.h"

namespace unit {
	using gameobject::GameObject;
	class Unit : public GameObject {
		Unit();
	};
}

#endif //UNIT_H