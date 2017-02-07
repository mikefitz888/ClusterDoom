#ifndef MANAGER_H
#define MANAGER_H

#include "gamecore.h"

#include "smartpointers.h"
#include "unitlogic.h"
#include "towerlogic.h"
#include "ObjectLogic.h"
#include "gameobject.h"
#include "gamecontroller.h"
#include "RenderManager.h"
#include "VertexBuffer.h"
#include "WorldRenderer.h"
#include "network/Network.h"
#include "ResourceManager.h"
#include "AudioManager.h"

namespace manager {
    using gameobject::GameObject;
    using smartpointers::master_ptr;
    using smartpointers::slave_ptr;
    using smartpointers::static_pointer_cast;
    using towerlogic::TowerLogic;
    using unitlogic::UnitLogic;
    using gamecontroller::GameController;
    using unit::Unit;
    using tower::Tower;
    using graphics::RenderManager;
    using worldrenderer::WorldRenderer;
    using network::NetworkManager;
    using gameobject::Collision;

    //typedef size_t id_t;

    class Manager {
        std::vector<master_ptr<GameObject>> game_object_pool;
        std::vector<id_t> free_id_list;

        TowerLogic* tower_logic;
        UnitLogic* unit_logic;
        GameController* game_controller; //IRenderable
        ObjectLogic* object_logic;

        RenderManager*  render_manager    = nullptr;
        WorldRenderer*  world_renderer    = nullptr;
        NetworkManager* network_manager   = nullptr;
        ResourceManager* resource_manager = nullptr;
        AudioManager*    audio_manager    = nullptr;

        void addToPool(GameObject* game_object);
        void removeFromPool(id_t id);
        id_t getFreePoolKey();

    public:
        Manager();
        GameController* getGameController() const;
        //Tower Methods
        slave_ptr<Tower> createTower(tower::TYPE type);
        slave_ptr<GameObject> createObject(gameobject::OBJECT_TYPE type);
        void destroyTower(slave_ptr<Tower> tower);
        void clearTowers();
        std::vector<slave_ptr<Tower>>& getTowers() const;

        //Unit Methods
        slave_ptr<Unit> createUnit(unit::TYPE type);
        void destroyUnit(slave_ptr<Unit>& unit);
        std::vector<slave_ptr<Unit>> getUnits() const;

        //Game Controller Methods (World Logic)
        //slave_ptr<GameObject> createObject(gameobject::OBJECT_TYPE type);
        void destroyObject(slave_ptr<GameObject>& obj);


        //GameObject Methods
        void destroy(GameObject* obj);

        //Network Methods
        void sendAllInstancesToClient(network::NetworkClient *network_client);

        //void sendEvent();
        //void receiveEvent();

        //Manager Methods
        void init() const;
        void initRenderManager(RenderManager &rm);
        bool render() const;
        RenderManager*   getRenderManager() const;
        ResourceManager* getResourceManager() const;
        AudioManager*    getAudioManager() const;
        void release();
        void renderAll();
        void stepAll();
        void collisionAll();


        bool step();
        void restart();
    };
}

#endif //MANAGER_H
