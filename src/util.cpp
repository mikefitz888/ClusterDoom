#include "../include/util.h"
float point_direction(glm::vec2 point_a, glm::vec2 point_b) {
    return atan2(point_b.y - point_a.y, point_b.x - point_a.x);
}
float point_direction_d(glm::vec2 point_a, glm::vec2 point_b) {
    return point_direction(point_a, point_b)*180.0f / glm::pi<float>();
}