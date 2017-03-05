#include "../../include/GameObjects/Projectiles.h"
#include "../../include/manager.h"
#include "../../include/ResourceManager.h"
#include "../../include/gamecontroller.h"
#include "../../include/unit.h"
#include "../../include/util.h"
#include "../../include/AnimatedTexture.h"

// ****************************************************** //
// Projectile Bomb

ProjectileBomb::ProjectileBomb(id_t id, manager::Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::SPAWN, m) {

}

void ProjectileBomb::init() {

    // Fetch texture
    this->bomb_texture = this->manager->getResourceManager()->getTexture("bomb_texture");
    this->bomb_white_texture = this->manager->getResourceManager()->getTexture("bomb_texture_white");
    this->explosion_texture = this->manager->getResourceManager()->getAnimatedTexture("explosion");

    num_explosion_frames = this->explosion_texture->getTotalFrames();
}

void ProjectileBomb::step() {
    GameObject::step();

    // Animate
    animation_rate += 0.005;
    animation_progress += animation_rate;

    // If not exploded
    if (!exploded) {
        // Check timer
        this->timer--;
        if (this->timer <= 0) {
            // Explode
            this->explode();
        }

        
    } else {
        // Show explosion
        if (animation_progress >= num_explosion_frames) {
            destroySelf();
        }
    }

    
}

void ProjectileBomb::explode() {
    
    std::vector<gameobject::unit_ptr> objects_in_range = this->manager->getGameController()->getUnitsInRange(this->position, this->blast_radius);
    for (gameobject::unit_ptr obj : objects_in_range) {

        // if the object exists, do damage
        if (obj) {

            // Adjust damage based on distance
            int dist  = obj->distanceTo(this->position);
            float force_factor = glm::clamp(1.0f - ((dist - this->falloff_range) / this->blast_radius), 0.0f, 1.0f);
            int damage = force_factor*this->damage;
            if (damage > 0) {
                obj->attacked(this->getSharedPtr(), this->damage);

                // If still alive, apply knockback
                if (obj) {
                    glm::vec2 dir = obj->getPosition() - this->position;
                    dir = glm::normalize(dir);
                    //obj->addVelocity(dir*0.25f);
                    float knockback_force = force_factor*(float)this->knockback_force;
                    obj->setPosition(obj->getX() + dir.x*knockback_force, obj->getY() + dir.y*knockback_force);
                }
            }
        }
    }

    // set exploded
    exploded = true;
    animation_rate = 0.75;
    animation_progress = 0.0f;
}

void ProjectileBomb::render() {
    if (!this->exploded) {
        this->bomb_texture->render(this->getX(), this->getY());
        int anim_alpha = (int)255.0f*glm::clamp(glm::sin(animation_progress), 0.0f, 1.0f);
        this->manager->getRenderManager()->setActiveColour(255, 255, 255, anim_alpha);
        this->bomb_white_texture->render(this->getX(), this->getY());
        this->manager->getRenderManager()->setActiveColour(255, 255, 255, 255);
    } else {
        this->explosion_texture->render(glm::floor(animation_progress), this->getX(), this->getY());
    }
}


void ProjectileBomb::renderGUI() {}
void ProjectileBomb::release() {}

// Property modifiers
void ProjectileBomb::setExplosionDamage(int damage) {
    this->damage = damage;
}

void ProjectileBomb::setExplosionRadius(int radius, int damage_falloff_radius) {
    this->blast_radius = radius;
    this->falloff_range = damage_falloff_radius;
}

void ProjectileBomb::setExplosionTimer(int timer) {
    this->timer = timer;
}

void ProjectileBomb::setExplosionKnockback(int knockback) {
    this->knockback_force = knockback;
}









// ****************************************************** //
// Projectile Laser
ProjectileLaser::ProjectileLaser(id_t id, manager::Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::SPAWN, m) {

}

void ProjectileLaser::init() {};
void ProjectileLaser::render() {};
void ProjectileLaser::renderGUI() {};
void ProjectileLaser::release() {};
void ProjectileLaser::step() {};

// ****************************************************** //
// Projectile Electricity
ProjectileElectricity::ProjectileElectricity(id_t id, manager::Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::SPAWN, m) {

}

void ProjectileElectricity::init() {};
void ProjectileElectricity::render() {};
void ProjectileElectricity::renderGUI() {};
void ProjectileElectricity::release() {};
void ProjectileElectricity::step() {};

// ****************************************************** //

