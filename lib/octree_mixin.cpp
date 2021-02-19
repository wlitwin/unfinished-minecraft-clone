#include "octree_mixin.hpp"
#include "chunk_defs.hpp"
#include <cmath>
#include <cassert>
#include <list>

namespace
{
    inline constexpr unsigned char operator "" _i8(unsigned long long arg) noexcept
    {
        return static_cast<int8_t>(arg);
    }

    constexpr int children(int x)
    {
        return x*8 + 1;
    }

    constexpr int parent(int x)
    {
        return (x-1) / 8;
    }

    constexpr int child_num(int x)
    {
        return x - (children(parent(x)));
    }

    constexpr uint8_t child_from_loc(const OctreeMixin::ivec3& loc)
    {
        return (loc.x << 2) | (loc.y << 1) | loc.z;
    }

    constexpr int determineNodeCount(int leafSize)
    {
        int total = leafSize;
        while (leafSize > 1)
        {
            leafSize /= 8;
            total += leafSize;
        }

        return total;
    }

    const OctreeMixin::ivec3 offsets[] = {
        OctreeMixin::ivec3(0, 0, 0),
        OctreeMixin::ivec3(0, 0, 1),
        OctreeMixin::ivec3(0, 1, 0),
        OctreeMixin::ivec3(0, 1, 1),
        OctreeMixin::ivec3(1, 0, 0),
        OctreeMixin::ivec3(1, 0, 1),
        OctreeMixin::ivec3(1, 1, 0),
        OctreeMixin::ivec3(1, 1, 1),
    };

    const OctreeMixin::ivec3 one = OctreeMixin::ivec3(1);
    const OctreeMixin::ivec3 zero = OctreeMixin::ivec3(0);
    const OctreeMixin::ivec3 vec3_x = OctreeMixin::ivec3(1, 0, 0);
    const OctreeMixin::ivec3 vec3_y = OctreeMixin::ivec3(0, 1, 0);
    const OctreeMixin::ivec3 vec3_z = OctreeMixin::ivec3(0, 0, 1);
}

OctreeMixin::ivec3 OctreeMixin::Node::get_base_coord(const Neighbor side) const
{
    switch (side)
    {
    case Neighbor::Left:   return ivec3(coord);
    case Neighbor::Right:  return ivec3(coord) + ivec3(coord.w - 1_i8, 0, 0);
    case Neighbor::Top:    return ivec3(coord) + ivec3(0, coord.w - 1_i8, 0);
    case Neighbor::Bottom: return ivec3(coord);
    case Neighbor::Front:  return ivec3(coord);
    case Neighbor::Back:   return ivec3(coord) + ivec3(0, 0, coord.w - 1_i8);
    default: assert(false);
    }
    return ivec3(0);
}

OctreeMixin::ivec3 OctreeMixin::Node::get_neighbor_coord(const Neighbor side) const
{
    switch (side)
    {
    case Neighbor::Left:   return ivec3(coord) + ivec3(-1_i8, 0, 0);
    case Neighbor::Right:  return ivec3(coord) + ivec3(coord.w, 0, 0);
    case Neighbor::Top:    return ivec3(coord) + ivec3(0, coord.w, 0);
    case Neighbor::Bottom: return ivec3(coord) + ivec3(0, -1_i8, 0);
    case Neighbor::Front:  return ivec3(coord) + ivec3(0, 0, -1_i8);
    case Neighbor::Back:   return ivec3(coord) + ivec3(0, 0, coord.w);
    default: assert(false);
    }
    return ivec3(0);
}

bool OctreeMixin::Node::has_children() const
{
    return (info & CHILDREN) > 0;
}

void OctreeMixin::Node::set_has_children(bool value)
{
    if (value) {
        info |= CHILDREN;
    } else {
        info &= ~CHILDREN;
    }
}

void OctreeMixin::Node::clear_visibility()
{
    info = (info & CHILDREN);
}

