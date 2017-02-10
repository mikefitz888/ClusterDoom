#ifndef BASICUNIT_H
#define BASICUNIT_H

#include "../gamecore.h"

#include "../tower.h"
#include "../unit.h"
#include "../gameobject.h"

namespace unit
{
    class BasicUnit : public Unit {
        graphics::Texture* texture = nullptr;

        int network_update_position_timer_max = 20;
        int network_update_position_timer = network_update_position_timer_max;

    public:
        BasicUnit(id_t key, Manager* m);
        void init() override;
        void render() override;
        //void step();
        void onCollision(gameobject_ptr other) override;

		// Networking
		/*
			The BasicUnit will have a number of simple network events.
		*/
		enum BasicUnitUpdateEvents { SEND_POSITION, SEND_HEALTH };
		enum BasicUnitInteractionEvents { SET_TARGET };

		void writeNetworkUpdate(int event_id, Buffer &buffer) override;
		void recvNetworkInteraction(int event_id, Buffer &buffer) override;
    };
}

#endif //BASICUNIT_H