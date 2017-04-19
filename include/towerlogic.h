#ifndef TOWERLOGIC_H
#define TOWERLOGIC_H

#include "gamecore.h"

namespace towerlogic {
    using tower::Tower;
    using manager::Manager;

    class TowerLogic {
        std::vector<tower_ptr> towers;
        Manager* manager;

    public:
        TowerLogic(Manager* m);
        std::vector<tower_ptr>& getTowers();
        tower_ptr getBase();

        void clean();
        Tower* createTower (id_t key, tower::TYPE type) const;
        tower_ptr createTower(tower::TYPE type, float x, float y) const;
        void removeTower(int x, int y);
        void removeTower(tower_ptr tower);

        void giveSlavePtr(tower_ptr tower);
    };
}

#endif //TOWERLOGIC_H