uint8_t OctreeMixin::Node::visibility() const
{
    return (info & VISIBILITY);
}

bool OctreeMixin::Node::is_visible(const Neighbor mask) const
{
    return (info & static_cast<uint8_t>(mask)) > 0;
}

void OctreeMixin::Node::set_visible(const Neighbor mask, bool value)
{
    if (value) {
        info |= static_cast<uint8_t>(mask);
    } else {
        info &= ~static_cast<uint8_t>(mask);
    }
}


void OctreeMixin::init_coords()
{
    Node& root = m_tree[0];
    root.coord = ivec4(0, 0, 0, m_sideDim);

    for (size_t i = 1; i < m_tree.size(); i += 8)
    {
        const Node& p = m_tree[parent(i)];
        const BaseType child_level = p.coord.w / 2;
        const ivec3 p_coord(p.coord);
        for (int child = 0; child < 8; ++child)
        {
            Node& child_node = m_tree[i + child];
            child_node.coord = ivec4(p_coord + offsets[child]*child_level, child_level);
        }
    }
}

OctreeMixin::OctreeMixin(int leafCount, IOctreeMixinFinder* finder, IOctreeUpdateObserver* observer)
    : m_finder(finder)
    , m_observer(observer)
    , m_leafCount(leafCount)
    , m_sideDim(std::cbrt(leafCount))
{
    assert(m_sideDim*m_sideDim*m_sideDim == leafCount);
    const int nodeCount = determineNodeCount(leafCount);
    m_offsetToLeaves = nodeCount - leafCount;
    m_tree.resize(nodeCount);

    init_coords();
}

// TODO - figure out coord from array index

int OctreeMixin::level_of(const Node& n) const
{
    int level = 0;
    int index = index_of(n);
    while (index > 0)
    {
        index = parent(index);
        ++level;
    }
    return level;
}

int OctreeMixin::power_of(const Node& n) const
{
    int power = 1;
    int index = index_of(n);
    while (index > 0)
    {
        index = parent(index);
        power *= 2;
    }
    return power;
}

OctreeMixin::ivec4 OctreeMixin::coord_of(const Node& input) const
{
    return input.coord;
}

const OctreeMixin::Node* OctreeMixin::get_node(const OctreeMixin::ivec3& p) const
{
    auto info = const_cast<OctreeMixin*>(this)->get_internal(p);
    return info.node;
}

OctreeMixin::Node* OctreeMixin::get_node(const ivec3& p)
{
    return get_internal(p).node;
}

int OctreeMixin::child_index(const Node& n) const
{
    const int index = index_of(n);
    return index - children(parent(index));
}

bool OctreeMixin::inBounds(const OctreeMixin::ivec3& p) const
{
    return p.x >= 0 
           && p.y >= 0
           && p.z >= 0
           && p.x < m_sideDim
           && p.y < m_sideDim
           && p.z < m_sideDim;
}

namespace
{
    struct NeighborInfo
    {
        OctreeMixin::ivec3 internal;
    };

    /*
    OctreeMixin::ivec3(0, 0, 0), 0
    OctreeMixin::ivec3(0, 0, 1), 1
    OctreeMixin::ivec3(0, 1, 0), 2
    OctreeMixin::ivec3(0, 1, 1), 3
    OctreeMixin::ivec3(1, 0, 0), 4
    OctreeMixin::ivec3(1, 0, 1), 5
    OctreeMixin::ivec3(1, 1, 0), 6
    OctreeMixin::ivec3(1, 1, 1), 7
    */

    const NeighborInfo neighborInfo[8] = {
        NeighborInfo{OctreeMixin::ivec3(1, 2, 4)},
        NeighborInfo{OctreeMixin::ivec3(0, 3, 5)},
        NeighborInfo{OctreeMixin::ivec3(0, 3, 6)},
        NeighborInfo{OctreeMixin::ivec3(1, 2, 7)},
        NeighborInfo{OctreeMixin::ivec3(0, 5, 6)},
        NeighborInfo{OctreeMixin::ivec3(1, 4, 7)},
        NeighborInfo{OctreeMixin::ivec3(2, 4, 7)},
        NeighborInfo{OctreeMixin::ivec3(3, 5, 6)},
    };
}

