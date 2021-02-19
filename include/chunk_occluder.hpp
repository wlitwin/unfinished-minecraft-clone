#pragma once

#include <array>
#include <vector>
#include "chunk.hpp"
#include "frustum.hpp"

class ChunkOccluder
{
    using ChunkVec = std::vector<const Chunk*>;
    std::array<Aabb, 8> m_octets;
    std::array<ChunkVec, 8> m_chunks;

    Aabb m_bounds;
public:
    ChunkOccluder();
    void clearAndSet(const AabbInt& worldAabb);
    void addChunk(const Chunk* c);

    struct Stats
    {
        int cubeCount = 0;
        int cullCount = 0;
    };
    Stats cull(const frustum_planes& planes, std::vector<const Chunk*>& visible) const;
};
