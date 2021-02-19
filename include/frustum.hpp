#pragma once

#include "aabb.hpp"
#include "plane.hpp"

struct frustum_planes
{
    Plane top;
    Plane left;
    Plane bot;
    Plane right;
    Plane near;
    Plane far;
};

bool aabb_outside_frustum(const frustum_planes&, const Aabb&);
bool aabb_intersects_frustum(const frustum_planes&, const Aabb&);
frustum_planes extract_frustum_planes(const glm::mat4& mat, const bool normalize = false);
