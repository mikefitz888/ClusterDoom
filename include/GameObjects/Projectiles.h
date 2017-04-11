#ifndef _PROJECTILES_H
#define _PROJECTILES_H

#include "../gamecore.h"
#include "../../include/gameobject.h"
#include "../../include/smartpointers.h"



/*
    Projectile Bomb
    ---------------

    The bomb projectile will explode after a given amount of time, damaging
    all enemies within its radius.

*/
class ProjectileBomb : public GameObject {
private:
    graphics::Texture* bomb_texture, *bomb_white_texture;
    graphics::AnimatedTexture* explosion_texture;

    int timer = 60;
    int damage = 1000;
    int blast_radius  = 80;
    int falloff_range = 25; // Damage is maximal up to 50 units off
    int knockback_force = 50;

    // Animation variables
    float animation_rate = 0.05f;
    float animation_progress = 0.0f;
    bool exploded = false;
    int num_explosion_frames = 0;

    // Private methods
    void explode();

public:
    // Constructor
    ProjectileBomb(id_t id, manager::Manager* m);

    // Virtual overloads
    virtual void init() override;
    virtual void render() override;
    virtual void renderGUI() override;
    virtual void release() override;
    virtual void step() override;

    // Property modifiers
    void setExplosionDamage(int damage);
    void setExplosionRadius(int radius, int damage_falloff_radius); 
    void setExplosionTimer(int timer);
    void setExplosionKnockback(int knockback);
};




/*
Projectile Laser
---------------

Longer range laser fired from the wizard towers

*/
class ProjectileLaser : public GameObject {
    int damage = 50;
    gameobject::TYPE collision_type = gameobject::TYPE::UNIT;
public:

	sf::String textureName = "redLaser";

    // Constructor
    ProjectileLaser(id_t id, manager::Manager* m);

    // Virtual overloads
    virtual void init() override;
    virtual void render() override;
    virtual void renderGUI() override;
    virtual void release() override;
    virtual void step() override;
    void onCollision(gameobject_ptr other) override;

    // Property modifiers
    void setCollisionType(gameobject::TYPE type);
    void setDamage(int damage);
    int  getDamage();
};




/*
Projectile Electricity
---------------

Electricity fired from wizard towers

*/
class ProjectileElectricity : public GameObject {
    typedef smartpointers::slave_ptr<ProjectileElectricity> electricity_ptr;
private:
    graphics::Texture* electricity_texture;

    // Targeting properties
    glm::vec2 source_position;              // Position from which the electricity is eminating
    gameobject::unit_ptr target_object = nullptr; // Gameobject that this is targeting.
    gameobject_ptr fork_parent = nullptr;   // The object from which the electricity eminated

    // Forks
    std::vector<std::pair<gameobject::unit_ptr, electricity_ptr>> forks;

    // Gameplay properties
    int damage_per_frame = 3; // The amount of damage the electricity inflicts
    int fork_count = 2;       // The number of subsequent objects the electricity will fork to.
    int fork_depth = 2;       // Number of subsequent objects that can then themselves branch
    int range = 200;           // The range of the beam

    // Timers
    int fork_alive_timer_max = 30;
    int fork_alive_timer = fork_alive_timer_max;
    int fork_check_timer_max = 8; // If higher than alive timer, it wont check again
    int fork_check_timer = 2;// fork_check_timer_max;


public:
    // Constructor
    ProjectileElectricity(id_t id, manager::Manager* m);

    // Virtual overloads
    virtual void init() override;
    virtual void render() override;
    virtual void renderGUI() override;
    virtual void release() override;
    virtual void step() override;

    // Property modifiers
    void setDamage(int damage_per_frame);
    void setForkCount(int fork_count, int fork_depth);
    void setRange(int range);
    void setTargetObject(gameobject::unit_ptr target);
    void setForkParent(gameobject_ptr parent);
};


#endif