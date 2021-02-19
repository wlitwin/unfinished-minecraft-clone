#pragma once

#include <glm/glm.hpp>

struct Ray
{
    glm::vec3 origin;
    glm::vec3 dir;
    glm::vec3 dir_inv;

    Ray(const glm::vec3& origin, const glm::vec3& dir);
};

struct RayResult
{
    Ray ray;
    float tmin = 0;
    float tmax = 0;
    bool intersected = false;

    RayResult(const Ray& ray);
};
