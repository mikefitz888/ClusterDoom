#include "../../include/GameObjects/PlayerInstance.h"
#include "../../include/manager.h"
#include "../../include/ResourceManager.h"
#include "../../include/gamecontroller.h"
#include "../../include/util.h"
#include "../../include/AnimatedTexture.h"


// Constructor
PlayerInstance::PlayerInstance(id_t id, manager::Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::PLAYER_INSTANCE, m) {
    this->setNetworkSync(true);
    this->setDepth(-10);
}

void PlayerInstance::init() {

}

void PlayerInstance::step() {
    // Check if network player instance exists
    if (!this->manager->getNetworkManager()->getPlayerExists(this->network_player_id)) {
        destroy = true;
    }

    std::cout << "Network player handler: " << this->network_player_id << std::endl;

    // Existence check
    if (destroy) {
        destroySelf();
    }
}


void PlayerInstance::render() {

}

void PlayerInstance::renderGUI() {

}

void PlayerInstance::release() {

}

// Currency management
bool PlayerInstance::useCurrency(int amount) {
    if (this->currency >= amount) {
        this->currency -= amount;
        return true;
    } else {
        return false;
    }
}

int PlayerInstance::getAvailableCurrency() {
    return this->currency;
}

void PlayerInstance::giveCurrency(int amount) {
    this->currency += amount;
}

// Control and settings
void PlayerInstance::setNetworkInstanceID(NetworkClient* client, int network_player_id) {
    this->network_client_instance = client;
    this->network_player_id = network_player_id;
}