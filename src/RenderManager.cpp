#include "../include/RenderManager.h"
#include <iostream>


namespace graphics {
	// Create singleton
	RenderManager RenderManager::render_manager;

	// Init function
	void RenderManager::init() {
		// Create window
		RenderManager::render_manager.createWindow();
		glewInit();

		// OpenGL states
		glClearColor(0.6f, 0.7f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glOrtho(0, 800, 600, 0, 1, -1);

		// Set identify matrix
		RenderManager::render_manager.world_matrix = glm::mat4(1.0);

		// Create Camera
		RenderManager::render_manager.camera = new GCameraOrtho(1280, 720);

		// Call active parent init
		if (RenderManager::render_manager.render_parent != NULL) {
			RenderManager::render_manager.render_parent->init();
		}
	}

	void RenderManager::createWindow() {
		window = new sf::Window(sf::VideoMode(width, height), "OpenGL", (fullscreen)? sf::Style::Fullscreen : sf::Style::Default, sf::ContextSettings(32));
		window->setVerticalSyncEnabled(false);
		window->setFramerateLimit(60);
	}

	bool RenderManager::render() {
		bool running = true;

		// Handle events
		sf::Event event;
		while (RenderManager::render_manager.window->pollEvent(event)) {
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.6f, 0.7f, 1.0f, 1.0f);
		glClearDepth(1.0f);

		// Set active shader and apply universal uniforms
		if (RenderManager::render_manager.active_shader != NULL) {
			// Also applies matrix and camera updates, so if the shader is changed mid-run, the default camera and world matrix are still ready to go
			setActiveShader(RenderManager::render_manager.active_shader);
		}


		// Perform render of parent instance
		if (RenderManager::render_manager.render_parent != NULL) {
			RenderManager::render_manager.render_parent->render();
		}

		// end the current frame (internally swaps the front and back buffers)
		RenderManager::render_manager.window->display();

		return running;
	}

	/*
		Loads a shader from a file and performs the required openGL calls to prepare the shader for the current
		rendering environment.
	*/
	sf::Shader* RenderManager::createShaderFromFile(const std::string &vertex_shader_filename, const std::string &fragment_shader_filename) {
		sf::Shader *sh = new sf::Shader();
		if (!sh->loadFromFile(vertex_shader_filename, fragment_shader_filename)) {
			return NULL;
		}
		shaderPrepare(sh);
		return sh;
	}

	void RenderManager::shaderPrepare(sf::Shader *shader) {
		GLint sh_id = shader->getNativeHandle();

		glBindAttribLocation(sh_id, 0, "in_Position");
		glBindAttribLocation(sh_id, 1, "in_Colour");
		glBindAttribLocation(sh_id, 2, "in_TextureCoord");
		glBindAttribLocation(sh_id, 3, "in_Normal");
	}

	// These functions can be used to manipulate the game window
	void RenderManager::setWindowFullScreen(bool fullscreen) {
		fullscreen = !fullscreen;
		RenderManager::render_manager.window->close();
		delete RenderManager::render_manager.window;
		RenderManager::render_manager.createWindow();
	}

	void RenderManager::setWindowTitle(const sf::String title) {
		RenderManager::render_manager.window->setTitle(title);
	}

	void RenderManager::setWindowSize(int width, int height) {
		RenderManager::render_manager.width  = width;
		RenderManager::render_manager.height = height;
		RenderManager::render_manager.window->setSize(sf::Vector2u(width, height));
	}

	// These functions manipulate the current render state
	void RenderManager::setActiveShader(sf::Shader *shd) {
		if (shd != NULL) {
			if (shd != RenderManager::render_manager.active_shader) {
				RenderManager::render_manager.active_shader = shd;
				sf::Shader::bind(RenderManager::render_manager.active_shader);
			}
			setWorldMatrix(RenderManager::render_manager.world_matrix);
			RenderManager::render_manager.camera->renderCamera();
		}
	}

	void RenderManager::setRenderParent(IRenderable *render_instance) {
		RenderManager::render_manager.render_parent = render_instance;
	}

	void RenderManager::setTexture(sf::Texture *tex) {
		//active_shader->setUniform("texture_diffuse", tex);
		//sf::Texture::bind(tex);
		GLuint gl_tex_id = tex->getNativeHandle();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gl_tex_id);
	}

	void RenderManager::setTextureExt(sf::Texture *tex, GLuint texture_unit) {
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
	}

