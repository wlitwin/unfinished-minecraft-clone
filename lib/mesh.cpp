#include "mesh.hpp"

#include <cassert>
#include <GL/gl.h>

//=============================================================================
// Static data
//=============================================================================
unsigned char Mesh::VERTICES  = 0x1;
unsigned char Mesh::NORMALS   = 0x2;
unsigned char Mesh::TEXCOORDS = 0x4;

//=============================================================================
// Constructor
//=============================================================================

Mesh::Mesh() : 
    m_vao(0), 
    m_vbo(0),
    m_mesh(),
    m_primitiveType(GL_TRIANGLES),
    m_data_types(VERTICES | NORMALS | TEXCOORDS)
{ }

//=============================================================================
// Destructor
//=============================================================================

Mesh::~Mesh() 
{
    cleanup();
}

//=============================================================================
// cleanup 
//=============================================================================

void Mesh::cleanup()
{
    SetIncludedData(VERTICES | NORMALS | TEXCOORDS);

    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }

    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    m_mesh.clear();
}

//=============================================================================
// NewMesh
//=============================================================================

void Mesh::NewMesh()
{
    cleanup();  
}

//=============================================================================
// addToVector(vector<float>, glm::vec3)
//=============================================================================

static
void addToVector(std::vector<float>& vec, const glm::vec3& p)
{
    vec.push_back(p[0]);
    vec.push_back(p[1]);
    vec.push_back(p[2]);
}

//=============================================================================
// addToVector(vector<float>, glm::vec2)
//=============================================================================
static
void addToVector(std::vector<float>& vec, const glm::vec2& p)
{
    vec.push_back(p[0]);
    vec.push_back(p[1]);
}

//=============================================================================
// calcNormal
//=============================================================================

/*
static 
glm::vec3 calcNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
    // Calculate the triangles normal vector
    const glm::vec3 e1(p2 - p1);
    const glm::vec3 e2(p3 - p1);
    return glm::normalize(glm::cross(e1, e2));
}
*/

void Mesh::AddTriangle(const glm::vec3& p1, const glm::vec2& t1, const glm::vec3& n1,
                       const glm::vec3& p2, const glm::vec2& t2, const glm::vec3& n2,
                       const glm::vec3& p3, const glm::vec2& t3, const glm::vec3& n3)
{
    assert(m_vao == 0);
    assert(m_vbo == 0);
    assert(m_data_types == (VERTICES | NORMALS | TEXCOORDS));

    addToVector(m_mesh, p1);
    addToVector(m_mesh, n1);
    addToVector(m_mesh, t1);

    addToVector(m_mesh, p2);
    addToVector(m_mesh, n2);
    addToVector(m_mesh, t2);

    addToVector(m_mesh, p3);
    addToVector(m_mesh, n3);
    addToVector(m_mesh, t3);
}

//=============================================================================
// AddTriangle(p1, t1, p2, t2, p3, t3)
//=============================================================================
    
void Mesh::AddTriangle(const glm::vec3& p1, const glm::vec2& t1,
                       const glm::vec3& p2, const glm::vec2& t2,
                       const glm::vec3& p3, const glm::vec2& t3)
{
    assert(m_vao == 0);
    assert(m_vbo == 0);
    assert(m_data_types == (VERTICES | TEXCOORDS));

    addToVector(m_mesh, p1);
    addToVector(m_mesh, t1);

    addToVector(m_mesh, p2);
    addToVector(m_mesh, t2);

    addToVector(m_mesh, p3);
    addToVector(m_mesh, t3);
}

//=============================================================================
// AddTriangle(p1, n1, p2, n2, p3, n3)
//=============================================================================

void Mesh::AddTriangle(const glm::vec3& p1, const glm::vec3& n1,
                       const glm::vec3& p2, const glm::vec3& n2,
                       const glm::vec3& p3, const glm::vec3& n3)
{
    assert(m_vao == 0);
    assert(m_vbo == 0);
    assert(m_data_types == (VERTICES | NORMALS));

    addToVector(m_mesh, p1);
    addToVector(m_mesh, n1);

    addToVector(m_mesh, p2);
    addToVector(m_mesh, n2);

    addToVector(m_mesh, p3);
    addToVector(m_mesh, n3);
}

//=============================================================================
// AddTriangle(p1, p2, p3)
//=============================================================================

void Mesh::AddTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
    assert(m_vao == 0);
    assert(m_vbo == 0);
    assert(m_data_types == VERTICES);

    addToVector(m_mesh, p1);
    addToVector(m_mesh, p2);
    addToVector(m_mesh, p3);
}

//=============================================================================
// AddQuad(p1, t1, p2, t2, p3, t3, p4, t4)
//=============================================================================

void Mesh::AddQuad(const glm::vec3& p1, const glm::vec2& t1,
                   const glm::vec3& p2, const glm::vec2& t2,
                   const glm::vec3& p3, const glm::vec2& t3,
                   const glm::vec3& p4, const glm::vec2& t4)
{
    AddTriangle(p1, t1, p2, t2, p4, t4);
    AddTriangle(p2, t2, p3, t3, p4, t4);
}

//=============================================================================
// AddQuad(p1, p2, p3, p4)
//=============================================================================

void Mesh::AddQuad(const glm::vec3& p1, const glm::vec3& p2, 
                   const glm::vec3& p3, const glm::vec3& p4)
{
    AddTriangle(p1, p2, p4);
    AddTriangle(p2, p3, p4);
}

