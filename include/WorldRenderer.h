#ifndef WORLDRENDERER_H
#define WORLDRENDERER_H

#include "gamecore.h"

#include "RenderManager.h"

namespace worldrenderer {
    using graphics::IRenderable;
    using graphics::RenderManager;
    using manager::Manager;
    using towerlogic::tower_ptr;

    class WorldRenderer : public IRenderable {
        Manager* manager = nullptr;
        RenderManager* render_manager = nullptr;

        graphics::Texture* game_bg_texture = nullptr;
        sf::Shader* game_bg_shader = nullptr;
        graphics::VertexBuffer* vbuff = nullptr;
        sf::Text* text = nullptr;

        /// TEMP
        graphics::AnimatedTexture *at = nullptr;
        float index = 0;
        ///

    public:
        WorldRenderer(Manager* model);
        void init() override;
        void render() override;
        void renderGUI() override;
        void release() override;
    };
}

#endif//WORLDRENDERER_H
