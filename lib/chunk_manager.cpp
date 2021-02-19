#include "chunk_manager.hpp"
#include "intersection_tests.hpp"
#include "util.hpp"
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

ChunkManager::ChunkManager(const glm::ivec3& worldExtents)
    : m_worldExtents(worldExtents)
    , m_lastOrigin(0)
    , m_originChunk(0)
    , m_chunkCache(worldExtents.x*worldExtents.y*worldExtents.z*2)
    , m_progress(0)
    , m_lastPos(0)
{
}

Chunk* ChunkManager::CreateChunk(const glm::ivec3& loc)
{
    const auto getChunkAt = [&](const glm::ivec3& loc)
    {
        return this->getChunkAt(loc);
    };

    Chunk* chunk = new Chunk(getChunkAt, this);
    chunk->init(loc);
    chunk->generateCubes();
    chunk->update();

    return chunk;
}

void ChunkManager::init()
{
    m_chunks.reserve(m_worldExtents.x*m_worldExtents.y*m_worldExtents.z);
    m_originChunk = worldToIndex(-glm::vec3(m_worldExtents/2*chunk_size));
    m_lastOrigin = m_originChunk;
    AabbInt worldAabb{m_originChunk, m_worldExtents*chunk_size};
    m_occluder.clearAndSet(worldAabb);
}

glm::ivec3 ChunkManager::worldToIndex(const glm::ivec3& world) const
{
    const int vx = (world.x < 0) ? world.x - (chunk_size - 1) : world.x;
    const int vy = (world.y < 0) ? world.y - (chunk_size - 1) : world.y;
    const int vz = (world.z < 0) ? world.z - (chunk_size - 1) : world.z;
    return (glm::ivec3(vx, vy, vz) / chunk_size) * chunk_size;
}

Chunk* ChunkManager::getChunkAt(const glm::ivec3& p) const
{
    auto it = m_chunks.find(worldToIndex(p));
    if (it == m_chunks.end())
    {
        return nullptr;
    }

    return it->second;
}

bool ChunkManager::setBlockAt(const glm::vec3& pos, const cube_info& ci)
{
    const glm::ivec3 ipos = pos;
    Chunk* chunk = getChunkAt(ipos);
    if (!chunk) { 
        printf("No chunk\n");
        return false; }

    // Grab the chunk + cube
    const glm::ivec3 innerLoc = ipos - chunk->offset;
    const int renderCountBefore = chunk->renderCount();

    chunk->set(innerLoc, ci);
    chunk->update();
    const int renderCountAfter = chunk->renderCount();
    printf("Render count change %d -> %d\n", renderCountBefore, renderCountAfter);
    return true;
}

void ChunkManager::MarkForUpdating(Chunk* chunk)
{
    m_needsUpdate.insert(chunk);
}

cube_info* ChunkManager::getBlockAt(const glm::vec3& pos)
{
    const glm::ivec3 ipos = pos;
    Chunk* chunk = getChunkAt(ipos);
    if (!chunk) { return nullptr; }

    const glm::ivec3 innerLoc = ipos - chunk->offset;
    return &chunk->get(innerLoc.x, innerLoc.y, innerLoc.z);
}

ChunkManager::SearchResult::SearchResult(const Ray& ray)
    : result(ray)
    , node(nullptr)
{ }

// TODO could live outside ChunkManager
ChunkManager::SearchResult ChunkManager::intersect_ray(const Ray& _ray, const PredFunc& pred) const
{
    SearchResult result(_ray);

    const auto calc_face = [](const Aabb& aabb, const glm::vec3& pt)
    {
        const glm::vec3 extents = aabb.max - aabb.min;
        const glm::vec3 half    = extents*0.5f;
        const glm::vec3 top   = aabb.min + glm::vec3(half.x, extents.y, half.z);
        const glm::vec3 bot   = aabb.min + glm::vec3(half.x, 0, half.z);
        const glm::vec3 left  = aabb.min + glm::vec3(0, half.y, half.z);
        const glm::vec3 right = aabb.min + glm::vec3(extents.x, half.y, half.z);
        const glm::vec3 front = aabb.min + glm::vec3(half.x, half.y, 0);
        const glm::vec3 back  = aabb.min + glm::vec3(half.x, half.y, extents.z);

        const std::array<glm::vec3, 6> vals = {
            left, right, top, bot, front, back
        };
        
        float min = glm::length2(pt - left);
        uint8_t neighbor = static_cast<uint8_t>(Neighbor::Left);
        for (int i = 1; i < 6; ++i)
        {
            const float dist = glm::length2(pt - vals[i]);
            if (dist < min) {
                min = dist;
                neighbor = 1 << i;
            }
        }

        return static_cast<Neighbor>(neighbor);
    };

    int times = 0;
    do
    {
        Ray& ray = result.result.ray;
        const glm::ivec3 iorigin = glm::floor(ray.origin);
        const Chunk* c = getChunkAt(iorigin);
        if (c == nullptr) {
            result.result.intersected = false;
            return result;
        }

        const OctreeMixin::Node* node = c->m_octree->get_node(iorigin - c->offset);
        assert(node != nullptr);
        result.node = node;

        const glm::vec3 base = glm::ivec3(node->coord) + c->offset;
        const glm::vec3 size = glm::vec3(node->coord.w);
        const Aabb aabb{base, base+size};

        if (pred(result)) {
            result.face = calc_face(aabb, ray.origin);
            result.result.intersected = true;
            return result;
        }

        result.full_dist += result.result.tmax;
        intersect_ray_aabb(aabb, result.result);

        ray.origin += ray.dir*(result.result.tmax+0.01f);

        ++times;
    } while (times < 150);

    result.result.intersected = false;
    assert(false);
    return result;
}

