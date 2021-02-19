#pragma once
#include <unordered_map>
#include <glm/glm.hpp>
#include <list>
#include "chunk_defs.hpp"
#include "chunk.hpp"

using ChunkMap = std::unordered_map<glm::ivec3, Chunk*, GLMivec3Hash>;

class ChunkCache
{
public:
    ChunkCache(size_t maxChunks);
    Chunk* TakeChunk(const glm::ivec3&);
    void AddChunk(Chunk* chunk);

private:
    void CheckLRUSize();

    size_t m_maxChunks;
    using chunk_list = std::list<Chunk*>;
    chunk_list m_lru;
    std::unordered_map<glm::ivec3, chunk_list::iterator, GLMivec3Hash> m_chunkCache;
};

