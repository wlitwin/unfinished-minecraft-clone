#pragma once

#include <glm/glm.hpp>
#include <string>
#include <functional>

#define checkGLErrors() ((checkGLErrorsImpl() == true) ? assert(false) : assert(true))

struct GLMivec3Hash
{
    std::size_t operator()(const glm::ivec3& key) const
    {
        return ((key.x*73856093) ^ (key.y*19349663) ^ (key.z*83492791)) % 511;
    }
};

float randf();
bool checkGLErrorsImpl();
void timeit(const std::string& msg, const std::function<void()>& fun, bool ms = true);
void timeit_noprint(float& outTime, const std::function<void()>& fun, bool ms = true);
glm::vec3 min3(const glm::vec3& a, const glm::vec3& b);
glm::vec3 max3(const glm::vec3& a, const glm::vec3& b);
bool ge3(const glm::ivec3& v1, const glm::ivec3& v2);
bool lt3(const glm::ivec3& v1, const glm::ivec3& v2);
