#pragma once

#include <memory>
#include <glm/glm.hpp>

enum class Neighbor : uint8_t
{
    Left=0x1,
    Right=0x2,
    Top=0x4,
    Bottom=0x8,
    Front=0x10,
    Back=0x20,
};

#define OPP_NEIGHBOR(x) ((((x) & 0b00010101) << 1) | (((x) & 0b00101010) >> 1))

extern const std::array<glm::ivec3, 6> neighbors;
glm::ivec3 neighbor_offset(const Neighbor n);
