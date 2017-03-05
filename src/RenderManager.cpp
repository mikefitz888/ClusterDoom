#include "../include/RenderManager.h"
#include "../include/manager.h"
#include "../include/RenderUtils.h"
#include "../include/VertexBuffer.h"
#include "../include/ResourceManager.h"

namespace graphics {

    // Resource loading
    void RenderManager::loadResources() {
        ResourceManager *rm = this->manager->getResourceManager();

        // Load Textures
        rm->textureLoad("background", "src/Resources/Textures/grass/grass_template.jpg");
        rm->textureLoad("basic_tower", "src/Resources/Textures/chess_piece_rook.png")->setOriginCentre();
        rm->textureLoad("basic_unit", "src/Resources/Textures/pawn.png")->setOriginCentre();
        rm->textureLoad("red", "src/Resources/Textures/red.png")->setOriginCentre();
        rm->textureLoad("white", "src/Resources/Textures/white.png");// ->setOriginCentre();
        rm->textureLoad("base_image", "src/Resources/Textures/base.png")->setOriginCentre();
        rm->textureLoad("spawn", "src/Resources/Textures/Spawn.png");
        rm->textureLoad("health_bar_progress_mask", "src/Resources/Textures/UI/healthbar_mask.png")->setOriginCentre();
        rm->textureLoad("health_bar_diffuse_mask", "src/Resources/Textures/UI/healthbar_diffuse.png")->setOriginCentre();
        rm->textureLoad("bomb_texture", "src/Resources/Textures/bomb.png")->setOrigin(30, 36);

        // Load Animated Textures
        //rm->animatedTextureLoad("explosion", "src/Resources/Textures/explosion.png", true, 12, 4, -1);

        // Load Shaders
        rm->shaderLoad("default", "src/Resources/Shaders/Render2D_vert.glsl", "src/Resources/Shaders/Render2D_frag.glsl");
        rm->shaderLoad( "health_bar_mask_shader",
                        "src/Resources/Shaders/health_bar_mask_shd_vert.glsl",
                        "src/Resources/Shaders/health_bar_mask_shd_frag.glsl");
        // Load fonts
        rm->fontLoad("agency", "src/Resources/Fonts/AGENCYR.TTF");
    }

    GCamera::GCamera(RenderManager* render_manager_) :
        render_manager(render_manager_) {}

    GCameraOrtho::GCameraOrtho(int width_, int height_, RenderManager* render_manager_) :
        GCamera(render_manager_),
        width(width_),
        height(height_) {}

