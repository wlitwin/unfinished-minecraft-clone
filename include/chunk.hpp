#pragma once

#include <glm/glm.hpp>
#include <GL/gl.h>
#include <cstdint>
#include <memory>
#include <vector>
#include <bitset>
#include "aabb.hpp"
#include "cube_info.hpp"
#include "chunk_defs.hpp"
#include <functional>
#include "util.hpp"
#include <unordered_set>
#include "octree_mixin.hpp"

using DirtyTrackerStore = std::unordered_set<glm::ivec3, GLMivec3Hash>;

class Chunk;
class IChunkUpdateScheduler
{
public:
    virtual void MarkForUpdating(Chunk*) = 0;
};

class DirtyTracker
{
    bool m_allDirty = false; 
    DirtyTrackerStore m_dirtyCubes;
public:
    DirtyTracker();
    DirtyTrackerStore::const_iterator begin() const;
    DirtyTrackerStore::const_iterator end() const;
    void addCube(const glm::ivec3&);
    size_t count() const;
    bool isDirty() const;
    bool allDirty() const;
    void setAllDirty();
    void clear();
};

class TextureTracker
{
    bool m_loaded = false;
    GLuint texture = 0;
    GLuint mat_texture = 0;

    void InitTextures();

public:
    TextureTracker();
    ~TextureTracker();
    void UploadTextureInfo(const std::vector<uint8_t>& locs,
                            const std::vector<uint8_t>& mats);
    void UploadTextureInfo(const uint8_t locs[],
                           const uint8_t mats[],
                           const int count);
    void Bind() const;
    void Free();
};

using GetChunkAtFunc = std::function<Chunk*(const glm::ivec3&)>;

class Chunk : public IOctreeMixinFinder, public IOctreeUpdateObserver
{
public:
    TextureTracker textures;
    Aabb aabb = { glm::vec3(0), glm::vec3(0) };
    glm::ivec3 offset = glm::ivec3(0);
    std::unique_ptr<OctreeMixin> m_octree;
    IChunkUpdateScheduler* m_scheduler;
    DirtyTracker m_dirty;
    int m_renderCount = 0;
    GetChunkAtFunc m_getChunkAt;

    Chunk(const GetChunkAtFunc& getChunkAt, IChunkUpdateScheduler* scheduler);
    Chunk(const Chunk&) = delete;
    Chunk& operator=(const Chunk&) = delete;

    OctreeMixin* GetNeighbor(Neighbor n) override;
    void OctreeUpdated() override;
    bool inBounds(const glm::ivec3&) const;
    bool exists(const glm::ivec3& c) const;
    bool surrounded(const glm::ivec3& coord) const;
    void set(const glm::ivec3& p, const cube_info&);
    cube_info& get(int x, int y, int z);
    int renderCount() const;
    void init(const glm::vec3& offset);
    void generateCubes();
    void copyCubesToBuffer();
    void update();
};
