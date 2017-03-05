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

void ProjectileElectricity::init() {
    forks.clear();
    electricity_texture = this->manager->getResourceManager()->getTexture("beam_segment");
};

void ProjectileElectricity::step() {
    
    // Existence check: (We need to check that our source object and the targeting object still exist)
    if (!fork_parent || !target_object) {
        //std::cout << "ELECTRICITY DESTROYED (EXISTANCE)!!" << std::endl;
        destroySelf();
    }
    this->position        = fork_parent->getPosition();
    this->source_position = fork_parent->getPosition();

    // Verify if fork is still in range of target
    if (glm::distance(source_position, target_object->getPosition()) > range) {
        //std::cout << "ELECTRICITY DESTROYED (RANGE)!!" << range << "  " << glm::distance(source_position, target_object->getPosition()) << std::endl;
        destroySelf();
    }

    // Perform damage and timer
    fork_alive_timer--;
    if (fork_alive_timer > 0) {
        target_object->attacked(this->getSharedPtr(), this->damage_per_frame);
        if (!target_object) {
            destroySelf();
        }
    } else {
        destroySelf();
    }

    // Check if child forks still exist
    /*
        We first take the list of all forks and copy the alive ones into a new list.
        We clear the old list and fill it with the contents of the new list.
    */
    std::vector<std::pair<gameobject::unit_ptr, electricity_ptr>> alive_forks;
    for (auto fork : forks) {
        if (fork.first && fork.second) { 
            alive_forks.push_back(fork); 
        }
    }
    forks.clear();
    for (auto fork : alive_forks) {
        forks.push_back(fork);
    }


    // Check if we can output more forks:
    fork_check_timer--;
    if (fork_check_timer <= 0) {
        fork_check_timer = fork_check_timer_max;

        int available_forks = fork_count - forks.size();
        if (available_forks > 0 && fork_depth > 0) {

            // Find 
            std::vector<std::pair<float, gameobject::unit_ptr>> nearest_units = this->manager->getGameController()->getNNearestUnits(this->target_object->getPosition(), fork_count + 10, range);
            for (auto distance_unit_pair : nearest_units) {
                float distance = distance_unit_pair.first;
                gameobject::unit_ptr obj = distance_unit_pair.second;

                // check distance (and also ensure the objects aren't the same)
                if (distance <= range && this->fork_parent && this->target_object &&
                    obj->getSharedPtr() != this->getSharedPtr() &&
                    obj->getSharedPtr() != this->fork_parent &&
                    obj->getSharedPtr() != this->target_object->getSharedPtr()) {

                    // Check to ensure the element is not in the list of objects already forked to
                    bool exists;
                    for (auto fork : forks) {
                        if (fork.first == obj) {
                            exists = true;
                            break;
                        }
                    }
                    if (!exists) {

                        // Create new fork
                        electricity_ptr new_fork = smartpointers::static_pointer_cast<ProjectileElectricity>(
                            this->manager->getGameController()->spawnObjectAt(
                                gameobject::OBJECT_TYPE::PROJECTILE_ELECTRICITY,
                                this->target_object->getX(),
                                this->target_object->getY()
                                ));

                        new_fork->setTargetObject(obj);
                        new_fork->setForkCount(this->fork_count, this->fork_depth - 1);
                        new_fork->setForkParent(this->target_object->getSharedPtr());


                        // Add fork-object pair to list
                        forks.push_back(std::pair<gameobject::unit_ptr, electricity_ptr>(obj, new_fork));

                        /*std::cout << "..FORKING!" << std::endl;
                        std::cout << "      Parent: " << this->getSharedPtr() << " location: (" << this->getX() << "," << this->getY() << ")" << std::endl;
                        std::cout << "      Target: " << obj->getSharedPtr() << " location: (" << obj->getX() << "," << obj->getY() << ")" << std::endl;*/
                        // Decrement available forks
                        available_forks--;
                        if (available_forks <= 0) {
                            break;
                        }
                    }
                }
            }

        }
    }
}

void ProjectileElectricity::render() {
    if (target_object) {
        glm::vec2 midpoint = (source_position + target_object->getPosition())*0.5f;
        int dist = glm::distance(source_position, target_object->getPosition());

        if (dist <= range) {
            float angle = point_direction(source_position, target_object->getPosition());
            float alpha_fac = glm::clamp(((float)fork_alive_timer / (float)fork_alive_timer_max), 0.0f, 1.0f);

            this->manager->getRenderManager()->setActiveColour(255, 255, 255, 255.0f*alpha_fac);
            this->electricity_texture->render(midpoint.x, midpoint.y, dist + 2, this->electricity_texture->getSize().y, angle, 1.0f);
            this->manager->getRenderManager()->setActiveColour(255, 255, 255, 255);
        }
    }
};

void ProjectileElectricity::renderGUI() {};
void ProjectileElectricity::release() {};


// Property modifiers
void ProjectileElectricity::setDamage(int damage_per_frame) {
    this->damage_per_frame = damage_per_frame;
}

void ProjectileElectricity::setForkCount(int fork_count, int fork_depth) {
    this->fork_count = fork_count;
    this->fork_depth = fork_depth;
}

void ProjectileElectricity::setRange(int range) {
    this->range = range;
}

void ProjectileElectricity::setTargetObject(gameobject::unit_ptr target) {
    this->target_object = target;
}

void ProjectileElectricity::setForkParent(gameobject_ptr parent) {
    this->fork_parent = parent;
}


// ****************************************************** //

