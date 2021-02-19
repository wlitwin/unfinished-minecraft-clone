#include "chunk.hpp"
#include <algorithm>
#include <GL/gl.h>
#include "util.hpp"
#include <glm/gtc/noise.hpp>
#include "simplex.hpp"
#include "neighbors.hpp"

#define IDX(x, y, z) ((x) + (y)*chunk_size + (z)*chunk_size*chunk_size)

bool isSolidCubeType(const cube_info& ci)
{
    return ci.type != CubeType::Air;
}

bool DirtyTracker::isDirty() const
{
    return !m_dirtyCubes.empty() || m_allDirty;
}

void DirtyTracker::addCube(const glm::ivec3& loc)
{
    if (!m_allDirty) {
        m_dirtyCubes.insert(loc);
    }
}

size_t DirtyTracker::count() const
{
    return m_dirtyCubes.size();
}

DirtyTracker::DirtyTracker()
{
    m_dirtyCubes.reserve(chunk_size*chunk_size*chunk_size);
}

void DirtyTracker::setAllDirty()
{
//    m_allDirty = true;
//    m_dirtyCubes.clear();
}

DirtyTrackerStore::const_iterator DirtyTracker::begin() const
{
    return m_dirtyCubes.begin();
}

DirtyTrackerStore::const_iterator DirtyTracker::end() const
{
    return m_dirtyCubes.end();
}

bool DirtyTracker::allDirty() const
{
    return m_allDirty;
}

void DirtyTracker::clear()
{
    m_dirtyCubes.clear();
}

TextureTracker::TextureTracker() = default;

TextureTracker::~TextureTracker()
{
    Free();
}

void TextureTracker::InitTextures()
{
    // Pixel store doesn't matter here because we're using single
    // row textures
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    checkGLErrors();
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    checkGLErrors();
    glTexImage1D(GL_TEXTURE_1D,
            0,
            GL_RGBA,
            chunk_size*chunk_size*chunk_size,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            NULL);
    checkGLErrors();

    glGenTextures(1, &mat_texture);
    glBindTexture(GL_TEXTURE_1D, mat_texture);
    checkGLErrors();
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage1D(GL_TEXTURE_1D,
            0,
            GL_R8UI,
            chunk_size*chunk_size*chunk_size,
            0,
            GL_RED_INTEGER,
            GL_UNSIGNED_BYTE,
            NULL);
    checkGLErrors();

    m_loaded = true;
}

void TextureTracker::UploadTextureInfo(const uint8_t locs[],
                                       const uint8_t mats[],
                                       const int count)
{
    if (!m_loaded)
    {
        InitTextures();
    }

    glBindTexture(GL_TEXTURE_1D, texture);
    glTexSubImage1D(GL_TEXTURE_1D,
            0,
            0,
            count,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            &locs[0]);

    checkGLErrors();
    glBindTexture(GL_TEXTURE_1D, mat_texture);
    glTexImage1D(GL_TEXTURE_1D,
            0,
            GL_R8UI,
            count,
            0,
            GL_RED_INTEGER,
            GL_UNSIGNED_BYTE,
            &mats[0]);
    checkGLErrors();
}

void TextureTracker::UploadTextureInfo(const std::vector<uint8_t>& locs,
                                       const std::vector<uint8_t>& mats)
{
    assert(false);
    assert(locs.size() == mats.size()*4);
    if (locs.empty()) return;
    if (!m_loaded)
    {
        InitTextures();
    }

    glBindTexture(GL_TEXTURE_1D, texture);
    glTexSubImage1D(GL_TEXTURE_1D,
            0,
            0,
            mats.size(),
            GL_RGB,
            GL_UNSIGNED_BYTE,
            locs.data());

    checkGLErrors();
    glBindTexture(GL_TEXTURE_1D, mat_texture);
    glTexImage1D(GL_TEXTURE_1D,
            0,
            GL_R8UI,
            mats.size(),
            0,
            GL_RED_INTEGER,
            GL_UNSIGNED_BYTE,
            mats.data());
    checkGLErrors();
}

void TextureTracker::Bind() const
{
    if (!m_loaded) {
        printf("Trying to bind unbound texture\n");
        return;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, texture);
    checkGLErrors();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, mat_texture);
    checkGLErrors();
}

void TextureTracker::Free()
{
    if (m_loaded)
    {
        const GLuint textures[] = { texture, mat_texture };
        glDeleteTextures(2, &textures[0]);
        m_loaded = false;
    }
}

