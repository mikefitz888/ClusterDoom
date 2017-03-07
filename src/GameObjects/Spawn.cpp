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
    for (int number_of_enemies = 1 + scenario*scenario; number_of_enemies > 0; number_of_enemies--) {
        std::cout << number_of_enemies << "\n";
        spawn_queue.emplace_back(unit::TYPE::BASIC, delay);
        delay += spawn_rate;
    }
    
}

Spawn::unit_spawn::unit_spawn(unit::TYPE type, int d) : unit_type(type), delay(d) {}