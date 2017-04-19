#include "../include/towerlogic.h"
#include "../include/Towers/BasicTower.h"
#include "../include/Towers/Base.h"
#include "../include/Towers/ElectricTower.h"
#include "../include/Towers/BombTower.h"
#include "../include/Towers/LaserTower.h"
#include "../include/Towers/SpecialTower.h"
#include "../include/manager.h"

namespace towerlogic {
    TowerLogic::TowerLogic(Manager* m) :
        manager(m) {}

    tower_ptr TowerLogic::createTower(tower::TYPE type, float x, float y) const {
        return manager->createTower(type, x, y);
    }

    //Tower factory
    Tower* TowerLogic::createTower(id_t key, tower::TYPE type) const {
        switch(type)
        {
        case tower::TYPE::BASIC:
            return new tower::BasicTower(key, manager);
        case tower::TYPE::BASE:
            return new tower::Base(key, manager);
        case tower::TYPE::ELECTRIC:
            return new tower::ElectricTower(key, manager);
        case tower::TYPE::BOMB:
            return new tower::BombTower(key, manager);
        case tower::TYPE::LASER:
            return new tower::LaserTower(key, manager);
        case tower::TYPE::SPECIAL:
            return new tower::SpecialTower(key, manager);
        default:
            std::cout << "FATAL ERROR! INCORRECT INSTANCE, nullptr RETURNED" << std::endl;
            return nullptr;
        }
    }

    std::vector<tower_ptr>& TowerLogic::getTowers() {
        return towers;
    } //TODO: filter out BASE

    tower_ptr TowerLogic::getBase() {
        return towers[0];
        //return nullptr;
    }

    void TowerLogic::clean() {
        towers.erase(std::remove_if(towers.begin(), towers.end(), [&](tower_ptr x)
        {
            return !(x.valid());
        }), towers.end());
    }

    void TowerLogic::giveSlavePtr(tower_ptr towerptr){
        //Look for free place in towers pool
        for (auto tower = towers.begin(); tower != towers.end(); ++tower) {
            if(!(*tower)) {
                towers.emplace(tower, towerptr);
                return;
            }
        }
        towers.push_back(towerptr);
    }

    void TowerLogic::removeTower(int x, int y) {
        for(auto it = towers.begin(); it != towers.end(); ++it) {
            if ((*it)->getX() == x && (*it)->getY() == y) {
                return manager->destroyTower((*it));
            }
         }
    }
}
