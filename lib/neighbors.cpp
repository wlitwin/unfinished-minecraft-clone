#include "neighbors.hpp"

const std::array<glm::ivec3, 6> neighbors = {
    glm::ivec3(-1, 0,  0), // Left
    glm::ivec3( 1, 0,  0), // Right
    glm::ivec3( 0, 1,  0), // Top
    glm::ivec3( 0,-1,  0), // Bottom
    glm::ivec3( 0, 0, -1), // Front
    glm::ivec3( 0, 0,  1), // Back
};

glm::ivec3 neighbor_offset(const Neighbor n)
{
    switch (n)
    {
        case Neighbor::Left:   return neighbors[0];
        case Neighbor::Right:  return neighbors[1];
        case Neighbor::Top:    return neighbors[2];
        case Neighbor::Bottom: return neighbors[3];
        case Neighbor::Front:  return neighbors[4];
        case Neighbor::Back:   return neighbors[5];
        default: assert(false);
    }
    return glm::ivec3(0);
}
