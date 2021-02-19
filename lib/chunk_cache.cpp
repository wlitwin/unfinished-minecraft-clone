#include "chunk_cache.hpp"

ChunkCache::ChunkCache(const size_t maxChunks)
    : m_maxChunks(maxChunks)
{ }

void ChunkCache::CheckLRUSize()
{
    if (m_lru.size() > m_maxChunks)
    {
//        printf("\nPURGING CACHE\n\n");
        // Pop off the last item in the list and delete it
        auto chunk = m_lru.back();
        m_lru.pop_back();
        m_chunkCache.erase(chunk->offset);
        delete chunk;
    }
}

void ChunkCache::AddChunk(Chunk* chunk)
{
    auto exists = m_chunkCache.find(chunk->offset);
    if (exists != m_chunkCache.end())
    {
        Chunk* other = *exists->second;
        printf("Chunk %d %d %d already present coords %d %d %d\n",
                chunk->offset.x,
                chunk->offset.y,
                chunk->offset.z,
                other->offset.x,
                other->offset.y,
                other->offset.z);
        printf("Same chunk? %p %p %d\n", chunk, *exists->second, chunk == *exists->second);
        assert(false);
    }

    auto it = m_lru.insert(m_lru.begin(), chunk);
    m_chunkCache.insert(std::make_pair(chunk->offset, it));
    CheckLRUSize();
}

Chunk* ChunkCache::TakeChunk(const glm::ivec3& loc)
{
    auto it = m_chunkCache.find(loc);
    if (it == m_chunkCache.end())
    {
        return nullptr;
    }

    Chunk* chunk = *(it->second);
    m_lru.erase(it->second);
    m_chunkCache.erase(it);

    return chunk;
}