namespace
{
    const char* SideToStr(const Neighbor side)
    {
        switch (side)
        {
        case Neighbor::Left: return "LEFT";
        case Neighbor::Right: return "RIGHT";
        case Neighbor::Top: return "TOP";
        case Neighbor::Bottom: return "BOTTOM";
        case Neighbor::Front: return "FRONT";
        case Neighbor::Back: return "BACK";
        default: return "UNKN";
        }
    }

    OctreeMixin::ivec3 fixup_neighbor_coord(const OctreeMixin::ivec3& vec, const Neighbor side, const int8_t sideDim)
    {
        using ivec3 = OctreeMixin::ivec3;
        switch (side)
        {
        case Neighbor::Left:   return (vec.x < 0) ? ivec3(sideDim-1, vec.y, vec.z) : vec;
        case Neighbor::Right:  return (vec.x >= sideDim) ? ivec3(0, vec.y, vec.z) : vec;
        case Neighbor::Top:    return (vec.y >= sideDim) ? ivec3(vec.x, 0, vec.z) : vec;
        case Neighbor::Bottom: return (vec.y < 0) ? ivec3(vec.x, sideDim-1, vec.z) : vec;
        case Neighbor::Front:  return (vec.z < 0) ? ivec3(vec.x, vec.y, sideDim-1) : vec;
        case Neighbor::Back:   return (vec.z >= sideDim) ? ivec3(vec.x, vec.y, 0) : vec;
        default: assert(false);
        }
        return ivec3(0);
    }

    Neighbor opposite_side(const Neighbor side)
    {
        switch (side)
        {
        case Neighbor::Left:   return Neighbor::Right;
        case Neighbor::Right:  return Neighbor::Left;
        case Neighbor::Top:    return Neighbor::Bottom;
        case Neighbor::Bottom: return Neighbor::Top;
        case Neighbor::Front:  return Neighbor::Back;
        case Neighbor::Back:   return Neighbor::Front;
        default: assert(false);
        }
        return Neighbor::Left;
    }
}

void OctreeMixin::updateVisibilityMismatchedSizes(
        OctreeMixin::Node& node,
        OctreeMixin& mixin,
        const Neighbor side,
        const OctreeMixin::ivec3& baseVec,
        const OctreeMixin::ivec3& dim1,
        const OctreeMixin::ivec3& dim2)
{
    // Set from our perspective
    const bool isAir = node.ci.type == CubeType::Air;
    const Neighbor oppSide = opposite_side(side);
    const int scale = node.coord.w;

    bool visible = false;
    for (BaseType a = 0; a < scale; ++a)
    for (BaseType b = 0; b < scale; ++b)
    {
        // TODO - Should skip by this node size
        const auto loc = baseVec + a*dim1 + b*dim2;
        const auto n_info = mixin.get_internal(loc);
        const bool neighborIsAir = n_info.node->ci.type == CubeType::Air;
        n_info.node->set_visible(oppSide, !neighborIsAir && isAir);

        visible |= (neighborIsAir && !isAir);
    }
    node.set_visible(side, visible);
}

