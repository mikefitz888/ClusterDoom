#ifndef _PROJECTILES_H
#define _PROJECTILES_H

#include "../gamecore.h"
#include "../../include/gameobject.h"



/*
    Projectile Bomb
    ---------------

    The bomb projectile will explode after a given amount of time, damaging
    all enemies within its radius.

*/
class ProjectileBomb : public GameObject {
private:
    graphics::Texture* bomb_texture;
    int timer = 300;
    int damage = 150;
    int blast_radius  = 150;
    int falloff_range = 50; // Damage is maximal up to 50 units off

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
};




/*
Projectile Laser
---------------

Longer range laser fired from the wizard towers

*/
class ProjectileLaser : public GameObject {

public:
    // Constructor
    ProjectileLaser(id_t id, manager::Manager* m);

    // Virtual overloads
    virtual void init() override;
    virtual void render() override;
    virtual void renderGUI() override;
    virtual void release() override;
    virtual void step() override;
};




/*
Projectile Electricity
---------------

Longer range laser fired from the wizard towers

*/
class ProjectileElectricity : public GameObject {

public:
    // Constructor
    ProjectileElectricity(id_t id, manager::Manager* m);

    // Virtual overloads
    virtual void init() override;
    virtual void render() override;
    virtual void renderGUI() override;
    virtual void release() override;
    virtual void step() override;
};


#endif