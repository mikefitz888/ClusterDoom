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

    // Currency
    int currency = 0;
    int max_currency = 0;

    // Network instance handling
    NetworkClient* network_client_instance = nullptr;
    int network_player_id = -1;
    bool destroy = false;

    // Networking and events
    int send_currency_timer_max = 15;
    int send_currency_timer = send_currency_timer_max;

    // Currency rewarding
    int currency_reward_timer_max = 10;
    int currency_reward_timer = currency_reward_timer_max;

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

    // Networking
    void writeNetworkUpdate(int event_id, Buffer &buffer) override;

    enum PlayerNetworkEvents : unsigned int{
        SEND_CURRENCY=0x00
    };
};


#endif