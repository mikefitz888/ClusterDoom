#include "../include/WorldRenderer.h"
#include "../include/AnimatedTexture.h"
#include "../include/VertexBuffer.h"
#include "../include/manager.h"
#include "../include/ResourceManager.h"

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
        game_bg_texture = manager->getResourceManager()->getTexture("background");


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
        renderGUI();

        manager->renderAll();
    }

    void WorldRenderer::renderGUI(){
        render_manager->setActiveShader(game_bg_shader);
        render_manager->setTexture(game_bg_texture);
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
