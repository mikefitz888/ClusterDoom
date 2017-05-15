#ifndef _RESOURCE_MINE_H
#define _RESOURCE_MINE_H

#include "../gamecore.h"
#include "../../include/gameobject.h"
#include "../../include/smartpointers.h"

/*
    Resource Mine
    -------------

    The resource mine is a game object that will continuously spawn throughout a round. The mines can be collected by both units and towers.
    When a tower collects a mine, this will boost the efficiency of all towers (implemented in GameController).
        - Note: In this implementation, the Mine itself determines whether a tower is mining it, rather than having this check in the towers.

    An interface to allow anything else to claim resource is also provided. The takeResource(..) function can be called by anything to reduce
    the amount of available resource. This is used in practise by units. The function returns the amount of resource gathered.
        - Note: IF something requests to take more resource than is available, it will take all the remaining resource and the
                return value will correspond to that amount.

    Constant properties:
        - The properties of the mine can be modified in this header. This includes the range of potential resource value offered by a mine, and
          the distance at which a tower can gather/rate of gather.




*/

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

    bool last_hit_unit = false;

public:
    // Constructor
    ResourceMine(id_t id, manager::Manager* m);

    // Virtual overloads
    virtual void init() override;
    virtual void render() override;
    virtual void renderGUI() override;
    virtual void release() override;
    virtual void step() override;

    int takeResource(bool isUnit, int amt = 1);
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
