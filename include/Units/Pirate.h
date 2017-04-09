#ifndef PIRATE_H
#define PIRATE_H

#include "../gamecore.h"

#include "../tower.h"
#include "../unit.h"

namespace unit
{
    class Pirate : public Unit {
        graphics::AnimatedTexture* texture = nullptr;

        int network_update_position_timer_max = 300;
        int network_update_position_timer = network_update_position_timer_max;

        bool pressed = false; //TEMP;
        float animation_progress = 0.0f;
        Point<float> magneticVelocity = Point<float>(0, 0);

    public:
        Pirate(id_t key, Manager* m);
        void init() override;
        void render() override;
        void onCollision(gameobject_ptr other) override;
        void step() override;

		// Networking
		/*
			The BasicUnit will have a number of simple network events.
		*/
		//enum BasicUnitUpdateEvents { SEND_POSITION, SEND_HEALTH };
		//enum BasicUnitInteractionEvents { SET_TARGET };

		//void writeNetworkUpdate(int event_id, Buffer &buffer) override;
		//void recvNetworkInteraction(int event_id, Buffer &buffer) override;
    };
}

#endif //PIRATE_H