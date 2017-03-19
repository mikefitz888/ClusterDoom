#ifndef _GAME_STATE_NOTIFIER_H
#define _GAME_STATE_NOTIFIER_H
/*
    Game State Notifier
    ----------------------

    The purpose of this object is to send information regarding the game-state over the network. This information
    is pulled from gamecontroller and this object is simply a wrapper which makes use of the cleanly handled
    gameobject INetworkInstance interface.

    The object does not currently draw anything on the main game, but exists in the object list in order to 
    benefit from the systems behaviour.

    Examples of sent information:
    - Game alerts (to network connected display + clients)
    - Next wave timer
    - Available wealth (To display the cost of available wealth on the board for the defenders, so they can see the number).
*/

#include "../gamecore.h"
#include "../../include/gameobject.h"
#include "../../include/smartpointers.h"

class GameStateNotifier : public GameObject {

    // PACKET TIMERS
    // Wave timer
    sf::Time current_time;
    unsigned int wave_timer = 0;
    unsigned int last_send_time = 0;
    int send_wave_timer = 0;
    int send_wave_timer_max = 300; // Send the wave timer update every 5 seconds (we can force send by setting timer to 0).

    // Warnings timer
    int last_warnings_list_length = 0; // Set this to the warnings list length so we know when new warnings have come in
    std::string last_warning = "";

    // Available wealth
    int send_wealth_timer = 0;
    int send_wealth_timer_max = 30; // Send wealth every half second

public:
    // Constructor
    GameStateNotifier(id_t id, manager::Manager* m);

    // Virtual overloads
    virtual void init() override;
    virtual void render() override;
    virtual void renderGUI() override;
    virtual void release() override;
    virtual void step() override;

    // NETWORK EVENTS
    enum GameStateNotifierUpdateEvents { SEND_WAVE_TIMER = 0, SEND_WARNINGS = 1, SEND_WEALTH = 2 };
    void writeNetworkUpdate(int event_id, Buffer &buffer) override;
};



#endif