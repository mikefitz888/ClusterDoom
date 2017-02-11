#ifndef BASE_H
#define BASE_H

#include "../gamecore.h"
#include "../tower.h"

namespace tower
{
	class Base : public Tower {
		graphics::Texture* texture = nullptr;
	public:
		Base(id_t key, Manager* m);
		void init() override;
		void render() override;
	};
}

#endif // !BASE_H
