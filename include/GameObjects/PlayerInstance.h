#ifndef _PLAYER_INSTANCE_H
#define _PLAYER_INSTANCE_H
/*
    PLAYER INSTANCE
    ---------------
    A PlayerInstance is the server-sided control object for a networked client.
    This object is responsible for managing the players actions. For example, it 
    keeps track of the server-side currency value, which will be synced with the client
    and also used when spawning units to prevent invalid spawns.

    This object is strictly bound to a NetworkClient instance. When the network client
    gets destroyed, this object is no longer relevant (for that connection) and needs to be removed.

    Current mechanics:
    - Ability to manage currency
    - 
*/
#include "../gamecore.h"
#include "../gameobject.h"
#include "../../include/smartpointers.h"

using network::NetworkClient;

class PlayerInstance : public GameObject {

private:
    int currency = 0;
    NetworkClient* network_client_instance = nullptr;
    int network_player_id = -1;
    bool destroy = false;

public:
    // Constructor
    PlayerInstance(id_t id, manager::Manager* m);

    // Virtual overloads
    virtual void init() override;
    virtual void render() override;
    virtual void renderGUI() override;
    virtual void release() override;
    virtual void step() override;

    // Currency
    bool useCurrency(int amount);
    int  getAvailableCurrency();
    void giveCurrency(int amount);

    // Control and setting
    void setNetworkInstanceID(NetworkClient* client, int network_player_id);
};


#endif