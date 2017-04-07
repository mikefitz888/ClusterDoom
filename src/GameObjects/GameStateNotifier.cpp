#include "../../include/GameObjects/GameStateNotifier.h"
#include "../../include/manager.h"
#include "../../include/gamecontroller.h"
#include "../../include/smartpointers.h"

// Constructor
GameStateNotifier::GameStateNotifier(id_t id, manager::Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::GAME_STATE_NOTIFIER, m) {

}

// EVENTS ------------------------------------------------

// init
void GameStateNotifier::init() {

}
void GameStateNotifier::render() {}
void GameStateNotifier::renderGUI() {}
void GameStateNotifier::release() {}

// Step event
void GameStateNotifier::step() {
    
    // Send wealth
    this->send_wealth_timer--;
    if (this->send_wealth_timer <= 0) {
        this->send_wealth_timer = this->send_wealth_timer_max;
        this->sendNetworkUpdate(GameStateNotifierUpdateEvents::SEND_WEALTH);
    }

    // Send warnings
    std::vector<std::string> alerts = this->game_controller->getWarnings();
    if (alerts.size() > this->last_warnings_list_length) {
        // We have a new warning, so send:
        this->last_warning = alerts.back();
        this->sendNetworkUpdate(GameStateNotifierUpdateEvents::SEND_WARNINGS);
    }

    // Send Wave timer
    sf::Time current_time = this->game_controller->timeUntilNextWave();
    wave_timer = (unsigned int) current_time.asSeconds() * 60;

    this->send_wave_timer--;
    if (this->send_wave_timer <= 0 || wave_timer > last_send_time) {
        this->send_wave_timer = this->send_wave_timer_max;
        
        

        this->sendNetworkUpdate(GameStateNotifierUpdateEvents::SEND_WAVE_TIMER);
        last_send_time = wave_timer;
    }

    // This will trigger a re-send once the timer is reset (to something larger than zero) again (after having completed).
    if (wave_timer < 0) {
        last_send_time = 0;
    }

    // Send base health
    float base_health = this->game_controller->getBaseHealth();
    this->send_base_health_timer--;
    if (this->send_base_health_timer <= 0 || (base_health < this->last_sent_base_health-1.0f)) {
        this->last_sent_base_health  = base_health;
        this->send_base_health_timer = this->send_base_health_timer_max;
        this->sendNetworkUpdate(GameStateNotifierUpdateEvents::SEND_BASE_HEALTH);
    }


}

// NETWORKING ------------------------------------------------
/*
    This method writes out the data to the network. This function should
    be paralleled on the client implementation.
*/
void GameStateNotifier::writeNetworkUpdate(int event_id, Buffer &buffer) {
    switch (event_id) {

        // SPAWN TIMER 
        case GameStateNotifierUpdateEvents::SEND_WAVE_TIMER:
            buffer << (unsigned int)wave_timer; // sends the wave timer as a number of frames
            break;

        // WEALTH
        case GameStateNotifierUpdateEvents::SEND_WEALTH:
            buffer << this->game_controller->availableWealth();
            break;

        // WARNINGS
        case GameStateNotifierUpdateEvents::SEND_WARNINGS:
            buffer << this->last_warning.c_str();
            break;

        // SEND BASE HEALTH
        case GameStateNotifierUpdateEvents::SEND_BASE_HEALTH:
            buffer << this->last_sent_base_health;
            buffer << this->game_controller->getBaseMaxHealth();
            break;

    }
}
// -------------------------------------------------------