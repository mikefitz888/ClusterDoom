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

    //std::cout << "Network player handler: " << this->network_player_id << std::endl;

    // Existence check
    if (destroy) {
        destroySelf();
    }

    // Reward currency
    currency_reward_timer--;
    if (currency_reward_timer <= 0) {
        currency_reward_timer = currency_reward_timer_max;
        giveCurrency(1);
    }

    // Update currency
    send_currency_timer--;
    if (send_currency_timer <= 0) {
        send_currency_timer = send_currency_timer_max;
        sendNetworkUpdate(PlayerNetworkEvents::SEND_CURRENCY);
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
    if (this->currency > this->max_currency) {
        this->currency = this->max_currency;
    }
}

// Control and settings
void PlayerInstance::setNetworkInstanceID(NetworkClient* client, int network_player_id) {
    this->network_client_instance = client;
    this->network_player_id = network_player_id;
}

// Networking
void PlayerInstance::writeNetworkUpdate(int event_id, Buffer &buffer) {
    switch (event_id) {
        case SEND_CURRENCY:
            buffer << (unsigned int)currency;
            buffer << (unsigned int)max_currency;
            std::cout << "SENDING CURRENCY " << currency << std::endl;
            break;
    }
}