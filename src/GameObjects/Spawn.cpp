#include "../../include/GameObjects/Spawn.h"
#include "../../include/manager.h"
#include "../../include/ResourceManager.h"
#include "../../include/gamecontroller.h"
#include "../../include/network/Network.h"
#include "../../include/GameObjects/PlayerInstance.h"

//using manager::Manager;

Spawn::Spawn(id_t id, manager::Manager* m) : GameObject(id, gameobject::TYPE::OBJECT, gameobject::OBJECT_TYPE::SPAWN, m) {

}
void Spawn::init() {
    rng.seed(std::random_device()());
	//manager->getGameController()->spawnUnitAt(getX(), getY());
}
void Spawn::step() {

    // If singleplayer mode, spawn units
    if (this->manager->getGameController()->getCurrentGameMode() == gamecontroller::GameMode::SINGLE_PLAYER) {

        //Potential to get accurate game_time from game_controller
        int t = time;
        //Steps through the spawn queue counting down the timer on any delayed-spawns, removes any that have spawned.
        if (running && manager->getTowers().size() >= 0 && manager->getUnits().size() < 1000) {
            spawn_queue.erase(
                std::remove_if(spawn_queue.begin(), spawn_queue.end(), [this](const unit_spawn s) -> bool {
                if (time > s.delay) {
                    manager->getGameController()->spawnUnitAt(getX() + spawn_jitter(rng), getY() + spawn_jitter(rng), s.unit_type)->setDifficulty(1.f + ((float)scenario * 3.f + (float)wave)/15.f);
                    return true;
                }
                return false;
            }), spawn_queue.end());
            time++;
        }
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
    size_t offset = 0;
    for (size_t number_of_spawn_attempts = 2 + (size_t) ((float)scenario * (float)scenario/3.f); number_of_spawn_attempts > 0; number_of_spawn_attempts--) {
        if (number_of_spawn_attempts > 2 && noSpawn(rng)) {
            continue;
        }
        unsigned int unit_type_id = random_unit(rng);
        for (int i = cluster_size[unit_type_id-1](rng); i > 0; i--) {
            spawn_queue.emplace_back(static_cast<unit::TYPE>(unit_type_id), time + offset);
            offset += 200;
        }
    }

    return;
    //Adjust parameters for the next wave based on this->wave and this->scenario here
    int spawn_rate = 15 - scenario;
    int delay = time;
    std::cout << scenario << "\n";
    spawn_queue.clear();
    int focus = (wave + 3 * scenario) % 10;
    //if (focus != spawn_id) return;
    for (int number_of_enemies = 2 + scenario/3; number_of_enemies > 0; number_of_enemies--) {
        std::cout << number_of_enemies << "\n";
		// auto k didn't seem to do anything, changed one-line if to allow more units
        //auto k = 0 < 4 ? unit::TYPE::WIZARD : unit::TYPE::BASIC;
        //spawn_queue.emplace_back((number_of_enemies+focus) % 2 ? unit::TYPE::WIZARD : unit::TYPE::BASIC, delay);
		if ((number_of_enemies + focus) % 9 < 3) {
			spawn_queue.emplace_back(unit::TYPE::BASIC, delay);
			spawn_queue.emplace_back(unit::TYPE::BASIC, delay+300);
		}
		else if ((number_of_enemies + focus) % 9 < 6) {
			spawn_queue.emplace_back(unit::TYPE::PIRATE, delay);
			spawn_queue.emplace_back(unit::TYPE::PIRATE, delay+300);
			spawn_queue.emplace_back(unit::TYPE::PIRATE, delay+600);
			//spawn_queue.emplace_back(unit::TYPE::PIRATE, delay+900);
			//spawn_queue.emplace_back(unit::TYPE::PIRATE, delay+1200);
			//spawn_queue.emplace_back(unit::TYPE::PIRATE, delay+1500);
		}
		else {
			spawn_queue.emplace_back(unit::TYPE::WIZARD, delay);
			spawn_queue.emplace_back(unit::TYPE::WIZARD, delay+300);
			//spawn_queue.emplace_back(unit::TYPE::WIZARD, delay+600);
			//spawn_queue.emplace_back(unit::TYPE::WIZARD, delay+900);
		}
        delay += spawn_rate;
    }
    
}

Spawn::unit_spawn::unit_spawn(unit::TYPE type, int d) : unit_type(type), delay(d) {}

// NETWORK EVENTS
void Spawn::recvNetworkInteraction(int event_id, Buffer &buffer, network::NetworkClient* interaction_connection_client) {

    // Switch on events
    switch (event_id) {
        case NetworkInteractionEvent::SPAWN_UNIT:

            // Interaction information
            unsigned int unit_type = 0;
            int target = -1;
            buffer >> unit_type;
            buffer >> target;
            std::cout << "SPAWN RECEIVED";




            // Get spawner
            smartpointers::slave_ptr<PlayerInstance> player = interaction_connection_client->getPlayerInstance();

            if (player != nullptr && player) {

                // Determine unit cost
                int unit_cost = 9999;
                int unit_num = 0;
                switch (unit_type) {
                    case unit::TYPE::BASIC:
                        unit_cost = player->ROBOT_COST;
                        unit_num = 3;
                        break;
                    case unit::TYPE::PIRATE:
                        unit_cost = player->PIRATE_COST;
                        unit_num = 4;
                        break;
                    case unit::TYPE::WIZARD:
                        unit_cost = player->WIZARD_COST;
                        unit_num = 2;
                        break;
                }

                // If we have enough money, spawn
                if (player->getAvailableCurrency() >= unit_cost) {
                    if (player->useCurrency(unit_cost)) {
                        // If multiplayer mode, spawn units
                        if (this->manager->getGameController()->getCurrentGameMode() == gamecontroller::GameMode::MULTI_PLAYER) {
                            
                            for (int r = 0; r < unit_num; r++) {
                                smartpointers::slave_ptr<unit::Unit> unit = smartpointers::static_pointer_cast<unit::Unit>(manager->getGameController()->spawnUnitAt(getX(), getY(), (unit::TYPE)unit_type));
                                if (target != -1) game_controller->unitTargetMine(unit->getID(), target);
                            }
                        }
                    }
                }
            }
            break;
    }
}