void OctreeMixin::SetVisibility(Node& n,
                                const Neighbor side,
                                const ivec3& dim1,
                                const ivec3& dim2)
{
    OctreeMixin* mixin = this;
    ivec3 neighbor_coord = n.get_neighbor_coord(side); 
    if (!inBounds(neighbor_coord))
    {
        neighbor_coord = fixup_neighbor_coord(neighbor_coord, side, m_sideDim);
        mixin = m_finder->GetNeighbor(side);
    }

    if (!mixin)
    {
        n.set_visible(side, false);
        return;
    }

    assert(m_sideDim == mixin->m_sideDim);
    const bool isAir = n.ci.type == CubeType::Air;
    const Neighbor oppSide = opposite_side(side);
    const int scale = n.coord.w;
    Node& neighbor_node = *mixin->get_node(neighbor_coord);
    if (neighbor_node.coord.w == scale)
    {
        // Can bail early, same depth size
        const bool neighborIsAir = neighbor_node.ci.type == CubeType::Air;
        neighbor_node.set_visible(oppSide, !neighborIsAir && isAir);
        n.set_visible(side, neighborIsAir && !isAir);
    }
    else if (neighbor_node.coord.w > scale) 
    {
        // If other cube is larger, we need to swap this loop
        const ivec3 ourCoord = neighbor_node.get_neighbor_coord(oppSide);
        const ivec3 baseVec = fixup_neighbor_coord(ourCoord, oppSide, m_sideDim);
        updateVisibilityMismatchedSizes(neighbor_node, *this, oppSide, baseVec, dim1, dim2);
    }
    else
    {
        updateVisibilityMismatchedSizes(n, *mixin, side, neighbor_coord, dim1, dim2);
    }

    if (mixin != this)
    {
        mixin->m_observer->OctreeUpdated();
    }
}

OctreeMixin::Batcher::Batcher(OctreeMixin& tree)
    : tree(tree)
    , update_visibility(tree.batch_disable_update_visibility)
{
    tree.batch_disable_update_visibility = true;
}

OctreeMixin::Batcher::~Batcher()
{
    tree.batch_disable_update_visibility = update_visibility;
    tree.updateVisibilityOfLeaves(tree.m_tree[0]);
}

OctreeMixin::Batcher OctreeMixin::batch_set()
{
    return Batcher(*this);
}

void OctreeMixin::updateVisibilityOfLeaves(Node& n)
{
    if (!n.has_children())
    {
        updateVisibility(n);
        return;
    }

    const int child_offset = children(index_of(n));
    for (int i = 0; i < 8; ++i)
    {
        updateVisibilityOfLeaves(m_tree[child_offset + i]);
    }
}

void OctreeMixin::updateVisibility(Node& n)
{
    if (batch_disable_update_visibility)
    {
        return;
    }

    // Do the inefficient way for now
    SetVisibility(n, Neighbor::Left,   vec3_y, vec3_z);
    SetVisibility(n, Neighbor::Right,  vec3_y, vec3_z);
    SetVisibility(n, Neighbor::Top,    vec3_x, vec3_z);
    SetVisibility(n, Neighbor::Bottom, vec3_x, vec3_z);
    SetVisibility(n, Neighbor::Front,  vec3_x, vec3_y);
    SetVisibility(n, Neighbor::Back,   vec3_x, vec3_y);
    m_observer->OctreeUpdated();
}

uint8_t OctreeMixin::visibility(const OctreeMixin::ivec3& p)
{
    return get_internal(p).node->visibility();
}

uint8_t OctreeMixin::InsertInfo::child_index() const
{
    return child_from_loc(p / N);
}

void OctreeMixin::InsertInfo::down(std::vector<Node>& nodes)
{
    const OctreeMixin::ivec3 loc = p / N;
    const uint8_t child = child_from_loc(loc);
    const int index = node - &nodes[0];
    node = &nodes[children(index) + child];
    p -= N*loc;
    N /= 2;
}

OctreeMixin::InsertInfo::InsertInfo(const int dims, const OctreeMixin::ivec3& p, Node* node)
    : N(dims)
    , p(p)
    , node(node)
{
}

OctreeMixin::InsertInfo
OctreeMixin::get_internal(const OctreeMixin::ivec3& input)
{
    assert(inBounds(input));

    InsertInfo info(m_sideDim/2, input, &m_tree[0]);
    while (info.N > 0 && info.node->has_children())
    {
        info.down(m_tree);
    }

    return info;
}

