#ifndef BASE_H
#define BASE_H

#include "../gamecore.h"
#include "../tower.h"

namespace tower
{
	class Base : public Tower {
		graphics::Texture* texture = nullptr;
        std::map<unit::TYPE, graphics::AnimatedTexture*> exuhporoshun;
        std::vector<std::pair<graphics::AnimatedTexture*, std::pair<float, Point<int>>>> animations;
	public:
		Base(id_t key, Manager* m);
		void init() override;
		void render() override;
        void step() override;
        void attacked(gameobject_ptr aggressor) override;
	};
}

#endif // !BASE_H
