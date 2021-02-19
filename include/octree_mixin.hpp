#pragma once

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <functional>
#include "cube_info.hpp"
#include "neighbors.hpp"

class OctreeMixin;
class IOctreeMixinFinder
{
public:
    virtual OctreeMixin* GetNeighbor(Neighbor n) = 0;
    virtual ~IOctreeMixinFinder() = default;
};

class IOctreeUpdateObserver
{
public:
    virtual void OctreeUpdated() = 0;
};

// Can change this to glm::i8vec to save lots of space
// as these octrees most likely won't be larger than 256x256x256
// 4+4+4+4 = 12 -> 1+1+1+1 = 4
class OctreeMixin
{
public:
    using BaseType  = int8_t;
    using ivec4 = glm::tvec4<BaseType>;
    using ivec3 = glm::tvec3<BaseType>;

    struct Node
    {
        ivec4 coord = ivec4(0);
        cube_info ci;
        uint8_t info = 0;

        static constexpr uint8_t VISIBILITY = 0x3F;
        static constexpr uint8_t CHILDREN = 0x40;

        bool has_children() const;
        void set_has_children(bool);
        bool is_visible(const Neighbor) const;
        void set_visible(const Neighbor, bool);
        uint8_t visibility() const;
        void clear_visibility();
        ivec3 get_base_coord(Neighbor side) const;
        ivec3 get_neighbor_coord(Neighbor side) const;
    };

    OctreeMixin(int leafCount, IOctreeMixinFinder* finder, IOctreeUpdateObserver* observer);
    bool inBounds(const ivec3&) const;
    void set(const ivec3&, const cube_info&);
    cube_info& get(const ivec3&);
    const Node* get_node(const ivec3&) const;
    Node* get_node(const ivec3&);

    using NodeInfoVector = std::vector<const Node*>;
    void get_bounds_info(NodeInfoVector& info, bool leavesOnly = false) const;
    void traverse(const std::function<void(const Node&)>& f) const;

    std::function<const cube_info&(const ivec3&)> cubeAt;
    ivec4 coord_of(const Node& n) const;

    class Batcher
    {
        OctreeMixin& tree;
        bool update_visibility = false;
        public:
        Batcher(OctreeMixin& tree);
        ~Batcher();
    };

    Batcher batch_set();

    uint8_t visibility(const ivec3&);
private:
    int index_of(const Node& n) const;
    int level_of(const Node& n) const;
    int power_of(const Node& n) const;
    int child_index(const Node& n) const;
    void updateVisibilityMismatchedSizes(
            Node& node,
            OctreeMixin& mixin,
            const Neighbor side,
            const OctreeMixin::ivec3& baseVec,
            const OctreeMixin::ivec3& dim1,
            const OctreeMixin::ivec3& dim2);

    void init_coords();
    
    struct InsertInfo
    {
        BaseType N;
        ivec3 p;
        Node* node;

        InsertInfo(const int dims, const ivec3& p, Node* node);

        void down(std::vector<Node>& nodes);
        uint8_t child_index() const;
    };

    void SetVisibility(Node& n,
                       const Neighbor side,
                       const ivec3& dim1,
                       const ivec3& dim2);
    void collapse(Node& node);
    void updateVisibility(Node&);
    void updateVisibilityOfLeaves(Node& start);
    InsertInfo get_internal(const ivec3&);
    void insert_down(InsertInfo&, const cube_info&);

    struct SameInfo
    {
        Node* p;
        cube_info ci;
        bool same;
    };
    SameInfo children_all_same(const Node& node);

    // TODO - When only AIR can resize m_tree to be 1
    //        and then resize back to save space
    IOctreeMixinFinder* m_finder;
    IOctreeUpdateObserver* m_observer;
    int m_leafCount = 0;
    int m_sideDim = 0;
    int m_offsetToLeaves = 0;
    std::vector<Node> m_tree;
    bool batch_disable_update_visibility = false;
};
