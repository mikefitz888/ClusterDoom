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
	//Potential to get accurate game_time from game_controller
    int t = time;
	if(running && manager->getTowers().size() >= 0 && manager->getUnits().size() < 1000){
        spawn_queue.erase(
            std::remove_if(spawn_queue.begin(), spawn_queue.end(), [this](const unit_spawn s) -> bool { 
                if (time > s.delay) {
                    manager->getGameController()->spawnUnitAt(getX(), getY(), s.unit_type);
                    return true;
                }
                return false;
            }), spawn_queue.end());
        time++;
	}
}
void Spawn::render() {
	graphics::Texture *tex = this->manager->getResourceManager()->getTexture("spawn");
	tex->render((int)this->position.x, (int)this->position.y);
}
void Spawn::renderGUI() {}
void Spawn::release() {}
void Spawn::setSpawnID(size_t id) {
    spawn_id = id;
}

void Spawn::startScenario(int scenario_number) {
    running = true;
    this->scenario = scenario_number;
    startWave(0);
}

void Spawn::startWave(int wave_number) {
    this->wave = wave_number;
    beginWave();
}

void Spawn::beginWave() {
    //Adjust parameters for the next wave based on this->wave and this->scenario here
    int spawn_rate = 15 - scenario;
    int delay = time;
    std::cout << scenario << "\n";
    spawn_queue.clear();
    int focus = (wave + 3 * scenario) % 4;
    if (focus != spawn_id) return;
    for (int number_of_enemies = 1 + scenario*scenario; number_of_enemies > 0; number_of_enemies--) {
        std::cout << number_of_enemies << "\n";
		// auto k didn't seem to do anything, changed one-line if to allow more units
        //auto k = 0 < 4 ? unit::TYPE::WIZARD : unit::TYPE::BASIC;
        //spawn_queue.emplace_back((number_of_enemies+focus) % 2 ? unit::TYPE::WIZARD : unit::TYPE::BASIC, delay);
		if ((number_of_enemies + focus) % 3 == 0) {
			spawn_queue.emplace_back(unit::TYPE::BASIC, delay);
		}
		else if ((number_of_enemies + focus) % 3 == 1) {
			spawn_queue.emplace_back(unit::TYPE::PIRATE, delay);
		}
		else {
			spawn_queue.emplace_back(unit::TYPE::WIZARD, delay);
		}
        delay += spawn_rate;
    }
    
}

Spawn::unit_spawn::unit_spawn(unit::TYPE type, int d) : unit_type(type), delay(d) {}

// NETWORK EVENTS
void Spawn::recvNetworkInteraction(int event_id, Buffer &buffer) {

    // Switch on events
    switch (event_id) {
        case NetworkInteractionEvent::SPAWN_UNIT:
            unsigned int unit_type = 0;
            buffer >> unit_type;
            std::cout << "SPAWN RECEIVED";
            manager->getGameController()->spawnUnitAt(getX(), getY(), (unit::TYPE)unit_type);
            break;
    }
}