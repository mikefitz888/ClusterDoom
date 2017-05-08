#ifndef BASICUNIT_H
#define BASICUNIT_H

#include "../gamecore.h"

#include "../tower.h"
#include "../unit.h"

namespace unit
{
    class BasicUnit : public Unit {
        graphics::AnimatedTexture* texture = nullptr;
		graphics::AnimatedTexture* deathTexture = nullptr;
		graphics::AnimatedTexture* attackTexture = nullptr;

        int network_update_position_timer_max = 300;
        int network_update_position_timer = network_update_position_timer_max;

        bool pressed = false; //TEMP;
        Point<float> magneticVelocity = Point<float>(0.0f, 0.0f);

    public:
        BasicUnit(id_t key, Manager* m);
        ~BasicUnit();
        void init() override;
        void render() override;
        void onCollision(gameobject_ptr other) override;
        void step() override;

        Point<float> getMagneticVelocity();
        void setMagneticVelocity(const Point<float> v);

		// Networking
		/*
			The BasicUnit will have a number of simple network events.
		*/
		enum BasicUnitUpdateEvents { SEND_POSITION, SEND_HEALTH };
		enum BasicUnitInteractionEvents { SET_TARGET };

		void writeNetworkUpdate(int event_id, Buffer &buffer) override;
		void recvNetworkInteraction(int event_id, Buffer &buffer, network::NetworkClient* interaction_connection_client) override;
    };
}

#endif //BASICUNIT_H