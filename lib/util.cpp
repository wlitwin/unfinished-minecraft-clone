#include "util.hpp"

#include <iostream>
#include <string>
#include <cassert>
#include <GLFW/glfw3.h>

glm::vec3 min3(const glm::vec3& a, const glm::vec3& b)
{
    return glm::vec3(
            std::min(a.x, b.x),
            std::min(a.y, b.y),
            std::min(a.z, b.z));
}

glm::vec3 max3(const glm::vec3& a, const glm::vec3& b)
{
    return glm::vec3(
            std::max(a.x, b.x),
            std::max(a.y, b.y),
            std::max(a.z, b.z)
            );
}

float randf()
{
    return float(rand()) / INT_MAX;
}

bool checkGLErrorsImpl()
{
    const auto perr = [](const std::string& s) {
        std::cerr << "GL error " << s << std::endl;
    };

    GLenum error = glGetError();
    if (error == GL_NO_ERROR)
    {
        return false;
    }

    switch (error)
    {
    case GL_NO_ERROR: break;
    case GL_INVALID_ENUM: perr("Invalid enum"); break;
    case GL_INVALID_VALUE: perr("Invalid value"); break;
    case GL_INVALID_OPERATION: perr("Invalid operation"); break;
    case GL_INVALID_FRAMEBUFFER_OPERATION: perr("Invalid framebuffer operation"); break;
    case GL_OUT_OF_MEMORY: perr("Invalid out of memory"); break;
    case GL_STACK_UNDERFLOW: perr("Invalid stack underflow"); break;
    case GL_STACK_OVERFLOW: perr("Invalid stack overflow"); break;
    default: perr("Unknown"); break;
    }

    return true;
}

void timeit_noprint(float& outTime, const std::function<void()>& fun, bool ms)
{
    const double start = glfwGetTime();
    fun();
    const double dt = glfwGetTime() - start;
    if (ms) { outTime = dt*1000.0f; }
    else { outTime = dt; }
}

void timeit(const std::string& msg, const std::function<void()>& fun, bool ms)
{
    const double start = glfwGetTime();
    fun();
    const double dt = glfwGetTime() - start;
    if (ms) printf("%s took %fms\n", msg.c_str(), dt*1000.0);
    else printf("%s took %fs\n", msg.c_str(), dt);
}

bool ge3(const glm::ivec3& v1, const glm::ivec3& v2)
{
    return v1.x >= v2.x && v1.y >= v2.y && v1.z >= v2.z;
}

bool lt3(const glm::ivec3& v1, const glm::ivec3& v2)
{
    return v1.x < v2.x && v1.y < v2.y && v1.z < v2.z;
}
