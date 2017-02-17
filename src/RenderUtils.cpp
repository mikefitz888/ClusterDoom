#include "../include/RenderUtils.h"
#include "../include/manager.h"
#include "../include/ResourceManager.h"
#include "../include/VertexBuffer.h"
#include <glm/gtx/rotate_vector.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

using graphics::RenderUtils;
using graphics::Colour;
using graphics::Texture;

manager::Manager* RenderUtils::manager = nullptr;
Texture* RenderUtils::health_bar_diffuse_mask = nullptr;
Texture* RenderUtils::health_bar_progress_mask = nullptr;
graphics::Texture* RenderUtils::white = nullptr;
sf::Shader* RenderUtils::health_bar_mask_shader = nullptr;
VertexBuffer* RenderUtils::single_px_buffer = nullptr;

void RenderUtils::init(manager::Manager *manager) {
	RenderUtils::manager = manager;
	RenderUtils::health_bar_progress_mask = manager->getResourceManager()->getTexture("health_bar_progress_mask");
	RenderUtils::health_bar_diffuse_mask = manager->getResourceManager()->getTexture("health_bar_diffuse_mask");
	RenderUtils::health_bar_mask_shader = manager->getResourceManager()->getShader("health_bar_mask_shader");
    RenderUtils::white = manager->getResourceManager()->getTexture("white");

    RenderUtils::single_px_buffer = new VertexBuffer();
    RenderUtils::single_px_buffer->addQuad(-0.5f, -0.5f, 0.5f, 0.5f);
    RenderUtils::single_px_buffer->freeze();
}

void RenderUtils::render_circular_health(int x, int y, int health, int health_max, Colour colour) {
	RenderManager *rm = manager->getRenderManager();

	// Using a custom shader for the health bar to mask it based on the progress
	rm->setActiveShader(RenderUtils::health_bar_mask_shader);
	rm->setActiveColour(Colour(255, 255, 255, 255));
		
	// Set health uniform
	float health_progress = (float)health / (float)health_max;
	GLuint _uniform_progress_uid = glGetUniformLocation(RenderUtils::health_bar_mask_shader->getNativeHandle(), "healthProgress");
	glUniform1f(_uniform_progress_uid, health_progress);

	// Bind diffuse and mask textures
	rm->setTextureExt(RenderUtils::health_bar_diffuse_mask, 0, "texture_diffuse");
	rm->setTextureExt(RenderUtils::health_bar_progress_mask, 1, "texture_mask");

	// Draw mask
	rm->setActiveColour(colour);
	RenderUtils::health_bar_diffuse_mask->render(x, y);

	rm->resetActiveShader();
}

Colour RenderUtils::colour_blend(Colour a, Colour b, float factor) {
    Colour result(
        (unsigned char) (a.r*factor + b.r*(1.0f - factor)),
        (unsigned char) (a.g*factor + b.g*(1.0f - factor)),
        (unsigned char) (a.b*factor + b.b*(1.0f - factor)),
        (unsigned char) (a.a*factor + b.a*(1.0f - factor))
    );
    return result;
}

void RenderUtils::draw_line(int x1, int y1, int x2, int y2, int thickness, Colour colour) {
    //RenderManager *rm = manager->getRenderManager();
    float length = glm::length(vec2(x1, y1) - vec2(x2, y2));
    float angle  = (float)(atan2(y2- y1, x2 - x1));

   /* rm->setActiveColour(colour);
    rm->setWorldMatrixIdentity();
    glm::mat4 world_matrix = rm->getWorldMatrix();
    world_matrix = glm::translate(world_matrix, glm::vec3((x1 + x2)*0.5, (y1 + y2)*0.5, 10));
    world_matrix = glm::scale(world_matrix, glm::vec3(length, thickness,1));
    world_matrix = glm::scale(world_matrix, glm::vec3(1.0f/(float)white->getSize().x, 1.0f / (float)white->getSize().y, 1));
    world_matrix = glm::rotate(world_matrix, angle*180.0f / (float)M_PI, glm::vec3(0, 0, 1));
    rm->setWorldMatrix(world_matrix);
    white->render();*/
    //single_px_buffer->render();
    white->render(/*(int)(x1+x2)*0.5, (int)(y1 + y2)*0.5*/x1,y1, (float)length / (float)white->getSize().x, (float)thickness/(float)white->getSize().y, angle);
}