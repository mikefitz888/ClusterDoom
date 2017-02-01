#ifndef TOWERLOGIC_H
#define TOWERLOGIC_H

#include "gamecore.h"

#include "smartpointers.h"
#include "tower.h"
#include "manager.h"

namespace towerlogic {
    using tower::Tower;
    using manager::Manager;
    using gameobject::id_t;

    class TowerLogic {
        std::vector<tower_ptr> towers;
        Manager * manager;

    public:
        inline TowerLogic(Manager* m) : manager(m) {};
		std::vector<tower_ptr>& getTowers();
		tower_ptr getBase();

        inline void clean() { towers.erase(std::remove_if(towers.begin(), towers.end(), [&](tower_ptr x) {return !(x.valid()); }), towers.end()); }
        Tower* createTower (id_t key, tower::TYPE type) const;
        tower_ptr createTower(tower::TYPE type) const;
        void removeTower(int x, int y);
        void removeTower(tower_ptr tower);

        void giveSlavePtr(tower_ptr tower);
    };
}

#endif //TOWERLOGIC_H
