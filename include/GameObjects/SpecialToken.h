#ifndef SPECIAL_TOKEN_H
#define SPECIAL_TOKEN_H

#include "../gamecore.h"
#include "../../include/gameobject.h"

/*
  This class represents the tokens available on the map
  that provide the special effects for the Special Tower.
  It's a relatively standard GameObject, the step method
  checks for nearby special towers to pick it up.
*/
class SpecialToken : public GameObject {
    gameobject::OBJECT_TYPE type;
    graphics::Texture* tokenTexture;

    static const int MAX_PICKUP_RANGE;

protected:

	int maxCooldown = 1000;
	size_t cooldown = 0;

public:
    // Constructor
    SpecialToken(id_t id, manager::Manager* m, gameobject::OBJECT_TYPE type);

    // Virtual overloads
    virtual void init() override;
    virtual void render() override;
    virtual void renderGUI() override;
    virtual void release() override;
    virtual void step() override;
};

#endif