    // Init function
    void RenderManager::init(Manager *manager) {

        // Assign manager
        this->manager = manager;

        // Create window
        createWindow();
        GLenum err = glewInit();
        if(GLEW_OK != err){
            std::cout << glewGetErrorString(err) << std::endl;
        }

        // OpenGL states
        glClearColor(0.6f, 0.7f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glOrtho(0, 800, 600, 0, 1, -1);

        // Set identify matrix
        world_matrix = glm::mat4(1.0);

        // Create Camera
        camera = new GCameraOrtho(1280, 720, this);

        // Load resources
        this->loadResources();
        RenderUtils::init(this->manager);

        // Call active parent init
        if (render_parent != nullptr) {
            render_parent->init();
        }

    }

    void RenderManager::createWindow() {
        sf::ContextSettings ctx = sf::ContextSettings(32);
#ifdef __APPLE__
        ctx.majorVersion = 3;
        ctx.minorVersion = 3;
        printf("APPLE BOYSSS");
#elif __linux__
        //Leave default
#else
        ctx.majorVersion = 3;
        ctx.minorVersion = 1;
#endif
        ctx.attributeFlags = ctx.Core;
        window = new sf::Window(sf::VideoMode(width, height), "OpenGL", (fullscreen)? sf::Style::Fullscreen : sf::Style::Default, ctx);
        window->setVerticalSyncEnabled(false);
        window->setFramerateLimit(60);
    }

    bool RenderManager::render() {
        bool running = true;

        // Handle events
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                // end the program
                running = false;
            }
            else if (event.type == sf::Event::Resized) {
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        // Clear the current frame
        glClearColor(0.6f, 0.7f, 1.0f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set active shader and apply universal uniforms
        if (active_shader != nullptr) {
            // Also applies matrix and camera updates, so if the shader is changed mid-run, the default camera and world matrix are still ready to go
            setActiveShader(active_shader);
        }


        // Perform render of parent instance
        if (render_parent != nullptr) {
            render_parent->render();
        }

        // end the current frame (internally swaps the front and back buffers)
        window->display();

        return running;
    }

    /*
        Loads a shader from a file and performs the required openGL calls to prepare the shader for the current
        rendering environment.
    */
    sf::Shader* RenderManager::createShaderFromFile(const std::string &vertex_shader_filename, const std::string &fragment_shader_filename) {
        sf::Shader *sh = new sf::Shader();
        if (!sh->loadFromFile(vertex_shader_filename, fragment_shader_filename)) {
            return nullptr;
        }
        shaderPrepare(sh);
        return sh;
    }

    sf::Window* RenderManager::getWindow() {
        return this->window;
    }

    void RenderManager::shaderPrepare(sf::Shader *shader) {
        GLuint sh_id = shader->getNativeHandle();

        glBindAttribLocation(sh_id, 0, "in_Position");
        glBindAttribLocation(sh_id, 1, "in_Colour");
        glBindAttribLocation(sh_id, 2, "in_TextureCoord");
        glBindAttribLocation(sh_id, 3, "in_Normal");
    }

    // These functions can be used to manipulate the game window
    void RenderManager::setWindowFullScreen(bool fullscreen_) {
        fullscreen = !fullscreen_;
        window->close();
        delete window;
        createWindow();
    }

    void RenderManager::setWindowTitle(const sf::String title) const {
        window->setTitle(title);
    }

    void RenderManager::setWindowSize(int width_, int height_) {
        width  = width_;
        height = height_;
        window->setSize(sf::Vector2u(width, height));
    }

    int RenderManager::getWindowHeight() const {
        return height;
    }

    int RenderManager::getWindowWidth() const {
        return width;
    }



    // These functions manipulate the current render state
    void RenderManager::setActiveShader(sf::Shader *shd) {
        if (shd != nullptr) {
            if (shd != active_shader) {
                active_shader = shd;
                sf::Shader::bind(active_shader);
                bind_colour_uniform();
            }
            setWorldMatrix(world_matrix);
            camera->renderCamera();
        }
    }

    void RenderManager::resetActiveShader() {
        sf::Shader *shd = manager->getResourceManager()->getShader("default");
        setActiveShader(shd);
    }

    void RenderManager::setActiveColour(Colour c) {
        this->active_colour = c;
        bind_colour_uniform();
    }
    void RenderManager::setActiveColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        this->active_colour = Colour(r, g, b, a);
        bind_colour_uniform();
    }

    void RenderManager::bind_colour_uniform() {
        GLuint _uniform_colour_id = glGetUniformLocation(active_shader->getNativeHandle(), "drawColour");
        glUniform4f(_uniform_colour_id, (float)this->active_colour.r/255.0f,
                                        (float)this->active_colour.g/255.0f,
                                        (float)this->active_colour.b/255.0f,
                                        (float)this->active_colour.a/255.0f );
    }

    void RenderManager::setRenderParent(IRenderable *render_instance) {
        render_parent = render_instance;
    }

    void RenderManager::setTexture(sf::Texture *tex) {
        //active_shader->setUniform("texture_diffuse", tex);
        //sf::Texture::bind(tex);
        GLuint gl_tex_id = tex->getNativeHandle();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gl_tex_id);
    }

