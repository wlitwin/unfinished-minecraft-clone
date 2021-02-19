#pragma once

#include "chunk_defs.hpp"
#include "chunk.hpp"
#include <glm/glm.hpp>
#include <unordered_set>
#include "octree_mixin.hpp"
#include "chunk_cache.hpp"
#include "chunk_occluder.hpp"
#include "ray.hpp"

using ChunkMap = std::unordered_map<glm::ivec3, Chunk*, GLMivec3Hash>;
class ChunkManager : public IChunkUpdateScheduler
{
public:
    ChunkManager(const glm::ivec3& totalWorldExtentToLoad);

    void init();
    void updateWorldCenter(const glm::vec3& newPos);

    void MarkForUpdating(Chunk* chunk) override;

    bool setBlockAt(const glm::vec3& pos, const cube_info& ci);
    cube_info* getBlockAt(const glm::vec3&);
//    bool inBounds(const glm::ivec3& p) const;
    Chunk* getChunkAt(const glm::ivec3& p) const;
    bool existsInNeighbor(const Chunk&, const glm::ivec3&) const;

    glm::ivec3 worldToIndex(const glm::ivec3& world) const;

    const ChunkMap& chunks() const;
    const ChunkOccluder& GetOccluder() const;

    struct SearchResult
    {
        RayResult result;
        float full_dist = 0;
        const OctreeMixin::Node* node = nullptr;
        Neighbor face = Neighbor::Left;
        SearchResult(const Ray&);
    };
    using PredFunc = std::function<bool(const SearchResult&)>;
    SearchResult intersect_ray(const Ray& ray, const PredFunc& pred) const;

private:
    Chunk* CreateChunk(const glm::ivec3& loc);

    ChunkOccluder m_occluder;
    std::unordered_set<Chunk*> m_needsUpdate;
    ChunkMap m_chunks;
    glm::ivec3 m_worldExtents;
    glm::ivec3 m_lastOrigin;
    glm::ivec3 m_originChunk;
    ChunkCache m_chunkCache;
    glm::ivec3 m_progress;
    glm::vec3 m_lastPos;
    float m_lastChunkOriginChange = 0;
};
