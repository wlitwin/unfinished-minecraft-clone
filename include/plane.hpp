#pragma once

#include "aabb.hpp"
#include <glm/glm.hpp>

struct Plane
{
    float a = 0;
    float b = 0;
    float c = 0;
    float d = 0;

	static Plane normalize(const Plane& plane);
};

bool aabb_intersects_plane(const Plane& plane, const Aabb& aabb);
bool aabb_outside_plane(const Plane& plane, const Aabb& aabb);