int OctreeMixin::index_of(const Node& n) const
{
    return (&n - &m_tree[0]);
}

void OctreeMixin::get_bounds_info(std::vector<const OctreeMixin::Node*>& info, bool leavesOnly) const
{
    // TODO make more efficient
    std::list<const Node*> savedIndices;
    savedIndices.push_back(&m_tree[0]);

    while (!savedIndices.empty())
    {
        const Node* node = savedIndices.front();
        savedIndices.pop_front();

        const int nodeIndex = index_of(*node);
        if (node->has_children() == false)
        {
            info.push_back(node);
            continue;
        }

        if (!leavesOnly)
        {
            info.push_back(node);
        }

        assert(nodeIndex < m_offsetToLeaves);

        const int index = children(nodeIndex);
        for (int idx = 0; idx < 8; ++idx)
        {
            savedIndices.push_back(&m_tree[index+idx]);
        }
    }
}

void OctreeMixin::traverse(const std::function<void(const Node&)>& f) const
{
    std::vector<const Node*> nodes;
    get_bounds_info(nodes);
    for (const auto& node : nodes)
    {
        f(*node);
    }
}

OctreeMixin::SameInfo OctreeMixin::children_all_same(const Node& node)
{
    if (index_of(node) <= 0)
    {
        SameInfo si;
        si.same = false;
        return si;
    }

    const int parent_index = parent(index_of(node));
    Node* parent = &m_tree[parent_index];
    assert(parent->has_children());
    const int children_index = children(parent_index);
    const cube_info& ci_0 = m_tree[children_index].ci;
    for (int i = 0; i < 8; ++i)
    {
        const Node& child = m_tree[children_index + i];
        if (ci_0 != child.ci || child.has_children())
        {
            SameInfo si;
            si.same = false;
            return si;
        }
    }

    SameInfo si;
    si.p = parent;
    si.ci = ci_0;
    si.same = true;
    return si;
}

void OctreeMixin::collapse(Node& node)
{
    assert(index_of(node) >= m_offsetToLeaves);

    Node* n = &node;
    SameInfo si = children_all_same(*n);
    while (si.same)
    {
        n = si.p;
        n->clear_visibility();
        n->set_has_children(false);
        n->ci = si.ci;
        si = children_all_same(*n);
    }

    updateVisibility(*n);
}

void OctreeMixin::insert_down(InsertInfo& info, const cube_info& ci)
{
    assert(info.N > 0);
    assert(info.node->has_children() == false);

    while (info.N > 0)
    {
        assert(info.node->ci != ci);
        Node& parent = *info.node;
        parent.set_has_children(true);
        //info.node->visibility = 0xFF;
        const int index = index_of(*info.node);
        const int child_index = children(index);
        for (int i = 0; i < 8; ++i)
        {
            Node& child = m_tree[child_index + i];
            child.set_has_children(false);
            child.ci = parent.ci;
            child.clear_visibility();

            // These need to be the same visibility
            // of the parent based on their location
        }

        parent.ci.type = CubeType::Air;
        // TODO - make this more efficient
        for (int i = 0; i < 8; ++i)
        {
            updateVisibility(m_tree[child_index + i]);
        }

        info.down(m_tree);
    }

    info.node->ci = ci;
    updateVisibility(*info.node);
}

void OctreeMixin::set(const OctreeMixin::ivec3& p, const cube_info& ci)
{
    auto info = get_internal(p);
    Node& node = *info.node;
    if (node.ci == ci)
    {
        return;
    }

    const int index = index_of(node);
    if (index >= m_offsetToLeaves)
    {
        // We're a leaf node, set this directly
        node.ci = ci;
        collapse(node);
        return;
    }

    // Not a leaf, so need to carve out a path to the cube
    insert_down(info, ci);
}

cube_info& OctreeMixin::get(const OctreeMixin::ivec3& p)
{
    return get_internal(p).node->ci;
}
