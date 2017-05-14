#ifndef _UTIL_H
#define _UTIL_H
#include "gamecore.h"

/*
    Provides a couple of useful functions for getting the angle to a target; used for making units face their target.
*/

float point_direction(glm::vec2 point_a, glm::vec2 point_b);
float point_direction_d(glm::vec2 point_a, glm::vec2 point_b);

#endif