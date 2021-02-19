#pragma once

#include <cstdint>

enum CubeType : uint8_t
{
    Air = 0,
    Solid,
    Rock,
};

struct cube_info
{
    cube_info();
    cube_info(const cube_info&);
    cube_info(CubeType type);

    CubeType type = CubeType::Air;

    bool operator==(const cube_info&) const;
    bool operator!=(const cube_info&) const;
};

