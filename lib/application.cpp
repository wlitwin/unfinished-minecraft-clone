#include "mesh.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "objloader.hpp"
#include "application.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ctime>
#include <cstdlib>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

namespace
{

// Application title
const char* title = "Meshless";

float width = 0;
float height = 0;
double dt_multiplier = 1.0; // slow motion

Camera camera;
}

// Small macro to check for a single keydown event
bool __glfw_keys[GLFW_KEY_LAST + 1];
#define glfwKeyPressed(KEY)\
    (glfwGetKey(KEY) == GLFW_PRESS ?\
     (__glfw_keys[KEY] ? false : (__glfw_keys[KEY] = true)) :\
     (__glfw_keys[KEY] = false))

//=============================================================================
// initialize
//=============================================================================

bool initialize(int argc, char* argv[])
{
    /*
    glEnable(GL_DEPTH_TEST);

    // Load the model
    ObjLoader obj;
    const char* file = "sphere.obj";
    if (argc > 1)
    {
        file = argv[1];
    }

    if (!obj.LoadFile(file) || !obj.ToMesh(g_obj_mesh, Mesh::VERTICES))
    {
        cerr << "Failed to load OBJ file" << endl;
        return false;
    }

    size_t size = g_obj_mesh.GetDataSize();
    real* data = g_obj_mesh.GetData();
    for (size_t i = 0; i < size; i += 3)
    {
        data[i+0] += 0;
        data[i+1] += 5;
        data[i+2] += 0;
    }
    g_obj_mesh.UpdateData();

    g_psystem = new PSystem(g_obj_mesh);

	std::cout << g_psystem->GetNumParticles() << " number of particles\n";

    // Create the ground plane
    g_ground_mesh.NewMesh();

    using glm::vec3;
    using glm::vec2;
    float norm[3] = { 0, 1, 0 };
    const float plane_half_size = 20.0f;
    float v[4][3] = { {-plane_half_size, 0, -plane_half_size},
                      {-plane_half_size, 0,  plane_half_size},
                      { plane_half_size, 0,  plane_half_size},
                      { plane_half_size, 0, -plane_half_size},
                    };
    float t[4][2] = { {0, 0}, {0, 1}, {1, 1}, {1, 0} };
    vec3 normal(norm);
    g_ground_mesh.AddQuad(vec3(v[0]), vec2(t[0]), normal,
                        vec3(v[1]), vec2(t[1]), normal,
                        vec3(v[2]), vec2(t[2]), normal,
                        vec3(v[3]), vec2(t[3]), normal);

    g_ground_mesh.Finish();

    glfwDisable(GLFW_MOUSE_CURSOR);
    glfwGetMousePos(&g_mouse_x, &g_mouse_y);

    // Defaults for now
    camera.LookAt(glm::vec3(0, 10, -20), glm::vec3(0), glm::vec3(0, 1, 0));

    // Set the forces
    g_gravity = 0, -9.8, 0;

    srand(time(NULL));

    const bool shaders_loaded = 
            ground_shader.LoadShaders("glsl/ground.vert", "glsl/ground.frag") &&
            object_shader.LoadShaders("glsl/object.vert", "glsl/object.frag",
                                     "glsl/object.geom");
    if (shaders_loaded)
    {
        print_help();
        return true;
    }

    */
    return false;
}

//=============================================================================
// cleanup
//=============================================================================

void cleanup()
{
}

//=============================================================================
// resolution_changed
//=============================================================================

void resolution_changed(int w, int h)
{
    width = w; 
    height = h;
    glViewport(0, 0, w, h);

    camera.SetPerspective(75.0f, (float)w/h, 0.1f, 1000.0f);
}

//=============================================================================
// randf
//=============================================================================

static float randf()
{
    return static_cast<float>(::rand() / static_cast<float>(RAND_MAX));
}

static float randf(float min, float max)
{
    return randf()*(max-min) + min;
}

//=============================================================================
// update
//=============================================================================

bool update(double dt)
{
    return true;
}


//=============================================================================
// render
//=============================================================================

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
    // All vertices are already in world coordinates, don't need model matrix
    ground_shader.Bind();
    glUniformMatrix4fv(ground_shader["proj"], 1, GL_FALSE, glm::value_ptr(camera.GetProj()));
    glUniformMatrix4fv(ground_shader["view"], 1, GL_FALSE, glm::value_ptr(camera.GetView()));
    g_ground_mesh.Render();

    object_shader.Bind();
    glUniformMatrix4fv(object_shader["proj"], 1, GL_FALSE, glm::value_ptr(camera.GetProj()));
    glUniformMatrix4fv(object_shader["view"], 1, GL_FALSE, glm::value_ptr(camera.GetView()));
    g_psystem->Render();
    */
}

//=============================================================================
// 
//=============================================================================
