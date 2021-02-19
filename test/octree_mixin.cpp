#include "catch.hpp"
#include "octree_mixin.hpp"

#include <sstream>
#include <algorithm>

namespace Catch {
    template<>
    struct StringMaker<OctreeMixin::ivec3> {
        static std::string convert( OctreeMixin::ivec3 const& value ) {
            std::stringstream ss;
            ss << "{" << value.x << ", " << value.y << ", " << value.z << "}";
            return ss.str();
        }
    };

    template<>
    struct StringMaker<OctreeMixin::ivec4> {
        static std::string convert( OctreeMixin::ivec4 const& value ) {
            std::stringstream ss;
            ss << "{" << value.x << ", " << value.y << ", " << value.z << ", " << value.w << "}";
            return ss.str();
        }
    };

    /*
    template<>
    struct StringMaker<uint8_t> {
        static std::string convert( uint8_t const& value ) {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }
    };
    */
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

    void print_tree(OctreeMixin& octree)
    {
        octree.traverse([](const OctreeMixin::Node& n) {
            printf("Node %d (%d %d %d) <-> (%d %d %d), children? %d - type %d\n",
                    n.ci.type,
                    n.coord.x,
                    n.coord.y,
                    n.coord.z,
                    n.coord.x + n.coord.w,
                    n.coord.y + n.coord.w,
                    n.coord.z + n.coord.w,
                    n.has_children(),
                    n.ci.type);
        });
        fflush(stdout);
    }

    int count_type(const OctreeMixin::NodeInfoVector& vec, CubeType type)
    {
        int count = 0;
        for (const auto& node : vec)
        {
            if (node->ci.type == type)
            {
                ++count;
            }
        }

        return count;
    }
}

TEST_CASE("Test the coord function", "[Octree coord_of]")
{
    OctreeMixinFinder finder;
    OctreeMixin octree(8*8*8, &finder, &finder);

    cube_info ci;
    ci.type = CubeType::Solid;
    octree.set(OctreeMixin::ivec3(7, 7, 7), ci);
    octree.set(OctreeMixin::ivec3(7, 5, 7), ci);

    const OctreeMixin::Node* n1 = octree.get_node(OctreeMixin::ivec3(7, 7, 7));
    const OctreeMixin::Node* n2 = octree.get_node(OctreeMixin::ivec3(7, 5, 7));

    REQUIRE(n1 != nullptr);
    REQUIRE(n2 != nullptr);

    const auto c1 = octree.coord_of(*n1);
    const auto c2 = octree.coord_of(*n2);

    REQUIRE(OctreeMixin::ivec3(c1) == OctreeMixin::ivec3(7, 7, 7));
    REQUIRE(OctreeMixin::ivec3(c2) == OctreeMixin::ivec3(7, 5, 7));
    REQUIRE(c1.w == 1);
    REQUIRE(c2.w == 1);

    // Make that whole level solid now
    octree.set(OctreeMixin::ivec3(6, 6, 6), ci);
    octree.set(OctreeMixin::ivec3(6, 6, 7), ci);
    octree.set(OctreeMixin::ivec3(6, 7, 6), ci);
    octree.set(OctreeMixin::ivec3(6, 7, 7), ci);
    octree.set(OctreeMixin::ivec3(7, 6, 6), ci);
    octree.set(OctreeMixin::ivec3(7, 6, 7), ci);
    octree.set(OctreeMixin::ivec3(7, 7, 6), ci);
    octree.set(OctreeMixin::ivec3(7, 7, 7), ci);

    const OctreeMixin::Node* n3 = octree.get_node(OctreeMixin::ivec3(6, 7, 6));

    REQUIRE(n3 != nullptr);

    const auto c3 = octree.coord_of(*n3);

    REQUIRE(OctreeMixin::ivec3(c3) == OctreeMixin::ivec3(6, 6, 6));
    REQUIRE(c3.w == 2);
}