    void RenderManager::setTextureExt(sf::Texture *tex, GLuint texture_unit, GLchar* texture_uniform_name) {
        //active_shader->setUniform("texture_diffuse", tex);
        //sf::Texture::bind(tex);
        GLuint gl_tex_id = tex->getNativeHandle();
        switch (texture_unit) {
            case 0:glActiveTexture(GL_TEXTURE0); break;
            case 1:glActiveTexture(GL_TEXTURE1); break;
            case 2:glActiveTexture(GL_TEXTURE2); break;
            default:glActiveTexture(GL_TEXTURE0); break;
        }
        glBindTexture(GL_TEXTURE_2D, gl_tex_id);

        // Bind texture unit to uniform name
        GLuint tex_location = glGetUniformLocation(active_shader->getNativeHandle(), texture_uniform_name);
        glUniform1i(tex_location, texture_unit);
    }

    void RenderManager::release() const {
        if (render_parent != nullptr) {
            render_parent->release();
        }
        window->close();
        delete window;
    }

    float RenderManager::getAspect() const {
        return (float) (width / height);
    }

    void RenderManager::setViewProjection(glm::mat4 *vp_matrix) const {
        //active_shader->setUniform("matrixProjection", sf::Glsl::Mat4(glm::value_ptr(vp_matrix)));
        GLuint _mat_id = glGetUniformLocation(active_shader->getNativeHandle(), "matrixProjection");
        glUniformMatrix4fv(_mat_id, 1, GL_FALSE, glm::value_ptr(*vp_matrix));

        //vp_matrix = glm::mat4();
        /*std::cout << "MAT: " << std::endl;
        std::cout << (*vp_matrix)[0][0] << " " << (*vp_matrix)[0][1] << " " << (*vp_matrix)[0][2] << " " << (*vp_matrix)[0][3]  << std::endl;
        std::cout << (*vp_matrix)[1][0] << " " << (*vp_matrix)[1][1] << " " << (*vp_matrix)[1][2] << " " << (*vp_matrix)[1][3] << std::endl;
        std::cout << (*vp_matrix)[2][0] << " " << (*vp_matrix)[2][1] << " " << (*vp_matrix)[2][2] << " " << (*vp_matrix)[2][3] << std::endl;
        std::cout << (*vp_matrix)[3][0] << " " << (*vp_matrix)[3][1] << " " << (*vp_matrix)[3][2] << " " << (*vp_matrix)[3][3] << std::endl;*/
    }

    glm::mat4 RenderManager::getWorldMatrix() const {
        return world_matrix;
    }
    void RenderManager::setWorldMatrix(glm::mat4 world_matrix) const {
        world_matrix = world_matrix;
        if (active_shader != nullptr) {

            // Apply the matrix
            GLuint _mat_id = glGetUniformLocation(active_shader->getNativeHandle(), "matrixWorld");
            glUniformMatrix4fv(_mat_id, 1, GL_FALSE, glm::value_ptr(world_matrix));
        }
    }
    void RenderManager::setWorldMatrixIdentity() {
        setWorldMatrix(glm::mat4());
    }

