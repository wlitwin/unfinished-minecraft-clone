#pragma once

#include <glm/glm.hpp>

struct Aabb
{
    glm::vec3 min = glm::vec3(0);
    glm::vec3 max = glm::vec3(0);

	glm::vec3 center() const;
	glm::vec3 extent() const;
};

struct AabbInt
{
    glm::ivec3 min = glm::ivec3(0);
    glm::ivec3 max = glm::ivec3(0);

	glm::ivec3 center() const;
	glm::ivec3 extent() const;

    bool operator==(const AabbInt&) const;
};

Aabb aabb_union(const Aabb&, const Aabb&);
bool inside_aabb(const Aabb& aabb, const glm::vec3& p);
bool inside_aabb_exclusive(const Aabb& aabb, const glm::vec3& p);
bool inside_aabb_int_exclusive(const AabbInt& aabb, const glm::ivec3& p);

