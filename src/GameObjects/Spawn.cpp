#include "../../include/GameObjects/Spawn.h"
#include "../../include/manager.h"
Spawn::Spawn(gameobject::id_t id, Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::SPAWN, m) {}
void Spawn::init() {
	//manager->getGameController()->spawnUnitAt(getX(), getY());
}
void Spawn::step() {
	//manager->getGameController()->spawnUnitAt(getX(), getY());
}
void Spawn::render() {
	graphics::Texture *tex = this->manager->getResourceManager()->getTexture("spawn");
	tex->render(this->position.x, this->position.y);
}
void Spawn::renderGUI() {}
void Spawn::release() {}