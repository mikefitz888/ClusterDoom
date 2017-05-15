#ifndef SPECIAL_TOWER_H
#define SPECIAL_TOWER_H

#include "../gamecore.h"
#include "../tower.h"

namespace tower
{
    /*
      This class represents every type of special tower.
      The towers are distinguished by use of the decorator pattern,
      realised in a functional fashion using function pointers.

      However, to fix a bug where the effect would be wiped if
      the tower was covered up and therefore removed from play,
      the effect decorators are now static and the special tower
      is singleton.
    */
    class SpecialTowerMagnetEffect {

        // Visual variables
        const float min_scale_threshold = 0.25f;
        const float min_reset_scale = -0.6f;
        float scale = 1.0f;
        float alpha = 0.0f;

    public:
        SpecialTowerMagnetEffect();
        void step();
        void setScale(float scale);
        float getScale();
        float getAlpha();
    };

    class SpecialTower : public Tower {
        graphics::Texture* texture = nullptr;

        typedef void (SpecialTower::*Effect)(bool);

        // Effect textures
        float ticks_blend = 0.0f;

        // Ice effect ---------
        graphics::Texture* tx_ice_sweep = nullptr;
        graphics::Texture* tx_ice_base = nullptr;
        float ef_ice_sweep_angle = 0.0f;
        float ef_pulse_alpha = 1.0f;
        float ice_scale = 0.75f;
        float ef_ice_alpha = 0.0f;
        // ----------------------

        // Magnet effect --------
        static const int num_magnet_rings = 4;
        SpecialTowerMagnetEffect magnet_rings[num_magnet_rings];
        float ef_magnet_alpha = 0.0f;

        graphics::Texture* tx_magnet_ring = nullptr;
        // ----------------------

        // Wind effect ------
        graphics::Texture* tx_wind_effect = nullptr;
        float ef_wind_alpha = 0.0f;
        float ef_wind_rotation = 0.0f;
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
        static Effect effect;
        void noeffect(bool cancel);
        static unsigned int ticks;
        static SPECIAL_TYPE effectType;

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
        // Slowdown for Robot
        static const double ROBOT_MODIFIER;
        // Slowdown for Wizard
        static const double WIZARD_MODIFIER;
        // Slowdown for Pirate
        static const double PIRATE_MODIFIER;
        // Time before natural thaw
        static const unsigned int GLACIAL_TICKS;

        // WINDY EFFECTS
        void windy(bool cancel);
        static const double MAX_RANGE_WINDY;

    public:
        SpecialTower(id_t key, Manager* m);
        ~SpecialTower();
        void init() override;
        void render() override;
        void step() override;
        
        void changeEffect(SPECIAL_TYPE newtype);
    };
}

#endif //SPECIAL_TOWER_H