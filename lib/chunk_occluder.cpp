#include "chunk_occluder.hpp"

// TODO - make more dynamic and "online" so that
//        chunks can by dynamically added and removed.
//        Can also do some kind of temporal caching

ChunkOccluder::ChunkOccluder()
{
}

void ChunkOccluder::clearAndSet(const AabbInt& bounds)
{
    m_bounds = Aabb{glm::vec3(bounds.min), glm::vec3(bounds.max)};
    const glm::vec3 extent = m_bounds.extent();
    const glm::vec3 up(0, extent.y, 0);
    const glm::vec3 right(extent.x, 0, 0);
    const glm::vec3 back(0, 0, extent.z);

    // Clear out existing chunks
    for (auto& vec : m_chunks)
    {
        vec.clear();
    }

    // Setup new AABBs
    const glm::vec3 center = m_bounds.min+extent;
    m_octets[0] = Aabb{m_bounds.min,            center};
    m_octets[1] = Aabb{m_bounds.min+back,       center+back};
    m_octets[2] = Aabb{m_bounds.min+right,      center+right};
    m_octets[3] = Aabb{m_bounds.min+back+right, center+back+right};
    const glm::vec3 up_min = m_bounds.min+up;
    const glm::vec3 up_center = up_min+extent;
    m_octets[4] = Aabb{up_min,            up_center};
    m_octets[5] = Aabb{up_min+back,       up_center+back};
    m_octets[6] = Aabb{up_min+right,      up_center+right};
    m_octets[7] = Aabb{up_min+right+back, up_center+right+back};
}

void ChunkOccluder::addChunk(const Chunk* c)
{
    assert(c != nullptr);

    for (int i = 0; i < 8; ++i)
    {
        if (inside_aabb_exclusive(m_octets[i], glm::vec3(c->offset)))
        {
            m_chunks[i].push_back(c);
            return;
        }
    }

    assert(false);
}

ChunkOccluder::Stats ChunkOccluder::cull(const frustum_planes& planes, std::vector<const Chunk*>& visible) const
{
    Stats stats;
    visible.clear();

    for (int i = 0; i < 8; ++i)
    {
        if (aabb_outside_frustum(planes, m_octets[i]))
        {
            stats.cullCount += m_chunks[i].size();
            continue;
        }

        for (const auto& c : m_chunks[i])
        {
            const int renderCount = c->renderCount();
            if (renderCount == 0
                || aabb_outside_frustum(planes, c->aabb))
            {
                ++stats.cullCount;
                continue;
            }

            stats.cubeCount += renderCount;
            visible.push_back(c);
        }
    }

    return stats;
}