TEST_CASE("Octree Mixin insert one cube", "[Octree Insert]")
{
    OctreeMixinFinder finder;
    OctreeMixin octree(2*2*2, &finder, &finder);

    cube_info ci;
    ci.type = CubeType::Solid;
    octree.set(OctreeMixin::ivec3(1, 1, 1), ci);
    octree.set(OctreeMixin::ivec3(0, 0, 0), ci);

    REQUIRE(octree.get(OctreeMixin::ivec3(0)) == ci);
    REQUIRE(octree.get(OctreeMixin::ivec3(1)) == ci);

    const int vis_1 = octree.visibility(OctreeMixin::ivec3(0));
    const int vis_2 = octree.visibility(OctreeMixin::ivec3(1));

    REQUIRE(vis_1 == 0x26);
    REQUIRE(vis_2 == 0x19);

    octree.set(OctreeMixin::ivec3(0, 0, 1), ci);
    octree.set(OctreeMixin::ivec3(0, 1, 0), ci);
    octree.set(OctreeMixin::ivec3(0, 1, 1), ci);
    octree.set(OctreeMixin::ivec3(1, 0, 0), ci);
    octree.set(OctreeMixin::ivec3(1, 0, 1), ci);
    octree.set(OctreeMixin::ivec3(1, 1, 0), ci);
    
    OctreeMixin::NodeInfoVector vec;
    octree.get_bounds_info(vec);

    REQUIRE(vec.size() == 1);
    REQUIRE(vec[0]->coord == OctreeMixin::ivec4(0, 0, 0, 2));

    const int vis_0 = octree.visibility(OctreeMixin::ivec3(0));
    REQUIRE(vis_0 == 0);
}

TEST_CASE("Octree Mixin insert a couple cubes", "[Octree Insert]")
{
    OctreeMixinFinder finder;
    OctreeMixin octree(4*4*4, &finder, &finder);

    cube_info ci;
    ci.type = CubeType::Solid;

    octree.set(OctreeMixin::ivec3(0, 0, 0), ci);
    octree.set(OctreeMixin::ivec3(0, 0, 1), ci);
    octree.set(OctreeMixin::ivec3(0, 1, 0), ci);
    octree.set(OctreeMixin::ivec3(0, 1, 1), ci);
    octree.set(OctreeMixin::ivec3(1, 0, 0), ci);
    octree.set(OctreeMixin::ivec3(1, 0, 1), ci);
    octree.set(OctreeMixin::ivec3(1, 1, 0), ci);
    octree.set(OctreeMixin::ivec3(1, 1, 1), ci);
    
    OctreeMixin::NodeInfoVector vec;
    octree.get_bounds_info(vec);

    REQUIRE(vec.size() == 9);
    REQUIRE(vec[0]->coord == OctreeMixin::ivec4(0, 0, 0, 4));
    REQUIRE(vec[1]->coord == OctreeMixin::ivec4(0, 0, 0, 2));
    REQUIRE(vec[1]->ci == ci);

    cube_info air;
    for (int i = 2; i < 9; ++i)
    {
        REQUIRE(vec[i]->ci == air);
    }
}

TEST_CASE("Inserting cubes", "[Insert Cubes]")
{
    OctreeMixinFinder finder;
    OctreeMixin octree(16*16*16, &finder, &finder);

    cube_info ci;
    ci.type = CubeType::Solid;

    for (int x = 0; x < 16; ++x)
    for (int z = 0; z < 16; ++z)
    for (int y = 0; y < 8;  ++y)
    {
        octree.set(OctreeMixin::ivec3(x, y, z), ci); 
    }

    OctreeMixin::NodeInfoVector vec;
    octree.get_bounds_info(vec, true);

    REQUIRE(vec.size() == 8);
    REQUIRE(count_type(vec, CubeType::Air) == 4);
    REQUIRE(count_type(vec, CubeType::Solid) == 4);

    // Adding the same cube again, should do nothing
    vec.clear();
    octree.set(OctreeMixin::ivec3(0), ci);
    octree.get_bounds_info(vec, true);

    REQUIRE(vec.size() == 8);
    REQUIRE(count_type(vec, CubeType::Air) == 4);
    REQUIRE(count_type(vec, CubeType::Solid) == 4);

    // Set an air cube
    vec.clear();
    octree.set(OctreeMixin::ivec3(0, 8, 0), ci);
    //print_tree(octree);
}
