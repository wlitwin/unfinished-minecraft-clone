#include "catch.hpp"
#include "octree_mixin.hpp"
#include "intersection_tests.hpp"
#include <cmath>
#include <sstream>

namespace Catch {
    template<>
    struct StringMaker<glm::vec3> {
        static std::string convert( glm::vec3 const& value ) {
            std::stringstream ss;
            ss << "{" << value.x << ", " << value.y << ", " << value.z << "}";
            return ss.str();
        }
    };
}

namespace
{
    class OctreeMixinFinder : public IOctreeMixinFinder, public IOctreeUpdateObserver
    {
    public:
        OctreeMixin* GetNeighbor(Neighbor)
        {
            return nullptr;
        }

        void OctreeUpdated()
        {

        }
    };
}

bool intersect(Ray& ray, const OctreeMixin& octree, float& tmin, float& tmax, const OctreeMixin::Node*& node)
{
    const glm::ivec3 iorigin = glm::floor(ray.origin);
    printf("Ray %.20f %.20f %.20f iorigin %d %d %d\n",
            ray.origin.x,
            ray.origin.y,
            ray.origin.z,
            iorigin.x,
            iorigin.y,
            iorigin.z);
    node = octree.get_node(iorigin);
    if (node->ci.type != CubeType::Air)
    {
        printf("Not air, returning\n");
        return true;
    }
    printf("Node coord %d %d %d (%d)\n",
            node->coord.x,
            node->coord.y,
            node->coord.z,
            node->coord.w);

    assert(node != nullptr);

    const glm::vec3 base = node->coord;
    const glm::vec3 size = glm::vec3(node->coord.w);
    const Aabb aabb{base, base+size};
    const bool result = intersect_ray_aabb(aabb, ray, tmin, tmax); 
    printf("Result %d tmax %.20f tmin %.20f\n", result, tmax, tmin);

    if (result)
    {
        //printf("New TMAX %.20f\n", std::nextafter(std::nextafter(tmax, tmax+1), tmax+1));
        ray.origin += ray.dir*(tmax+0.01f);
        return intersect(ray, octree, tmin, tmax, node);
    }

    return false;
}

TEST_CASE("Assert how flooring works", "[AABB Floor]")
{
    REQUIRE(glm::floor(-0.1f) == -1.0f);
}

TEST_CASE("AABB Ray to octree", "[AABB Ray Octree]")
{
    OctreeMixinFinder finder;
    OctreeMixin octree(16*16*16, &finder, &finder);

    cube_info ci;
    ci.type = CubeType::Solid;

    for (int x = 0; x < 16; ++x)
    for (int z = 0; z < 16; ++z)
    for (int y = 0; y <  7; ++y)
    {
        octree.set(glm::ivec3(x, y, z), ci);
    }

    float tmin = 0;
    float tmax = 0;
    Ray ray(glm::vec3(1, 14, 1), glm::vec3(0, -1, 0));
    const OctreeMixin::Node* node = nullptr;
    const bool res = intersect(ray, octree, tmin, tmax, node);

    REQUIRE(node != nullptr);
    REQUIRE(glm::vec3(node->coord) == glm::vec3(1, 6, 1));
    REQUIRE(node->coord.w == 1);
    REQUIRE(res == true);
}

TEST_CASE("Simple AABB Ray intersection +X", "[AABB Ray]")
{
    Aabb aabb{glm::vec3(0), glm::vec3(16)};

    float tmin = 0;
    float tmax = 0;
    Ray ray(glm::vec3(1), glm::vec3(1, 0, 0)); 
    bool result = intersect_ray_aabb(aabb, ray, tmin, tmax); 

    REQUIRE(result == true);
    REQUIRE(tmin == -1);
    REQUIRE(tmax == 15);

    const glm::vec3 point = ray.origin + ray.dir*tmax;
    REQUIRE(point == glm::vec3(16, 1, 1));
}

TEST_CASE("Simple AABB Ray intersection -X", "[AABB Ray]")
{
    Aabb aabb{glm::vec3(0), glm::vec3(16)};

    float tmin = 0;
    float tmax = 0;
    Ray ray(glm::vec3(1), glm::vec3(-1, 0, 0)); 
    bool result = intersect_ray_aabb(aabb, ray, tmin, tmax); 

    REQUIRE(result == true);
    REQUIRE(tmin == -15);
    REQUIRE(tmax == 1);

    const glm::vec3 point = ray.origin + ray.dir*tmax;
    REQUIRE(point == glm::vec3(0, 1, 1));
}

TEST_CASE("Simple AABB Ray intersection +Y", "[AABB Ray]")
{
    Aabb aabb{glm::vec3(0), glm::vec3(16)};

    float tmin = 0;
    float tmax = 0;
    Ray ray(glm::vec3(1), glm::vec3(0, 1, 0)); 
    bool result = intersect_ray_aabb(aabb, ray, tmin, tmax); 

    REQUIRE(result == true);
    REQUIRE(tmin == -1);
    REQUIRE(tmax == 15);

    const glm::vec3 point = ray.origin + ray.dir*tmax;
    REQUIRE(point == glm::vec3(1, 16, 1));
}

TEST_CASE("Simple AABB Ray intersection -Y", "[AABB Ray]")
{
    Aabb aabb{glm::vec3(0), glm::vec3(16)};

    float tmin = 0;
    float tmax = 0;
    Ray ray(glm::vec3(1), glm::vec3(0, -1, 0)); 
    bool result = intersect_ray_aabb(aabb, ray, tmin, tmax); 

    REQUIRE(result == true);
    REQUIRE(tmin == -15);
    REQUIRE(tmax == 1);

    const glm::vec3 point = ray.origin + ray.dir*tmax;
    REQUIRE(point == glm::vec3(1, 0, 1));
}

TEST_CASE("Simple AABB Ray intersection +Z", "[AABB Ray]")
{
    Aabb aabb{glm::vec3(0), glm::vec3(16)};

    float tmin = 0;
    float tmax = 0;
    Ray ray(glm::vec3(1), glm::vec3(0, 0, 1)); 
    bool result = intersect_ray_aabb(aabb, ray, tmin, tmax); 

    REQUIRE(result == true);
    REQUIRE(tmin == -1);
    REQUIRE(tmax == 15);

    const glm::vec3 point = ray.origin + ray.dir*tmax;
    REQUIRE(point == glm::vec3(1, 1, 16));
}

TEST_CASE("Simple AABB Ray intersection -Z", "[AABB Ray]")
{
    Aabb aabb{glm::vec3(0), glm::vec3(16)};

    float tmin = 0;
    float tmax = 0;
    Ray ray(glm::vec3(1), glm::vec3(0, 0, -1)); 
    bool result = intersect_ray_aabb(aabb, ray, tmin, tmax); 

    REQUIRE(result == true);
    REQUIRE(tmin == -15);
    REQUIRE(tmax == 1);

    const glm::vec3 point = ray.origin + ray.dir*tmax;
    REQUIRE(point == glm::vec3(1, 1, 0));
}
