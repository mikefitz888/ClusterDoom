#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

/*
	GCamera maintains the current matrix transformation state 
*/

namespace graphics {

	class GCamera {

		protected:
			glm::vec3 camera_position;
			glm::vec3 camera_direction;
			glm::vec3 camera_up;
			float camera_fov = 80, znear = 1, zfar = 1000;

			glm::mat4 view_matrix;
			glm::mat4 projection_matrix;
			glm::mat4 vp_matrix;



		public:
			void setCameraPosition(float x, float y, float z);
			void setCameraDirection(float x, float y, float z);
			virtual void renderCamera();
			glm::vec3 getCameraPosition();
			glm::vec3 getCameraDirection();
		};

		class GCameraOrtho : public GCamera {
		private:
			int width, height;

		public:
			GCameraOrtho(int width, int height);
			virtual void renderCamera();
	};

	/*
		IRenderable is an interface defining the core functions required for something that
		can be rendered.
	*/
	class IRenderable {
	
		public:
			virtual void init() = 0;			// Function called once openGL context is guaranteed to exist. Useful for loading resources/initialises mesh's/shaders. 
			virtual void render() = 0;		// Function called during main render loop to allow an object to perform its rendering
			virtual void renderGUI() = 0;		// Function called after main render loop with an orthographic context that match the window dimensions
			virtual void release() = 0;		// Called when the openGL context is destroyed
	};

	/*
		RenderManager is responsible for coordinating all rendering features
		of the game. It maintains an internal Render master which it will invoke a render
		function upon.
	*/
	class RenderManager {

		protected:

			static RenderManager render_manager;

		public:

			static void init();
			static void setWindowFullScreen(bool fullscreen);
			static void setWindowTitle(const sf::String title);
			static void setWindowSize(int width, int height);
			static bool render();
			static void setActiveShader(sf::Shader *shd);
			static void setTexture(sf::Texture *tex);
			static void setTextureExt(sf::Texture *tex, GLuint texture_unit);
			static void setRenderParent(IRenderable *render_instance);
			static sf::Shader* createShaderFromFile(const std::string &vertex_shader_filename, const std::string &fragment_shader_filename);
			static void release();
			static float getAspect();
			static void setViewProjection(glm::mat4 *vp_matrix);

			static glm::mat4 getWorldMatrix();
			static void setWorldMatrix(glm::mat4 world_matrix);
			static void setWorldMatrixIdentity();


		private:

			GCamera *camera;
			int width = 1280, height = 720;
			bool fullscreen = false;
			sf::Window *window;
			sf::Shader *active_shader;		/* Whilst shaders can be changed, it needs to be done through the RenderManager
											so that binding textures/other graphics resources know which shader uniforms to modify */
			IRenderable *render_parent;		/* The top-most node in the composite rendering pattern (This should render any children in its own render method)*/

			// Transformation + Rendering
			glm::mat4 world_matrix;			/* Matrix used to represent the current transformation state */

			// Methods:
			void createWindow();
			static void shaderPrepare(sf::Shader *shader);	/* Performs any openGL operations upon a shader needed to configure it for the rendering environment*/
	};
}

#endif