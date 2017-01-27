#ifndef _RENDER_UTILS_H
#define _RENDER_UTILS_H
#include "RenderManager.h"

namespace manager {
	class Manager;
}

namespace graphics {

	class RenderUtils {
		friend class RenderManager;
	private:
		static Manager *manager;
		static Texture *health_bar_progress_mask;
		static Texture *health_bar_diffuse_mask;
		static sf::Shader *health_bar_mask_shader;

		static void init(Manager *manager);

	public:
		static void render_circular_health(int x, int y, int health, int health_max, Colour colour);
		static Colour colour_blend(Colour a, Colour b, float factor);
	};



}

#endif