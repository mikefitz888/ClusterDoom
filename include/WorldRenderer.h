#ifndef WORLDRENDERER_H
#define WORLDRENDERER_H

#include "RenderManager.h"
#include "manager.h"
#include "towerlogic.h"
#include "ResourceManager.h"
//#include "AnimatedTexture.h"

namespace graphics {
	class AnimatedTexture;
}

namespace worldrenderer {
	using graphics::IRenderable;
	using graphics::RenderManager;
	using manager::Manager;
	class WorldRenderer : public IRenderable {
		Manager* manager = nullptr;
		RenderManager* render_manager = nullptr;

		sf::Texture* game_bg_texture = nullptr;
		sf::Shader* game_bg_shader = nullptr;
		graphics::VertexBuffer* vbuff = nullptr;

		/// TEMP
		graphics::AnimatedTexture *at = nullptr;
		float index = 0;
		///

	public:
		inline WorldRenderer(Manager* model) : manager(model) {}
		void init() override;
		void render() override;
		void renderGUI() override;
		void release() override;
	};
}

#endif//WORLDRENDERER_H