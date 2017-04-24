#include "../../include/Towers/SpecialTower.h"
#include "../../include/Units/BasicUnit.h"
#include "../../include/RenderUtils.h"
#include "../../include/ResourceManager.h"
#include "../../include/manager.h"

namespace tower {
    const double SpecialTower::GM = 5000;
    const double SpecialTower::km = 0.1;
    const double SpecialTower::dt = 1;
    const double SpecialTower::MAX_RANGE_MAGNETIC = 225;
    const double SpecialTower::GM_neut = GM / (SpecialTower::MAX_RANGE_MAGNETIC*SpecialTower::MAX_RANGE_MAGNETIC);
    const double SpecialTower::PASSIVE_DAMAGE = 3;
    const double SpecialTower::MAX_RANGE_GLACIAL = 225;
    const double SpecialTower::ROBOT_MODIFIER = 0.85;
    const double SpecialTower::WIZARD_MODIFIER = 0.5;
    const double SpecialTower::PIRATE_MODIFIER = 0.5;
    const unsigned int SpecialTower::POWER_TIME = 1750;

    SpecialTower::SpecialTower(id_t key, Manager* m) : Tower(key, TYPE::SPECIAL, m) {
        effect = &SpecialTower::noeffect;
        ticks = 0;
        effectType = NOEFFECT;
    }

    void SpecialTower::init() {

        Tower::init();

        render_manager = manager->getRenderManager();
        game_controller = manager->getGameController();
        texture = manager->getResourceManager()->getTexture("yellowTower");
    }

    void SpecialTower::render() {
        Tower::render();
        texture->render((int) getXr(), (int) getYr(), 96, 96);
    }

    void SpecialTower::step() {
        if (ticks > 0)
        {
            if (--ticks == 0) changeEffect(NOEFFECT);
            else (this->*effect)(false);
        }
    }

    void SpecialTower::changeEffect(SPECIAL_TYPE newtype)
    {
        effectType = newtype;
        // Call the cancel version of the current effect before changing
        switch (newtype)
        {
        case SPECIAL_TYPE::NOEFFECT:
            (this->*effect)(true);
            effect = &SpecialTower::noeffect;
            break;
        case SPECIAL_TYPE::MAGNETIC:
            (this->*effect)(true);
            effect = &SpecialTower::magnetic;
            ticks = POWER_TIME;
            break;
        case SPECIAL_TYPE::GLACIAL:
            (this->*effect)(true);
            effect = &SpecialTower::glacial;
            ticks = POWER_TIME;
            break;
        case SPECIAL_TYPE::WINDY:
            (this->*effect)(true);
            effect = &SpecialTower::windy;
            ticks = POWER_TIME;
            break;
        }
    }

    SPECIAL_TYPE SpecialTower::getEffectType() {
        return effectType;
    }

    void SpecialTower::noeffect(bool cancel) {}
    