	void RenderManager::release() {
		if (RenderManager::render_manager.render_parent != NULL) {
			RenderManager::render_manager.render_parent->release();
		}
		RenderManager::render_manager.window->close();
		delete RenderManager::render_manager.window;
		delete &RenderManager::render_manager;
	}

	float RenderManager::getAspect() {
		return RenderManager::render_manager.width / RenderManager::render_manager.height;
	}

	void RenderManager::setViewProjection(glm::mat4 *vp_matrix) {
		//active_shader->setUniform("matrixProjection", sf::Glsl::Mat4(glm::value_ptr(vp_matrix)));
		GLuint _mat_id = glGetUniformLocation(RenderManager::render_manager.active_shader->getNativeHandle(), "matrixProjection");
		glUniformMatrix4fv(_mat_id, 1, GL_FALSE, glm::value_ptr(*vp_matrix));
	
		//vp_matrix = glm::mat4();
		/*std::cout << "MAT: " << std::endl;
		std::cout << (*vp_matrix)[0][0] << " " << (*vp_matrix)[0][1] << " " << (*vp_matrix)[0][2] << " " << (*vp_matrix)[0][3]  << std::endl;
		std::cout << (*vp_matrix)[1][0] << " " << (*vp_matrix)[1][1] << " " << (*vp_matrix)[1][2] << " " << (*vp_matrix)[1][3] << std::endl;
		std::cout << (*vp_matrix)[2][0] << " " << (*vp_matrix)[2][1] << " " << (*vp_matrix)[2][2] << " " << (*vp_matrix)[2][3] << std::endl;
		std::cout << (*vp_matrix)[3][0] << " " << (*vp_matrix)[3][1] << " " << (*vp_matrix)[3][2] << " " << (*vp_matrix)[3][3] << std::endl;*/
	}

	glm::mat4 RenderManager::getWorldMatrix() {
		return RenderManager::render_manager.world_matrix;
	}
	void RenderManager::setWorldMatrix(glm::mat4 world_matrix) {
		RenderManager::render_manager.world_matrix = world_matrix;
		if (RenderManager::render_manager.active_shader != NULL) {

			// Apply the matrix 
			GLuint _mat_id = glGetUniformLocation(RenderManager::render_manager.active_shader->getNativeHandle(), "matrixWorld");
			glUniformMatrix4fv(_mat_id, 1, GL_FALSE, glm::value_ptr(RenderManager::render_manager.world_matrix));
		}
	}
	void RenderManager::setWorldMatrixIdentity() {
		setWorldMatrix(glm::mat4());
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

	glm::vec3 GCamera::getCameraPosition() {
		return this->camera_position;
	}
	glm::vec3 GCamera::getCameraDirection() {
		return this->camera_direction;
	}

	void GCamera::renderCamera() {
		this->projection_matrix = glm::perspective(this->camera_fov, RenderManager::getAspect(), this->znear, this->zfar);
		this->view_matrix		= glm::lookAt(glm::vec3(0.0), -this->camera_direction, this->camera_up);
		this->view_matrix		= glm::translate(view_matrix, -camera_position);
		this->vp_matrix			= this->projection_matrix * this->view_matrix;

		RenderManager::setViewProjection(&this->vp_matrix);
	}

	void GCameraOrtho::renderCamera() {
		this->projection_matrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f); //glm::ortho<float>(0.0f, 0.0f, (float)width, (float)height, -0.1f, 100);
		this->view_matrix = glm::translate(glm::mat4(), -camera_position);
		this->vp_matrix = this->projection_matrix * this->view_matrix;
		//this->vp_matrix = this->projection_matrix;//glm::mat4();

		/*std::cout << "MATGG: " << std::endl;
		std::cout << (projection_matrix)[0][0] << " " << (projection_matrix)[0][1] << " " << (projection_matrix)[0][2] << " " << (projection_matrix)[0][3] << std::endl;
		std::cout << (projection_matrix)[1][0] << " " << (projection_matrix)[1][1] << " " << (projection_matrix)[1][2] << " " << (projection_matrix)[1][3] << std::endl;
		std::cout << (projection_matrix)[2][0] << " " << (projection_matrix)[2][1] << " " << (projection_matrix)[2][2] << " " << (projection_matrix)[2][3] << std::endl;
		std::cout << (projection_matrix)[3][0] << " " << (projection_matrix)[3][1] << " " << (projection_matrix)[3][2] << " " << (projection_matrix)[3][3] << std::endl;*/

		RenderManager::setViewProjection(&this->vp_matrix);
	}

	GCameraOrtho::GCameraOrtho(int width, int height) {
		this->width = width;
		this->height = height;
	}
}

