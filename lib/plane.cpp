#include "plane.hpp"
#include "aabb.hpp"
#include <cmath>
#include <algorithm>

Plane Plane::normalize(const Plane& plane)
{
    const float mag = std::sqrt(plane.a*plane.a + plane.b*plane.b + plane.c*plane.c);
    Plane p;
    p.a = plane.a / mag;
    p.b = plane.b / mag;
    p.c = plane.c / mag;
    p.d = plane.d / mag;
    return p;
}

bool aabb_outside_plane(const Plane& plane, const Aabb& aabb)
{
    using std::max;
    const float d = max(aabb.min.x*plane.a, aabb.max.x*plane.a)
                  + max(aabb.min.y*plane.b, aabb.max.y*plane.b)
                  + max(aabb.min.z*plane.c, aabb.max.z*plane.c)
                  + plane.d;

    return d <= 0;
}

bool aabb_intersects_plane(const Plane& plane, const Aabb& aabb)
{
    const glm::vec3 pnorm(plane.a, plane.b, plane.c);
    const glm::vec3 center = (aabb.max - aabb.min) * 0.5f;
    const glm::vec3 extents = aabb.max - center;
    const float r = extents.x*fabs(pnorm.x) + extents.y*fabs(pnorm.y) + extents.z*fabs(pnorm.z);
    const float s = glm::dot(pnorm, center) - plane.d;
    return fabs(s) <= r;
}