Chunk::Chunk(const GetChunkAtFunc& getChunkAt, IChunkUpdateScheduler* scheduler)
    : m_octree(std::make_unique<OctreeMixin>(total_size, this, this))
    , m_scheduler(scheduler)
    , m_getChunkAt(getChunkAt)
{
}

void Chunk::OctreeUpdated()
{
    m_scheduler->MarkForUpdating(this);
}

OctreeMixin* Chunk::GetNeighbor(Neighbor n)
{
    const auto neighborLoc = offset + neighbor_offset(n)*chunk_size;
    auto chunk = m_getChunkAt(neighborLoc);
    if (chunk != nullptr)
    {
        return chunk->m_octree.get();
    }
    return nullptr;
}

void Chunk::set(const glm::ivec3& p, const cube_info& ci)
{
    m_octree->set(p, ci); 
    //m_scheduler->MarkForUpdating(this);
}

cube_info& Chunk::get(int x, int y, int z)
{
    return m_octree->get(OctreeMixin::ivec3(x, y, z));
}

void Chunk::generateCubes()
{
    //m_dirty.setAllDirty();
    m_dirty.clear();
    cube_info ci;
    ci.type = CubeType::Solid;
    cube_info air;
    auto batch = m_octree->batch_set();
    for (int x = 0; x < chunk_size; ++x)
    for (int z = 0; z < chunk_size; ++z)
    {
        /*
        const glm::vec3 p = glm::vec3(offset.x, 5, offset.z) + glm::vec3(x, 0, z);
        const int n = abs(int(glm::simplex(p*0.01f)*chunk_size*4.0f)) + 1;
        const int height = glm::clamp(n, offset.y, offset.y+chunk_size) - offset.y;
        for (int y = 0; y < height; ++y)
        {
            set(glm::ivec3(x, y, z), ci);
        }
        for (int y = height; y < chunk_size; ++y)
        {
            const glm::ivec3 p(x, y, z);
            set(p, air);
        }
        */
        for (int y = 0; y < chunk_size; ++y)
        {
            const glm::vec3 p = glm::vec3(offset.x+x, offset.y+y, offset.z+z) * (1.0f / 128.0f);
            const float val = Simplex::ridgedNoise(p);
            if (val < 0.6)
            {
                set(glm::ivec3(x, y, z), ci);
            }
            /*
            const glm::ivec3 p(offset + glm::ivec3(x, y, z));
            if (p.y < 0)
            {
                set(glm::ivec3(x, y, z), ci);
            }
            */
            //printf("%d %d %d\n", p.x, p.y, p.z);
        }

        /*
        for (int y = 0; y < chunk_size/2; ++y)
        {
            set(glm::ivec3(x, y, z), ci);
        }
        */
    }
    //printf("Finished generateCubes %d dirty cubes\n", m_dirty.count());
}

bool Chunk::inBounds(const glm::ivec3& p) const
{
    return p.x >= 0
           && p.y >= 0
           && p.z >= 0
           && p.x < chunk_size
           && p.y < chunk_size
           && p.z < chunk_size;
}

void Chunk::init(const glm::vec3& off)
{
    offset = off;
    aabb.min = offset;
    aabb.max = glm::vec3(offset) + glm::vec3(chunk_size);
}

namespace
{
static std::vector<const OctreeMixin::Node*> tempNodeBuffer;
static uint8_t tempCubeBuffer[total_size*4]; // 4 bytes per cube x,y,z size
static uint8_t tempMatBuffer[total_size];  // 1 byte per cube
}

void Chunk::copyCubesToBuffer()
{
    tempNodeBuffer.clear();
    m_octree->get_bounds_info(tempNodeBuffer, true);

    int cubeIdx = 0;
    int matIdx = 0;
    for (const auto& node : tempNodeBuffer)
    {
        if (node->visibility() > 0)
        {
            tempCubeBuffer[cubeIdx+0] = node->coord.x;
            tempCubeBuffer[cubeIdx+1] = node->coord.y;
            tempCubeBuffer[cubeIdx+2] = node->coord.z;
            tempCubeBuffer[cubeIdx+3] = node->coord.w;
            tempMatBuffer[matIdx] = node->ci.type;
            cubeIdx += 4;
            ++matIdx;
        }
    }

    m_renderCount = matIdx;
}

int Chunk::renderCount() const
{
    return m_renderCount;
}

void Chunk::update()
{
    copyCubesToBuffer();
    if (m_renderCount == 0)
    {
        textures.Free();
        return;
    }

    //printf("Render count %d\n", m_renderCount);
    textures.UploadTextureInfo(tempCubeBuffer, tempMatBuffer, m_renderCount);
}
