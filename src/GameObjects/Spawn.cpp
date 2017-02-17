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
	if(running && manager->getTowers().size() >= 0 && manager->getUnits().size() < 1000){
        //printf("%d\n", time);
        if (time % spawn_rate == 0) {
            manager->getGameController()->spawnUnitAt(getX(), getY());
        }
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
    spawn_rate = (6 - wave) * 60;
    spawn_queue.emplace_back(unit::TYPE::BASIC);
}

Spawn::unit_spawn::unit_spawn(unit::TYPE type, int t) : unit_type(type), time(t) {}