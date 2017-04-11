#ifndef _RESOURCE_MINE_H
#define _RESOURCE_MINE_H

#include "../gamecore.h"
#include "../../include/gameobject.h"
#include "../../include/smartpointers.h"

class ResourceMine : public GameObject {

private:
    graphics::Texture* mine_texture, *sparkle_texture, *mining_effect_texture, *mining_effect_section_end;
    std::vector<ResourceMineSparkle*> sparkles;
    bool sparkles_created = false;

    // Properties
    const int max_tower_mine_range = 105;
    const int tower_gather_rate = 1;
    std::vector<gameobject::tower_ptr> tower_gather;

    // Wealth
    const int max_available_resource = 600;
    const int min_available_resource = 250;

    int starting_resource;
    int current_resource;

public:
    // Constructor
    ResourceMine(id_t id, manager::Manager* m);

    // Virtual overloads
    virtual void init() override;
    virtual void render() override;
    virtual void renderGUI() override;
    virtual void release() override;
    virtual void step() override;
};

class ResourceMineSparkle {
    friend class ResourceMine;

    glm::vec2 source_position;
    glm::vec2 draw_position;
    float angle = 0.0f;
    float alpha = 0.0f;
    float target_scale = 1.0f;
    const float scale_min = 0.2f;
    const float scale_max = 0.55f;
    const int sparkle_timer_max = 180;
    const int sparkle_timer_min = 40;
    const int invisible_timer_min = 60;
    const int invisible_timer_max = 200;
    const glm::vec2 position_offset_min = glm::vec2(-30.0f, -28.0f);
    const glm::vec2 position_offset_max = glm::vec2(35.0f, 28.0f);
    int timer = 0;

    bool visible = false;
    bool position_modified = false;
    
    gameobject_ptr parent = nullptr;

    void timer_reset();

public:
    ResourceMineSparkle(glm::vec2 source_pos, gameobject_ptr parent);
    void step();
};

#endif
