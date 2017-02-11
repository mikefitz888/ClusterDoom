#include "../../include/GameObjects/Spawn.h"
#include "../../include/manager.h"
#include "../../include/ResourceManager.h"
#include "../../include/gamecontroller.h"

//using manager::Manager;

Spawn::Spawn(id_t id, manager::Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::SPAWN, m) {

}
void Spawn::init() {
	//manager->getGameController()->spawnUnitAt(getX(), getY());
}
void Spawn::step() {
	//manager->getGameController()->spawnUnitAt(getX(), getY());
	if(manager->getTowers().size() >= 0 && manager->getUnits().size() < 10){
		manager->getGameController()->spawnUnitAt(getX(), getY());
	}
}
void Spawn::render() {
	graphics::Texture *tex = this->manager->getResourceManager()->getTexture("spawn");
	tex->render(this->position.x, this->position.y);
}
void Spawn::renderGUI() {}
void Spawn::release() {}

void Spawn::startSpawn(void* arg, void* ret) {

}

void Spawn::continueSpawn(void* arg, void* ret) {

}