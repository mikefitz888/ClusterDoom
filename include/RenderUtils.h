#ifndef _RENDER_UTILS_H
#define _RENDER_UTILS_H
#include "RenderManager.h"

/*
    RenderUtils
    -----------

    Class for a collection of general-purpose rendering utility functions that can be re-used by multiple components of the game.
*/

namespace graphics {

	class RenderUtils {
		friend class RenderManager;
	private:
		static Manager *manager;
		static Texture *health_bar_progress_mask;
		static Texture *health_bar_diffuse_mask;
        static graphics::Texture *white;
		static sf::Shader *health_bar_mask_shader;
        static VertexBuffer *single_px_buffer;

		static void init(Manager *manager);

	public:
		static void render_circular_health(int x, int y, int health, int health_max, Colour colour, float scale=1.0f);
		static Colour colour_blend(Colour a, Colour b, float factor);
        static void draw_line(int x1, int y1, int x2, int y2, int thickness, Colour colour);
	};



}

#endif
