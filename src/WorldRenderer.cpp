#include "../include/WorldRenderer.h"
#include "../include/AnimatedTexture.h"
#include "../include/VertexBuffer.h"
#include "../include/manager.h"
#include "../include/ResourceManager.h"
#include "../include/gamecontroller.h"
#include "../include/network/Network.h"

namespace worldrenderer {

    WorldRenderer::WorldRenderer(Manager* model) :
        manager(model) {}

    void WorldRenderer::init(){
        this->setDepth(1000);
        render_manager = manager->getRenderManager();


        textures[SCREEN::NONE] = manager->getResourceManager()->getTexture("background");
        textures[SCREEN::GAME_START] = manager->getResourceManager()->getTexture("begin");
        textures[SCREEN::GAME_LOSE] = manager->getResourceManager()->getTexture("lose");
        textures[SCREEN::GAME_WIN] = manager->getResourceManager()->getTexture("win");
        textures[SCREEN::MAIN_MENU] = manager->getResourceManager()->getTexture("main_menu_bg");

        game_bg_shader = manager->getResourceManager()->getShader("default");

        // Fetch UI textures
        menu_ui_bars = manager->getResourceManager()->getTexture("main_menu_bars");

        // Font
        text = manager->getResourceManager()->createTextFromFont( "agency","Git gud");
    }

    void WorldRenderer::render(){
        auto game_state = manager->getGameController()->getGameState();

        //If any of these screens need animations just do it in renderGUI()
        switch (game_state) {
            case gamecontroller::GameState::LOSE:
                display_screen = SCREEN::GAME_LOSE;
                break;
            case gamecontroller::GameState::START:
                display_screen = SCREEN::GAME_START;
                break;
            case gamecontroller::GameState::WIN:
                display_screen = SCREEN::GAME_WIN;
                break;
            case gamecontroller::GameState::MAIN_MENU:
            case gamecontroller::GameState::MENU_LOBBY_SP:
            case gamecontroller::GameState::MENU_LOBBY_MP:
                display_screen = SCREEN::MAIN_MENU;
                break;
            default:
                display_screen = SCREEN::NONE;
        }

        // Render Background
        textures[SCREEN::NONE]->render(0, 0);

        float width = (float)render_manager->getWindowWidth();
        float height = (float)render_manager->getWindowHeight();
        float sf = width / 1920.0f;

        // If in-game, render obejcts
        if (!display_screen) {
            manager->renderAll();
        } else {

            // Render splashscreen
            render_manager->setActiveShader(game_bg_shader);
            textures[display_screen]->render((int)width/2, (int)height/2, (int)width, (int)height);
        }

        // Menu renders:
        manager->getRenderManager()->setRenderFlipped(true);
        switch (game_state) {

            case gamecontroller::GameState::MAIN_MENU: {
                // Render mode buttons
                sf::Vector2i mouse_pos = sf::Mouse::getPosition(*(manager->getRenderManager()->getWindow()));
                mouse_pos.x = width - mouse_pos.x;

                bool hoverbtn1 = false;
                bool hoverbtn2 = false;
                if (mouse_pos.x >= 244 * sf && mouse_pos.x <= 644 * sf && mouse_pos.y >= 577 * sf && mouse_pos.y <= 977 * sf) {
                    hoverbtn1 = true;
                }
                if (mouse_pos.x >= 1164 * sf && mouse_pos.x <= 1564 * sf && mouse_pos.y >= 577 * sf && mouse_pos.y <= 977 * sf) {
                    hoverbtn2 = true;
                }

                render_manager->setActiveColour(255, 255, 255, 255);
                if (hoverbtn1) {
                    render_manager->setActiveColour(255, 255, 255, 128);
                }
                manager->getResourceManager()->getTexture("btn_defenders_vs_ai")->render(244 * sf, 577 * sf, sf, sf, 0.0f);

                render_manager->setActiveColour(255, 255, 255, 255);
                if (hoverbtn2) {
                    render_manager->setActiveColour(255, 255, 255, 128);
                }
                manager->getResourceManager()->getTexture("btn_defenders_vs_att")->render(1164 * sf, 577 * sf, sf, sf, 0.0f);
                render_manager->setActiveColour(255, 255, 255, 255);
            } break;

            case gamecontroller::GameState::MENU_LOBBY_SP:

                // Render Single player Lobby
                menu_ui_bars->render((int)width / 2, (int)height / 2, (int)width, (int)height);
                manager->getResourceManager()->getTexture("defenders_vs_ai")->render((731.0f/1920.0f)*width, (513.0f/1080.0f)*height, sf, sf, 0.0f);
                manager->getResourceManager()->getTexture("ai_logo")->render((76.0f/1920.0f)*width, (678.0f/1080.0f)*height, sf, sf, 0.0f);

                break;
            case gamecontroller::GameState::MENU_LOBBY_MP:

                // Render Multiplayer lobby
                menu_ui_bars->render((int)width / 2, (int)height / 2, (int)width, (int)height);
                manager->getResourceManager()->getTexture("menu_attackers_v_defenders")->render((613.0f / 1920.0f)*width, (513.0f / 1080.0f)*height, sf, sf, 0.0f);
                int num_players = manager->getNetworkManager()->getNumConnections();
                float yy = 668.0f;
                float xx = 91.0f;
                for (int i = 0; i < num_players; i++) {
                    // Draw
                    manager->getResourceManager()->getTexture("phone_logo")->render(xx*sf, yy*sf, sf, sf, 0.0f);

                    // Adjust position
                    yy += 106.0f;
                    if (yy >= 1000.0f) {
                        yy = 668.0f;
                        xx += 555.0f;
                    }
                }
                break;
        }

        // CV connection status
        if (game_state == gamecontroller::GameState::MENU_LOBBY_SP || game_state == gamecontroller::GameState::MENU_LOBBY_MP) {
            if (this->manager->getGameController()->getCVReady()) {
                manager->getResourceManager()->getTexture("camera_connected")->render(1215.0f*sf, 784.0f*sf, sf, sf, 0.0f);
            } else {
                manager->getResourceManager()->getTexture("camera_disconnected")->render(1215.0f*sf, 784.0f*sf, sf, sf, 0.0f);
            }
        }
        manager->getRenderManager()->setRenderFlipped(false);
    }

    void WorldRenderer::renderGUI(){

    }

    void WorldRenderer::release(){
        render_manager = nullptr;
    }
}
