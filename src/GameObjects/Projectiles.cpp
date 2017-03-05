#include "../../include/GameObjects/Projectiles.h"
#include "../../include/manager.h"
#include "../../include/ResourceManager.h"
#include "../../include/gamecontroller.h"
#include "../../include/unit.h"

// ****************************************************** //
// Projectile Bomb

ProjectileBomb::ProjectileBomb(id_t id, manager::Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::SPAWN, m) {

}

void ProjectileBomb::init() {

    // Fetch texture
    this->bomb_texture = this->manager->getResourceManager()->getTexture("bomb_texture");
}

void ProjectileBomb::step() {
    GameObject::step();

    // Check timer
    this->timer--;
    if (this->timer <= 0) {
        // Explode
        this->explode();
    }
}

void ProjectileBomb::explode() {
    
    std::vector<gameobject::unit_ptr> objects_in_range = this->manager->getGameController()->getUnitsInRange(this->position, this->blast_radius);
    for (gameobject::unit_ptr obj : objects_in_range) {

        // if the object exists, do damage
        if (obj) {

            // Adjust damage based on distance
            int dist = obj->distanceTo(this->position);
            int damage = glm::clamp(1.0f - ((dist - this->falloff_range) / this->blast_radius), 0.0f, 1.0f)*this->damage;
            obj->attacked(this->getSharedPtr(), this->damage);
        }
    }

    // Destroy self
    destroySelf();
}

void ProjectileBomb::render() {
    this->bomb_texture->render(this->getX(), this->getY());
}


void ProjectileBomb::renderGUI() {}
void ProjectileBomb::release() {}









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

