#include "ray.hpp"

Ray::Ray(const glm::vec3& origin, const glm::vec3& dir)
    : origin(origin)
    , dir(glm::normalize(dir))
    , dir_inv(1.0f / glm::normalize(dir))
{
}

RayResult::RayResult(const Ray& ray)
    : ray(ray)
{ }
