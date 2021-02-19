#include "aabb.hpp"
#include "util.hpp"

glm::vec3 Aabb::extent() const
{
    return (max - min) * 0.5f;
}

glm::vec3 Aabb::center() const
{
    return extent() + min;
}

bool AabbInt::operator==(const AabbInt& other) const
{
    return min == other.min && max == other.max;
}

glm::ivec3 AabbInt::center() const
{
    return extent() + min;
}

glm::ivec3 AabbInt::extent() const
{
    return (max - min) / 2;    
}

Aabb aabb_union(const Aabb& a, const Aabb& b)
{
    return Aabb {
        min3(a.min, b.min),
        max3(a.max, b.max)         
    };
}

bool inside_aabb(const Aabb& aabb, const glm::vec3& p)
{
    return aabb.min.x <= p.x 
           && aabb.max.x >= p.x
           && aabb.min.y <= p.y
           && aabb.max.y >= p.y
           && aabb.min.z <= p.z
           && aabb.max.z >= p.z;
}

bool inside_aabb_exclusive(const Aabb& aabb, const glm::vec3& p)
{
    return aabb.min.x <= p.x 
           && aabb.max.x > p.x
           && aabb.min.y <= p.y
           && aabb.max.y > p.y
           && aabb.min.z <= p.z
           && aabb.max.z > p.z;
}

bool inside_aabb_int_exclusive(const AabbInt& aabb, const glm::ivec3& p)
{
    return aabb.min.x <= p.x 
           && aabb.max.x > p.x
           && aabb.min.y <= p.y
           && aabb.max.y > p.y
           && aabb.min.z <= p.z
           && aabb.max.z > p.z;
}
