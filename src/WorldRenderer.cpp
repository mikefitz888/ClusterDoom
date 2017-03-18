#include "../include/WorldRenderer.h"
#include "../include/AnimatedTexture.h"
#include "../include/VertexBuffer.h"
#include "../include/manager.h"
#include "../include/ResourceManager.h"
#include "../include/gamecontroller.h"

namespace worldrenderer {

    WorldRenderer::WorldRenderer(Manager* model) :
        manager(model) {}

    void WorldRenderer::init(){
        this->setDepth(1000);
        render_manager = manager->getRenderManager();

        /*game_bg_texture = new sf::Texture();
        if (!game_bg_texture->loadFromFile("src/Resources/Textures/background.png")) {
            std::cout << "[ERROR] Could not load texture! (Tower)" << std::endl;
        }*/
        
        /*graphics::Texture* game_bg_texture = nullptr;
        graphics::Texture* game_begin_texture = nullptr;
        graphics::Texture* game_win_texture = nullptr;
        graphics::Texture* game_lose_texture = nullptr;

        std::map<SCREEN, graphics::Texture*> textures;*/

        textures[SCREEN::NONE] = manager->getResourceManager()->getTexture("background");
        textures[SCREEN::GAME_START] = manager->getResourceManager()->getTexture("begin");
        textures[SCREEN::GAME_LOSE] = manager->getResourceManager()->getTexture("lose");
        textures[SCREEN::GAME_WIN] = manager->getResourceManager()->getTexture("win");


        /*game_bg_shader = render_manager->createShaderFromFile("src/Resources/Shaders/Render2D_vert.glsl", "src/Resources/Shaders/Render2D_frag.glsl");
        if (game_bg_shader == nullptr) {
            std::cout << "[ERROR] FAILED TO LOAD SHADER (Tower)" << std::endl;
        }*/
        game_bg_shader = manager->getResourceManager()->getShader("default");

        vbuff = new graphics::VertexBuffer();
        float width = (float) render_manager->getWindowWidth();
        float height = (float) render_manager->getWindowHeight();
        vbuff->addQuad(-width/2, -height/2, width/2, height/2);
        vbuff->freeze();

        // Create Aniamted texture
        //at = manager->getResourceManager()->getAnimatedTexture("explosion");

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
            default:
                display_screen = SCREEN::NONE;
        }

        renderGUI();
        if(!display_screen) manager->renderAll();
    }

    void WorldRenderer::renderGUI(){
        render_manager->setActiveShader(game_bg_shader);
        render_manager->setTexture(textures[display_screen]);
        float width = (float) render_manager->getWindowWidth();
        float height = (float) render_manager->getWindowHeight();
        render_manager->setActiveColour(graphics::Colour(255, 255, 255, 255));

        glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(width/2, height/2, 0.0));
        render_manager->setWorldMatrix(transform);

        vbuff->render();


        // Render debug explosion
       /* render_manager->setBlendModeAdditive();
        index += 0.60f;
        //at->render(index);
        render_manager->setBlendModeNormal();*/
    }

    void WorldRenderer::release(){
        render_manager = nullptr;
    }
}
