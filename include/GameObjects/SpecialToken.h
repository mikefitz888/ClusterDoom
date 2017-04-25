#ifndef SPECIAL_TOKEN_H
#define SPECIAL_TOKEN_H

#include "../gamecore.h"
#include "../../include/gameobject.h"
#include "../../include/smartpointers.h"

class SpecialToken : public GameObject {
    tower::SPECIAL_TYPE type;

public:
    // Constructor
    SpecialToken(id_t id, manager::Manager* m, tower::SPECIAL_TYPE type);

    // Virtual overloads
    virtual void init() override;
    virtual void render() override;
    virtual void renderGUI() override;
    virtual void release() override;
    virtual void step() override;
};

#endif
