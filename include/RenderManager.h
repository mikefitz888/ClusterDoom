#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include "gamecore.h"

/*
    GCamera maintains the current matrix transformation state
*/

namespace graphics {
    using manager::Manager;
    class GCamera {
    protected:
        glm::vec3 camera_position;
        glm::vec3 camera_direction;
        glm::vec3 camera_up;
        float camera_fov = 80, znear = 1, zfar = 1000;

        glm::mat4 view_matrix;
        glm::mat4 projection_matrix;
        glm::mat4 vp_matrix;
        RenderManager *render_manager;
        GCamera(RenderManager* render_manager_);

    public:
        void setCameraPosition(float x, float y, float z);
        void setCameraDirection(float x, float y, float z);
        virtual void renderCamera();
        glm::vec3 getCameraPosition() const;
        glm::vec3 getCameraDirection() const;
    };

    class GCameraOrtho : public GCamera {
    private:
        int width, height;

    public:
        GCameraOrtho(int width_, int height_, RenderManager *render_manager_);
        virtual void renderCamera() override;
    };

    struct Colour{
        Colour(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        Colour(int colour, unsigned char alpha);
        Colour(int colour);
        unsigned char r, g, b, a;
    };
    /*
        IRenderable is an interface defining the core functions required for something that
        can be rendered.
    */
    class IRenderable {

        int depth = 0; // Depth is used to control the render order of objects. Objects with a higher depth will be rendered first. (The objects with lower depths, last).

        public:
            virtual void init() = 0;            // Function called once openGL context is guaranteed to exist. Useful for loading resources/initialises mesh's/shaders.
            virtual void renderBegin();
            virtual void render() = 0;        // Function called during main render loop to allow an object to perform its rendering
            virtual void renderGUI() = 0;        // Function called after main render loop with an orthographic context that match the window dimensions
            virtual void release() = 0;        // Called when the openGL context is destroyed
            void setDepth(int depth);
            int  getDepth();
    };

    /*
        RenderManager is responsible for coordinating all rendering features
        of the game. It maintains an internal Render master which it will invoke a render
        function upon.
    */
    class RenderManager {

    //protected:

        //static RenderManager render_manager;

    public:

        void init(Manager *manager);
        void setWindowFullScreen(bool fullscreen);
        void setWindowTitle(const sf::String title) const;
        void setWindowSize(int width, int height);
        void clear();
        int getWindowWidth() const;
        int getWindowHeight() const;
        bool render();
        void setActiveShader(sf::Shader *shd);
        void resetActiveShader();
        void setTexture(sf::Texture *tex);
        void setTextureExt(sf::Texture *tex, GLuint texture_unit, GLchar* texture_uniform_name);
        void setRenderParent(IRenderable *render_instance);
        void setActiveColour(Colour c);
        void setActiveColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        sf::Shader* createShaderFromFile(const std::string &vertex_shader_filename, const std::string &fragment_shader_filename);
        void release() const;
        float getAspect() const;
        void setViewProjection(glm::mat4 *vp_matrix) const;

        glm::mat4 getWorldMatrix() const;
        void setWorldMatrix(glm::mat4 world_matrix) const;
        void setWorldMatrixIdentity();

        void setBlendModeAdditive();
        void setBlendModeNormal();

        bool getRenderFlipped();
        void setRenderFlipped(bool flipped);

        sf::Window* getWindow();


    private:

        GCamera *camera;
        int width = 1280, height = 720;
		bool fullscreen = false;
        Colour active_colour = Colour(255, 255, 255, 255);
        sf::Window *window = nullptr;
        sf::Shader *active_shader = nullptr;        /* Whilst shaders can be changed, it needs to be done through the RenderManager
                                        so that binding textures/other graphics resources know which shader uniforms to modify */
        IRenderable *render_parent = nullptr;        /* The top-most node in the composite rendering pattern (This should render any children in its own render method)*/
        Manager *manager;

        // Active texture
        sf::Texture *active_texture[4];

        // Transformation + Rendering
        glm::mat4 world_matrix;            /* Matrix used to represent the current transformation state */
        bool flipped = false;
        bool prev_flipped = false;

        // Methods:
        void createWindow();
        static void shaderPrepare(sf::Shader *shader);    /* Performs any openGL operations upon a shader needed to configure it for the rendering environment*/
        void bind_colour_uniform();
        void loadResources(); // A location in which initial, core resources can be loaded in.
    };

    class Texture : public sf::Texture {
        friend class RenderManager;
    private:
        VertexBuffer  *texture_quad;
        RenderManager *render_manager;
        void createVertexBuffer();
        int origin_x, origin_y;

        void setOrigin(int x, int y);
        void setOriginCentre();
    public:
        Texture(RenderManager *render_manager);
        bool loadFromFile(sf::String filepath);
        void render();
        void render(int x, int y, float rotation = 0, float z = 0.0f);
        void render(int x, int y, float xscale, float yscale, float rotation = 0, float z = 0.0f);
        void render(int x, int y, int width, int height, float rotation = 0, float z = 0.0f);

    };
}

#endif
