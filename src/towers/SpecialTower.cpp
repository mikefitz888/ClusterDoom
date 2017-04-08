#include "../../include/Towers/SpecialTower.h"
#include "../../include/Units/BasicUnit.h"
#include "../../include/RenderUtils.h"
#include "../../include/ResourceManager.h"
#include "../../include/manager.h"

namespace tower {
    const double SpecialTower::GM = 3000;
    const double SpecialTower::km = 0.001;
    const double SpecialTower::dt = 10;
    const double SpecialTower::max_range = 225;
    const double SpecialTower::GM_neut = GM / (SpecialTower::max_range*SpecialTower::max_range);

    SpecialTower::SpecialTower(id_t key, Manager* m) : Tower(key, TYPE::SPECIAL, m) {
        effect = &SpecialTower::noeffect;
    }

    void SpecialTower::init() {

        Tower::init();

        render_manager = manager->getRenderManager();
        game_controller = manager->getGameController();
        texture = manager->getResourceManager()->getTexture("yellowTower");
    }

    void SpecialTower::render() {
        Tower::render();
        texture->render(getXr(), getYr(), 96, 96);
    }

    void SpecialTower::step() {
        (this->*effect)();
    }

    void SpecialTower::changeEffect(SPECIAL_TYPE newtype)
    {
        switch (newtype)
        {
        case SPECIAL_TYPE::NOEFFECT:
            effect = &SpecialTower::noeffect;
            break;
        case SPECIAL_TYPE::MAGNETIC:
            effect = &SpecialTower::magnetic;
            break;
        case SPECIAL_TYPE::GLACIAL:
            effect = &SpecialTower::glacial;
            break;
        case SPECIAL_TYPE::WINDY:
            effect = &SpecialTower::windy;
            break;
        }
    }

    void SpecialTower::noeffect() {}
    
    void SpecialTower::magnetic() 
    {
        // d(v, s)/dt = (GM/(s^2) - GM_neut - km*v^2)*dt
        for (auto unit : manager->getUnits())
        {
            if (unit->getSubType() == unit::TYPE::BASIC)
            {
                auto robot = smartpointers::static_pointer_cast<unit::BasicUnit>(unit);
                double d = robot->distanceTo(getPosition());
                Point<float> v = robot->getMagneticVelocity();
                if (d < max_range)
                {
                    // This robot must be accelerated towards us!
                    double v_mag = std::hypot(v.x, v.y);
                    double attraction = GM/(d * d) - GM_neut;
                    double resistance = km*v_mag*v_mag;
                    double acceleration = (attraction - resistance)*dt;
                    double x = robot->getPosition().x;
                    double y = robot->getPosition().y;
                    double theta = std::atan2(y - getPosition().y, x - getPosition().x);
                    // This ensures that the velocity never switches direction, which is very important;
                    // Sometimes the dynamics get a little iffy and this could otherwise result in a 
                    // robot flying off at quite a fast speed. (air resistance never works in reverse!)
                    if (v_mag - acceleration < 0) acceleration = -v_mag;
                    double du = acceleration * std::cos(theta);
                    double dv = acceleration * std::sin(theta);
                    double u_ = v.x + du;
                    double v_ = v.y + dv;
                    // This ensures that a robot never slides PAST a tower, it just stops when it comes 
                    // into contact with it. prevents endless oscillation
                    if (d - std::hypot(u_, v_) < 0)
                    {
                        u_ = d * std::cos(theta);
                        v_ = d * std::sin(theta);
                    }
                    // Adjust the robot's position and then store back the velocity
                    robot->setPosition(glm::vec2(x - u_, y - v_));
                    robot->setMagneticVelocity(Point<float>((float) u_, (float) v_));
                }
                else if (v != Point<float>(0.0f, 0.0f))
                {
                    // A robot has magnetic velocity but isn't in range, we ought to slow it down (in the direction it is travelling!)
                    double v_mag = std::hypot(v.x, v.y);
                    double slowdown = GM/(d * d) - GM_neut; // always negative
                    double resistance = km*v_mag*v_mag;
                    double acceleration = (slowdown - resistance)*dt;
                    double theta = std::atan2(v.x, v.y);
                    // This ensures that the velocity never switches direction, which is very important;
                    // Sometimes the dynamics get a little iffy and this could otherwise result in a 
                    // robot flying off at quite a fast speed. (air resistance never works in reverse!)
                    if (v_mag - acceleration < 0) acceleration = -v_mag;
                    double du = acceleration * std::cos(theta);
                    double dv = acceleration * std::sin(theta);
                    double u_ = v.x + du;
                    double v_ = v.y + dv;
                    // Assume there is no tower, so no need to correct distance
                    robot->setPosition(glm::vec2(robot->getPosition().x - u_, robot->getPosition().y - v_));
                    robot->setMagneticVelocity(Point<float>((float) u_, (float) v_));
                }
            }
        }
    }
    
    void SpecialTower::glacial() 
    {

    }
    
    void SpecialTower::windy() 
    {

    }
}