const ChunkOccluder& ChunkManager::GetOccluder() const
{
    return m_occluder;
}

void ChunkManager::updateWorldCenter(const glm::vec3& p)
{
    for (const auto& chunk : m_needsUpdate)
    {
        chunk->update();
    }
    m_needsUpdate.clear();

    const bool progressFinished = (m_progress == glm::ivec3(m_worldExtents.x, 0, 0));
    if (!progressFinished)
    {
        const glm::ivec3 lastEnd = m_lastOrigin + m_worldExtents*chunk_size;
        const glm::ivec3 newEnd  = m_originChunk + m_worldExtents*chunk_size;

        const AabbInt oldBounds = { m_lastOrigin, lastEnd };
        const AabbInt newBounds = { m_originChunk, newEnd };

        const double start = glfwGetTime();

        int added = 0;
        int deleted = 0;
        int cache_hits = 0;
        int x = m_progress.x;
        int y = m_progress.y;
        int z = m_progress.z;
        for (; x < m_worldExtents.x; ++x, y=0)
        for (; y < m_worldExtents.y; ++y, z=0)
        for (; z < m_worldExtents.z; ++z)
        {
            const glm::ivec3 p = glm::ivec3(x, y, z)*chunk_size;
            const glm::ivec3 pLast = m_lastOrigin + p;
            const glm::ivec3 pNew  = m_originChunk + p;
            const bool shouldDelete = !inside_aabb_int_exclusive(newBounds, pLast);
            if (shouldDelete)
            {
                auto it = m_chunks.find(pLast);
                assert(it != m_chunks.end());
                m_chunkCache.AddChunk(it->second);
                m_chunks.erase(it);
                ++deleted;
            }

            const bool shouldAdd = !inside_aabb_int_exclusive(oldBounds, pNew);
            const auto it = m_chunks.find(pNew);
            if (shouldAdd || it == m_chunks.end())
            {
                Chunk* c = m_chunkCache.TakeChunk(pNew);
                if (c != nullptr) {
                    ++cache_hits;
                } else {
                    c = CreateChunk(pNew);
                }
                m_chunks.insert(std::make_pair(pNew, c));
                m_occluder.addChunk(c);
                ++added;
            }
            else
            {
                m_occluder.addChunk(it->second);
            }

            if (glfwGetTime() - start > 8.0f / 1000.0f)
            {
                ++z;
                goto end;
            }
        }

end:
        const double time = glfwGetTime() - start;
        m_progress = glm::ivec3(x, y, z);
        printf("Final progress %d %d %d - added %d, deleted %d, cache hits %d took %fms\n", x, y, z, added, deleted, cache_hits, time*1000.0);
    }

    const glm::ivec3 newOrigin = worldToIndex(p - glm::vec3(m_worldExtents/2*chunk_size));
    const float now = glfwGetTime();
    // TODO - add hysteresis 
    if (newOrigin != m_originChunk /*&& len > chunk_size*0.5*/ && progressFinished && (now - m_lastChunkOriginChange) >= 5.0)
    {
        AabbInt worldAabb{newOrigin, m_worldExtents*chunk_size};
        m_occluder.clearAndSet(worldAabb);
        m_lastChunkOriginChange = now;
        m_needsUpdate.clear();

        printf("New bounds %d %d %d\n", newOrigin.x, newOrigin.y, newOrigin.z);

        m_lastPos = p;
        m_progress = glm::ivec3(0);

        m_lastOrigin = m_originChunk;
        m_originChunk = newOrigin;
    }

}

const ChunkMap& ChunkManager::chunks() const
{
    return m_chunks;
}
