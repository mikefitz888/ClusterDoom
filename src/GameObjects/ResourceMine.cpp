#include "../../include/GameObjects/ResourceMine.h"
#include "../../include/manager.h"
#include "../../include/ResourceManager.h"
#include "../../include/gamecontroller.h"
#include "../../include/unit.h"
#include "../../include/tower.h"
#include "../../include/util.h"
#include "../../include/AnimatedTexture.h"


// Resource mine constructor
ResourceMine::ResourceMine(id_t id, manager::Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::RESOURCE_MINE, m) {
    this->setNetworkSync(true);
    this->setDepth(-10);
}

void ResourceMine::init() {
    this->mine_texture = this->manager->getResourceManager()->getTexture("resource_mine");
    this->sparkle_texture = this->manager->getResourceManager()->getTexture("sparkle");
    this->mining_effect_texture = this->manager->getResourceManager()->getTexture("mining_beam_segment");
    this->mining_effect_section_end = this->manager->getResourceManager()->getTexture("mining_beam_ball");
    this->setDepth(5);

    // Set resource
    this->starting_resource = glm::linearRand(min_available_resource, max_available_resource);
    this->current_resource = this->starting_resource;
}

void ResourceMine::step() {

    if (current_resource > 0) {
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

        // Check for nearby towers:
        tower_gather.clear();
        auto towers = this->manager->getGameController()->getTowersInRange(this->position, max_tower_mine_range);
        for (auto t : towers) {
            int max_gather = glm::min(this->current_resource, tower_gather_rate);
            if (max_gather > 0) {
                this->current_resource -= max_gather;
                this->manager->getGameController()->increaseWealth(max_gather);
                tower_gather.push_back(t);
            }
        }

    } else {
        // No resource available
        destroySelf();
    }

}

void ResourceMine::render() {  
    this->mine_texture->render(this->getX(), this->getY()/*, scale, scale, 0.0f*/);
}
void ResourceMine::renderGUI(){
    float progress = ((float)current_resource / (float)starting_resource);

    // Draw links to mining progress
    this->manager->getRenderManager()->setBlendModeAdditive();
    for (auto t : this->tower_gather) {
        glm::vec2 midpoint = (this->position + t->getPosition())*0.5f;
        int dist = (int)glm::distance(this->position, t->getPosition());

        float angle = point_direction(this->position, t->getPosition());
        this->mining_effect_texture->render((int)midpoint.x, (int)midpoint.y, dist + 2, this->mining_effect_texture->getSize().y, angle);
        this->mining_effect_section_end->render(getX(), getY());

        // Draw orbs
        //float distance_factor = dist / (float)max_tower_mine_range;
        glm::vec2 to = t->getPosition() - position;

        int MAX_ORBS = 2;//(int)glm::max(1.0f, 2.5f*(dist/(float)max_tower_mine_range));
        if (dist < 75) {
            MAX_ORBS = 1;
        }
        for (int i = 0; i < MAX_ORBS; i++) {
            float factor = glm::mod((float)i/(float)MAX_ORBS - progress*7.5f/*distance_factor*/, 1.0f);
            glm::vec2 pos = position + to*factor;
            this->mining_effect_section_end->render(pos.x, pos.y);
        }
    }

    // Sparkles:
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
void ResourceMine::release() {

    // Free sparkles
    for (auto p : sparkles) {
        delete p;
    }
}

int ResourceMine::takeResource(int amt) {
    int removed = std::min(current_resource, amt);
    current_resource -= removed;
    return removed;
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
