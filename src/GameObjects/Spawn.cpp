#include "../../include/GameObjects/Spawn.h"
#include "../../include/manager.h"
Spawn::Spawn(int x, int y, id_t id, Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::SPAWN, m), position(Point<int>(x, y)){}

Spawn::Spawn(Point<int> position, id_t id, Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::SPAWN, m), position(position) {}

int Spawn::getX() {
	return this->position.x;
}

int Spawn::getY() {
	return this->position.y;
}

Point<int> Spawn::getPosition() {
	return this->position;
}


void Spawn::init() {}
void Spawn::step() {}
void Spawn::render() {
	graphics::Texture *tex = this->manager->getResourceManager()->getTexture("spawn");
	tex->render(this->position.x, this->position.y);
}
void Spawn::renderGUI() {}
void Spawn::release() {}