    // Blend Modes
    void RenderManager::setBlendModeAdditive() {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    void RenderManager::setBlendModeNormal() {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /// ------------------ CAMERA FUNCTIONS -------------------- //
    void GCamera::setCameraPosition(float x, float y, float z) {
        this->camera_position.x = x;
        this->camera_position.y = y;
        this->camera_position.z = z;

    }

    void GCamera::setCameraDirection(float x, float y, float z){
        this->camera_direction.x = x;
        this->camera_direction.y = y;
        this->camera_direction.z = z;
    }

    glm::vec3 GCamera::getCameraPosition() const {
        return this->camera_position;
    }
    glm::vec3 GCamera::getCameraDirection() const {
        return this->camera_direction;
    }

    void GCamera::renderCamera() {
        this->projection_matrix = glm::perspective(this->camera_fov, render_manager->getAspect(), this->znear, this->zfar);
        this->view_matrix        = glm::lookAt(glm::vec3(0.0), -this->camera_direction, this->camera_up);
        this->view_matrix        = glm::translate(view_matrix, -camera_position);
        this->vp_matrix            = this->projection_matrix * this->view_matrix;

        render_manager->setViewProjection(&this->vp_matrix);
    }

    void GCameraOrtho::renderCamera() {
        this->projection_matrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1000.0f, 1000.0f); //glm::ortho<float>(0.0f, 0.0f, (float)width, (float)height, -0.1f, 100);
        this->view_matrix = glm::translate(glm::mat4(), -camera_position);
        this->vp_matrix = this->projection_matrix * this->view_matrix;
        //this->vp_matrix = this->projection_matrix;//glm::mat4();

        /*std::cout << "MATGG: " << std::endl;
        std::cout << (projection_matrix)[0][0] << " " << (projection_matrix)[0][1] << " " << (projection_matrix)[0][2] << " " << (projection_matrix)[0][3] << std::endl;
        std::cout << (projection_matrix)[1][0] << " " << (projection_matrix)[1][1] << " " << (projection_matrix)[1][2] << " " << (projection_matrix)[1][3] << std::endl;
        std::cout << (projection_matrix)[2][0] << " " << (projection_matrix)[2][1] << " " << (projection_matrix)[2][2] << " " << (projection_matrix)[2][3] << std::endl;
        std::cout << (projection_matrix)[3][0] << " " << (projection_matrix)[3][1] << " " << (projection_matrix)[3][2] << " " << (projection_matrix)[3][3] << std::endl;*/

        render_manager->setViewProjection(&this->vp_matrix);
    }

    Colour::Colour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    Colour::Colour(int colour, unsigned char alpha) {
        this->a = alpha;
        this->r = (colour >> 16) & 255;
        this->g = (colour >> 8) & 255;
        this->b = colour&255;
    }
    Colour::Colour(int colour) {
        Colour(colour, 255);
    }

    /*GCameraOrtho::GCameraOrtho(int width, int height) {
        this->width = width;
        this->height = height;
    }*/

    ////////////////////////////////////////////////////
    // TEXTURE METHODS
    graphics::Texture::Texture(RenderManager *render_manager) : sf::Texture() {
        this->render_manager = render_manager;
    }

    void graphics::Texture::setOrigin(int x, int y) {
        this->origin_x = x;
        this->origin_y = y;
    }

    void graphics::Texture::setOriginCentre() {
        setOrigin(getSize().x / 2, getSize().y / 2);
    }

    void graphics::Texture::createVertexBuffer() {
        // Create vertex buffer
        this->texture_quad = new VertexBuffer();
        this->texture_quad->addQuad(0.0f, 0.0f, (float) this->getSize().x, (float) this->getSize().y);
        this->texture_quad->freeze();
    }

    void graphics::Texture::render() {
        GLuint gl_tex_id = this->getNativeHandle();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gl_tex_id);
        this->texture_quad->render();
    }

    void graphics::Texture::render(int x, int y, float rotation) {
        glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(x, y, 0.0));
        transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f) );
        transform = glm::translate(transform, glm::vec3(-origin_x, -origin_y, 0.0f));

        this->render_manager->setWorldMatrix(transform);
        this->render();
    }

    void graphics::Texture::render(int x, int y, float xscale, float yscale, float rotation) {
        glm::mat4 transform = glm::mat4();
        transform = glm::translate(transform, glm::vec3(x, y, 0.0));
        
        transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(xscale, yscale, 1.0));
        transform = glm::translate(transform, glm::vec3(-origin_x, -origin_y, 0.0f));

        this->render_manager->setWorldMatrix(transform);
        this->render();
    }

    void graphics::Texture::render(int x, int y, int width, int height, float rotation) {
        float xscale = (float)width / (float)this->getSize().x;
        float yscale = (float)height / (float)this->getSize().y;
        glm::mat4 transform = glm::mat4();
        transform = glm::translate(transform, glm::vec3(x, y, 0.0));
        
        transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(xscale, yscale, 1.0));
        transform = glm::translate(transform, glm::vec3(-origin_x, -origin_y, 0.0f));

        this->render_manager->setWorldMatrix(transform);
        this->render();
    }


    bool graphics::Texture::loadFromFile(sf::String resource_filepath) {
        bool result = this->sf::Texture::loadFromFile(resource_filepath);
        if (result) {
            this->createVertexBuffer();
        }
        return result;
    }

}
