#include "../include/RenderUtils.h"
#include "../include/manager.h"
#include "../include/ResourceManager.h"

using graphics::RenderUtils;
using graphics::Colour;
using graphics::Texture;

manager::Manager* RenderUtils::manager = nullptr;
Texture* RenderUtils::health_bar_diffuse_mask = nullptr;
Texture* RenderUtils::health_bar_progress_mask = nullptr;
sf::Shader* RenderUtils::health_bar_mask_shader = nullptr;

void RenderUtils::init(manager::Manager *manager) {
	RenderUtils::manager = manager;
	RenderUtils::health_bar_progress_mask = manager->getResourceManager()->getTexture("health_bar_progress_mask");
	RenderUtils::health_bar_diffuse_mask = manager->getResourceManager()->getTexture("health_bar_diffuse_mask");
	RenderUtils::health_bar_mask_shader = manager->getResourceManager()->getShader("health_bar_mask_shader");
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