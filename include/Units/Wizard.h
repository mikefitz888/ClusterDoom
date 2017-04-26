#ifndef WIZARD_H
#define WIZARD_H

#include "../gamecore.h"

#include "../tower.h"
#include "../unit.h"

namespace unit
{
    class Wizard : public Unit {
        graphics::AnimatedTexture* texture = nullptr;

        int network_update_position_timer_max = 300;
        int network_update_position_timer = network_update_position_timer_max;

        bool channeling = false;
        int channel_time = 200;
        int channel_cooldown = 150; //150

        bool pressed = false; //TEMP;
        float animation_progress = 0.0f;

        glm::vec2 adjust_max = glm::vec2(10.f, 10.f);
        std::vector<glm::vec2> adjust;

    public:
        Wizard(id_t key, Manager* m);
        ~Wizard();
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

#endif //WIZARD_H