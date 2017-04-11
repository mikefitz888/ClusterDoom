#include "../../include/GameObjects/ResourceMine.h"
#include "../../include/manager.h"
#include "../../include/ResourceManager.h"
#include "../../include/gamecontroller.h"
#include "../../include/unit.h"
#include "../../include/tower.h"
#include "../../include/util.h"
#include "../../include/AnimatedTexture.h"
#include "../../include/smartpointers.h"


// Resource mine constructor
ResourceMine::ResourceMine(id_t id, manager::Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::RESOURCE_MINE, m) {
    this->setNetworkSync(true);
}

void ResourceMine::init() {
    this->mine_texture = this->manager->getResourceManager()->getTexture("resource_mine");
    this->sparkle_texture = this->manager->getResourceManager()->getTexture("sparkle");
    this->setDepth(5);
}

void ResourceMine::step() {
    // Create sparkles
    if (!sparkles_created) {
        for (int i = 0; i < 4; i++) {
            ResourceMineSparkle* sparkle = new ResourceMineSparkle(this->position, this->getSharedPtr());
            sparkles.push_back(sparkle);
        }
        sparkles_created = true;
    }

    // Simulate sparkles
    for (auto p : sparkles) {
        p->step();
    }
}

void ResourceMine::render() {
    this->mine_texture->render(this->getX(), this->getY());

    // Sparkles:
    this->manager->getRenderManager()->setBlendModeAdditive();
    std::cout << "RENMAN: " << this->manager->getRenderManager() << std::endl;
    for (auto p : sparkles) {
        float alpha = p->alpha;
        if (alpha > 0.0f) {
            this->manager->getRenderManager()->setActiveColour(graphics::Colour(0xFFFFFF, (int)(255.0f*alpha)));
            this->sparkle_texture->render(p->draw_position.x, p->draw_position.y, p->target_scale, p->target_scale, p->angle);
        }
    }
    this->manager->getRenderManager()->setActiveColour(graphics::Colour(0xFFFFFF, 255));
    this->manager->getRenderManager()->setBlendModeNormal();
}
void ResourceMine::renderGUI() {

}
void ResourceMine::release() {

    // Free sparkles
    for (auto p : sparkles) {
        delete p;
    }
}

// Resource mine particle effect
ResourceMineSparkle::ResourceMineSparkle(glm::vec2 source_pos, gameobject_ptr parent) {
    this->source_position = source_pos;
    this->parent = parent;
    this->angle = glm::linearRand(0.0f, 360.0f);
    
    timer_reset();
}

void ResourceMineSparkle::timer_reset() {
    if (visible == false) {
        timer = glm::linearRand(invisible_timer_min, invisible_timer_max);
    } else {
        timer = glm::linearRand(sparkle_timer_min, sparkle_timer_max);
    }
    position_modified = false;
}

void ResourceMineSparkle::step() {

    // Angle
    angle += 0.008f;

    // Timer
    timer--;
    if (timer <= 0) {
        visible = !visible;
        timer_reset();
    }

    // Alpha
    if (visible) {
        if (alpha < 1.0f) {
            alpha += 0.02f;
        }
    } else {
        if (alpha > 0.0f) {
            alpha -= 0.02f;
        } else {
            if (!position_modified) {
                target_scale = glm::linearRand(scale_min, scale_max);
                draw_position = source_position + glm::linearRand(position_offset_min, position_offset_max);
            }
        }
    }
    alpha = glm::clamp(alpha, 0.0f, 1.0f);
}