//=============================================================================
// AddQuad(p1, t1, n1, p2, t2, n2, p3, t3, n3, p4, t4, n4)
//=============================================================================

void Mesh::AddQuad(const glm::vec3& p1, const glm::vec2& t1, const glm::vec3& n1,
                 const glm::vec3& p2, const glm::vec2& t2, const glm::vec3& n2,
                 const glm::vec3& p3, const glm::vec2& t3, const glm::vec3& n3,
                 const glm::vec3& p4, const glm::vec2& t4, const glm::vec3& n4)
{
    AddTriangle(p1, t1, n1, p2, t2, n2, p4, t4, n4);
    AddTriangle(p2, t2, n2, p3, t3, n3, p4, t4, n4);
}

//=============================================================================
// AddQuad(p1, n1, p2, n2, p3, n3, p4, n4)
//=============================================================================

void Mesh::AddQuad(const glm::vec3& p1, const glm::vec3& n1,
                   const glm::vec3& p2, const glm::vec3& n2, 
                   const glm::vec3& p3, const glm::vec3& n3,
                   const glm::vec3& p4, const glm::vec3& n4)
{
    AddTriangle(p1, n1, p2, n2, p4, n4);
    AddTriangle(p2, n2, p3, n3, p4, n4);
}

//=============================================================================
// Finish
//=============================================================================

void Mesh::Finish()
{
    assert(m_vao == 0);
    assert(m_vbo == 0);
    assert(m_mesh.size() != 0);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    assert(m_vao != 0);
    assert(m_vbo != 0);
    assert(m_data_types > 0);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    size_t size = m_mesh.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, size, &m_mesh[0], GL_STATIC_DRAW);

    // Setup the vertex buffers
    int ptr_index = 0;
    int stride = 0;

    // Figure out how many flags are enabled
    if ((m_data_types & VERTICES) > 0) 
    {
        stride += 3;
    }

    if ((m_data_types & NORMALS) > 0)
    {
        stride += 3;
    }

    if ((m_data_types & TEXCOORDS) > 0)
    {
        stride += 2;
    }

    stride *= sizeof(float);

    long offset = 0;

    if ((m_data_types & VERTICES) > 0)
    {
        glVertexAttribPointer(ptr_index, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
        ++ptr_index;
        offset += 3*sizeof(float);
    }

    if ((m_data_types & NORMALS) > 0)
    {
        glVertexAttribPointer(ptr_index, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
        ++ptr_index;
        offset += 3*sizeof(float);
    }

    if ((m_data_types & TEXCOORDS) > 0)
    {
        glVertexAttribPointer(ptr_index, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
        ++ptr_index;
    }

    // These are on a per vertex array basis
    for (int i = 0; i < ptr_index; ++i)
    {
        glEnableVertexAttribArray(i);
    }

    // Unbind TODO - rebind what was previously bound?
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

//=============================================================================
// UpdateData
//=============================================================================

void Mesh::UpdateData()
{
    assert(m_vao != 0);
    assert(m_vbo != 0);
    assert(m_mesh.size() > 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_mesh.size()*sizeof(float), &m_mesh[0]);
}

//=============================================================================
// SetPrimitiveType
//=============================================================================

void Mesh::SetPrimitiveType(GLenum type)
{
    assert(type == GL_TRIANGLES || type == GL_POINTS);

    m_primitiveType = type;
}

//=============================================================================
// SetIncludedData
//=============================================================================

void Mesh::SetIncludedData(unsigned char flags)
{
    // Mask out unused flags for now
    m_data_types = flags & 0x7;
}

//=============================================================================
// SetIncludedData
//=============================================================================

unsigned char Mesh::GetIncludedData()
{
    return m_data_types;
}

//=============================================================================
// AddPoint(glm::vec3)
//=============================================================================

void Mesh::AddPoint(const glm::vec3& point)
{
    assert(m_vao == 0);
    assert(m_vbo == 0);
    assert(m_data_types == VERTICES);

    addToVector(m_mesh, point);
}

//=============================================================================
// AddPoint(glm::vec3, glm::vec3)
//=============================================================================

void Mesh::AddPoint(const glm::vec3& point, const glm::vec3& norm)
{
    assert(m_vao == 0);
    assert(m_vbo == 0);
    assert(m_data_types == (VERTICES | NORMALS));

    addToVector(m_mesh, point);
    addToVector(m_mesh, norm);
}

//=============================================================================
// AddPoint(glm::vec3, glm::vec3, glm::vec2)
//=============================================================================

void Mesh::AddPoint(const glm::vec3& point, const glm::vec3& norm, 
        const glm::vec2& texCoord)
{
    assert(m_vao == 0);
    assert(m_vbo == 0);
    assert(m_data_types == (VERTICES | TEXCOORDS | NORMALS));

    addToVector(m_mesh, point);
    addToVector(m_mesh, norm);
    addToVector(m_mesh, texCoord);
}

//=============================================================================
// NewMesh
//=============================================================================

void Mesh::Render()
{
    assert(m_vao != 0);
    assert(m_vbo != 0);
    assert(m_data_types != 0);

    glBindVertexArray(m_vao);

    glDrawArrays(m_primitiveType, 0, m_mesh.size());
}

//=============================================================================
// 
//=============================================================================