    //TODO: Adjust strength based on number of robots stuck to us...
    // The number of robots attached is the number of robots whose velocity is 0 but are within the distance?
    void SpecialTower::magnetic(bool cancel)
    {
        // d(v, s)/dt = (GM/(s^2) - GM_neut - km*v^2)*dt
        int num_robots = 0;
        double sum_dist = 0.0;
        for (auto unit : manager->getUnits())
        {
            if (!unit) continue;
            double dx = unit->getPosition().x - getPosition().x;
            double dy = unit->getPosition().y - getPosition().y;
            double d = std::hypot(dx, dy);
            if (unit->getSubType() == unit::TYPE::BASIC && d < MAX_RANGE_MAGNETIC)
            {
                num_robots++;
                sum_dist += d;
            }
        }
        //std::cout << "tower at (" << getPosition().x << ", " << getPosition().y << ") " << num_robots << " " << sum_dist << std::endl;
        for (auto unit : manager->getUnits())
        {
            if (!unit) continue;
            if (unit->getSubType() == unit::TYPE::BASIC)
            {
                auto robot = smartpointers::static_pointer_cast<unit::BasicUnit>(unit);
                double x = robot->getX();
                double y = robot->getY();
                double dx = x - getX();
                double dy = y - getY();
                double d = std::hypot(dx, dy);
                Point<float> v = robot->getMagneticVelocity();
                if (d < MAX_RANGE_MAGNETIC && !cancel)
                {
                    // This robot must be accelerated towards us!
                    double v_mag = std::hypot(v.x, v.y);
                    double attraction = GM / (d * d);// -GM_neut;
                    double resistance = km*v_mag*v_mag;
                    double multiplier = 1.0/(double)num_robots;// (1 - d / sum_dist) / (num_robots == 1 ? num_robots : num_robots - 1);
                    //std::cout << attraction << ", " << resistance << ", " << multiplier << std::endl;
                    double acceleration = (attraction - resistance)*dt*multiplier;
                    double theta = std::atan2(dy, dx);
                    //std::cout << "robot #" << robot->getID() << " at position (" << x << ", " << y << ") angled at " << theta << " is distance " << d << std::endl;
                    double du = acceleration * std::cos(theta);
                    double dv = acceleration * std::sin(theta);
                    //std::cout << "acceleration: " << acceleration << " v(" << v.x << ", " << v.y << ") " << v_mag << std::endl;
                    double u_ = v.x + du;
                    double v_ = v.y + dv;
                    //std::cout << "u: " << u_ << " v: " << v_ << std::endl;
                    // This ensures that the velocity never switches direction, which is very important;
                    // Sometimes the dynamics get a little iffy and this could otherwise result in a 
                    // robot flying off at quite a fast speed. (air resistance never works in reverse!)
                    // basically, the velocity should always get closer in angle to the tower
                    //if (angle(dx, dy, u_, v_) >= angle(dx, dy, v.x, v.y))
                    //{
                    //    u_ = 0;
                   //     v_ = 0;
                    //}                    
                    // This ensures that a robot never slides PAST a tower, it just stops when it comes 
                    // into contact with it. prevents endless oscillation
                    // Robots should only ever get closer to the tower?
                    if (std::hypot(dx - u_, dy - v_) > d)
                    {
                        double magv_ = std::hypot(u_, v_);
                        u_ = magv_ * std::cos(theta);
                        v_ = magv_ * std::sin(theta);
                    }
                    if (d - std::hypot(u_, v_) < 0)
                    {
                        u_ = d * std::cos(theta);
                        v_ = d * std::sin(theta);
                    }
                    // Adjust the robot's position and then store back the velocity
                    //std::cout << "robot #" << robot->getID() << " moved to (" << x - u_ << ", " << y - v_ << ")" << std::endl;
                    robot->setPosition((float) (x - u_), (float) (y - v_));
                    robot->setMagneticVelocity(Point<float>((float) u_, (float) v_));
                    robot->attacked(self, (float) std::max(PASSIVE_DAMAGE/(d*d), PASSIVE_DAMAGE));
                }
                else if (v != Point<float>(0.0f, 0.0f))
                {
                    // Just halt movement immediately
                    robot->setMagneticVelocity(Point<float>(0.0f, 0.0f));
                    /*
                    //std::cout << "SLOWDOWN for (" << v.x << ", " << v.y << ")" << std::endl;
                    // A robot has magnetic velocity but isn't in range, we ought to slow it down (in the direction it is travelling!)
                    double v_mag = std::hypot(v.x, v.y);
                    double slowdown = GM/(d * d) - GM_neut; // always negative
                    double resistance = km*v_mag*v_mag;
                    double acceleration = (slowdown - resistance)*dt;
                    double theta = std::atan2(v.x, v.y);
                    // This ensures that the velocity never switches direction, which is very important;
                    // Sometimes the dynamics get a little iffy and this could otherwise result in a 
                    // robot flying off at quite a fast speed. (air resistance never works in reverse!)
                    if (v_mag + acceleration < 0) acceleration = -v_mag;
                    double du = acceleration * std::cos(theta);
                    double dv = acceleration * std::sin(theta);
                    double u_ = v.x + du;
                    double v_ = v.y + dv;
                    // Assume there is no tower, so no need to correct distance
                    robot->setPosition((float) (x - u_), (float) (y - v_));
                    robot->setMagneticVelocity(Point<float>((float) u_, (float) v_));
                    */
                }
            }
        }
    }

    double SpecialTower::angle(double x, double y, double u, double v)
    {
        return std::acos((x*u + y*v)/(std::hypot(x, y)*std::hypot(u, v)));
    }
    
    void SpecialTower::glacial(bool cancel)
    {
        for (auto unit : manager->getUnits())
        {
            if (!unit) continue;
            double modifier = 1.0;
            switch (unit->getSubType())
            {
            case unit::TYPE::BASIC:
                modifier = ROBOT_MODIFIER;
                break;
            case unit::TYPE::WIZARD:
                modifier = WIZARD_MODIFIER;
                break;
            case unit::TYPE::PIRATE:
                modifier = PIRATE_MODIFIER;
                break;
            }
            double d = std::hypot(unit->getX() - getX(), unit->getY() - getY());
            unit->setUnderGlacialEffect(d <= MAX_RANGE_GLACIAL && !cancel ? modifier : 1.0);
        }
    }
    
    void SpecialTower::windy(bool cancel)
    {
        /*snip*/
    }
}