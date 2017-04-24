#ifndef SPECIAL_TOWER_H
#define SPECIAL_TOWER_H

#include "../gamecore.h"
#include "../tower.h"

namespace tower
{
    class SpecialTower : public Tower {
        graphics::Texture* texture = nullptr;

        // Effect textures

        // Ice effect ---------
        graphics::Texture* tx_ice_sweep = nullptr;
        graphics::Texture* tx_ice_base = nullptr;
        float ef_ice_sweep_angle = 0.0f;
        float ef_pulse_alpha = 1.0f;
        float ice_scale = 0.75f;
        float ef_ice_alpha = 0.0f;
        // ----------------------

       
        /*********************************************\
        /****************** EFFECTS ******************\
        /*********************************************\
        / The effect of a special tower is governed   \
        / by a pointer to one of its methods. That    \
        / method will, on every step, be called in    \
        / order to perform the desired effect. In     \
        / order to keep things simple, we will carry  \
        / around the state for all the different      \
        / effects, resetting them on a state change.  \
        /*********************************************/
        void (SpecialTower::*effect)(bool);
        void noeffect(bool cancel);
        unsigned int ticks;
        SPECIAL_TYPE effectType;

        // The number of frames a power is active for after it is acquired
        static const unsigned int POWER_TIME;

        SPECIAL_TYPE getEffectType();

        // MAGNETIC EFFECTS
        void magnetic(bool cancel);
        // Mass of tower times Magnetic Constant (technically gravitation)
        static const double GM;
        // Constant of air resistance over mass of robot
        static const double km;
        // Timestep, or the multiplier of acceleration
        static const double dt;
        // Longest distance that the tower has effect at  
        static const double MAX_RANGE_MAGNETIC;
        // GM/(max_range^2), used to ensure no positive force past range
        static const double GM_neut;
        // Damage caused by tower (inversely proportional to square of distance)
        static const double PASSIVE_DAMAGE;
        static double angle(double x, double y, double u, double v);

        // GLACIAL EFFECTS
        void glacial(bool cancel);
        // Radius of effect
        static const double MAX_RANGE_GLACIAL;
        static const double ROBOT_MODIFIER;
        static const double WIZARD_MODIFIER;
        static const double PIRATE_MODIFIER;

        // WINDY EFFECTS
        void windy(bool cancel);

    public:
        SpecialTower(id_t key, Manager* m);
        void init() override;
        void render() override;
        void step() override;
        
        void changeEffect(SPECIAL_TYPE newtype);
    };
}

#endif //SPECIAL_TOWER_H