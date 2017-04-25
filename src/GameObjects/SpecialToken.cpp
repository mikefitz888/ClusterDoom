#include "../../include/GameObjects/SpecialToken.h"

SpecialToken::SpecialToken(id_t id, manager::Manager* m, tower::SPECIAL_TYPE type) 
    : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::SPECIAL_TOKEN, m) {
    this->type = type;
    this->setNetworkSync(true);
    this->setDepth(-10);
}

void SpecialToken::init() {

}

void SpecialToken::render() {

}

void SpecialToken::renderGUI() {

}

void SpecialToken::release() {

}

void SpecialToken::step() {

}
