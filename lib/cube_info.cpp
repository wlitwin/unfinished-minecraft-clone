#include "cube_info.hpp"

cube_info::cube_info() = default;
cube_info::cube_info(const cube_info&) = default;

cube_info::cube_info(CubeType type)
    : type(type)
{
}

bool cube_info::operator==(const cube_info& other) const
{
    return type == other.type;
}

bool cube_info::operator!=(const cube_info& other) const
{
    return !(*this == other);
}

