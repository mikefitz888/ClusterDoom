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
        graphics::Texture* game_begin_texture = nullptr;
        graphics::Texture* game_win_texture = nullptr;
        graphics::Texture* game_lose_texture = nullptr;



        sf::Shader* game_bg_shader = nullptr;
        graphics::VertexBuffer* vbuff = nullptr;
        sf::Text* text = nullptr;

        /// TEMP
        graphics::AnimatedTexture *at = nullptr;
        float index = 0;
        
        // Game Screens
        enum SCREEN : unsigned int {NONE = 0, GAME_START, GAME_LOSE, GAME_WIN};
        SCREEN display_screen = SCREEN::GAME_START;
        std::map<SCREEN, graphics::Texture*> textures;

    public:
        WorldRenderer(Manager* model);
        void init() override;
        void render() override;
        void renderGUI() override;
        void release() override;
    };
}

#endif//WORLDRENDERER_H
