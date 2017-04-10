#ifndef SPECIAL_TOWER_H
#define SPECIAL_TOWER_H

#include "../gamecore.h"
#include "../tower.h"

namespace tower
{
    enum SPECIAL_TYPE : unsigned int { NOEFFECT = 0, MAGNETIC, GLACIAL, WINDY};

    class SpecialTower : public Tower {
        graphics::Texture* texture = nullptr;
       
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
        void (SpecialTower::*effect)();
        void noeffect();

        // MAGNETIC EFFECTS
        void magnetic();
        // Mass of tower times Magnetic Constant (technically gravitation)
        static const double GM;
        // Constant of air resistance over mass of robot
        static const double km;
        // Timestep, or the multiplier of acceleration
        static const double dt;
        // Longest distance that the tower has effect at  
        static const double MAX_RANGE;
        // GM/(max_range^2), used to ensure no positive force past range
        static const double GM_neut;
        // Damage caused by tower (inversely proportional to square of distance)
        static const double PASSIVE_DAMAGE;
        static double angle(double x, double y, double u, double v);

        // GLACIAL EFFECTS
        void glacial();

        // WINDY EFFECTS
        void windy();

    public:
        SpecialTower(id_t key, Manager* m);
        void init() override;
        void render() override;
        void step() override;
        
        void changeEffect(SPECIAL_TYPE newtype);
    };
}

#endif //SPECIAL_TOWER_H