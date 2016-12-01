#ifndef WORLDRENDERER_H
#define WORLDRENDERER_H

#include "RenderManager.h"
#include "manager.h"
#include "towerlogic.h"

namespace manager {
	class Manager;
}

namespace worldrenderer {
	using graphics::IRenderable;
	using graphics::RenderManager;
	using manager::Manager;
	class WorldRenderer : public IRenderable {
		Manager* manager = NULL;
		RenderManager* render_manager = NULL;
	public:
		inline WorldRenderer(Manager* model) : manager(model) {}
		void init() override;
		void render() override;
		void renderGUI() override;
		void release() override;
	};
}

#endif//WORLDRENDERER_H