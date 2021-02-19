#include <GLFW/glfw3.h>

#include <cstdlib>  // For EXIT_SUCCESS/FAILURE
#include <iostream> // For cout/cerr

#define GLM_ENABLE_EXPERIMENTAL

#include <cassert>
#include <sstream>
#include <algorithm>
#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>

#include "util.hpp"
#include "chunk.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "application.hpp"
#include "frustum.hpp"
#include "chunk_manager.hpp"
#include "texture.hpp"
#include "grass_texture.hpp"
#include "job_system.hpp"

using std::cerr;
using std::cout;

// Window size
int window_width  = 800;
int window_height = 800;

// Simulation variables
const double SIM_FPS = 120.0;
const double SIM_DT  = 1.0 / SIM_FPS;
const double SIM_MAX_DT = 10.0 * SIM_DT;

// Change these to change what version of OpenGL to use
const int desired_major_version = 4;
const int desired_minor_version = 6;

Camera camera;

// Called when the window is resized. We need to tell the
// application that the window sizk has changed so it can
// update some internal information, like FBOs.
//
void window_resize(GLFWwindow*, int width, int height)
{
    //resolution_changed(width, height);
    window_width = width;
    window_height = height;
    glViewport(0, 0, width, height);

    camera.SetPerspective(45.0f, (float)width/height, 1.0f, 300.0f);
}

const glm::vec3 voxelSize(0.5, 0.5, 0.5);
const glm::vec3 cubeCoordScale(255, 255, 255);

enum Vertex { FTL=0, FTR, FBL, FBR, BBR, BTR, BTL, BBL };

GLuint create_point_template()
{
    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const std::vector<glm::vec3> vec = {
        glm::vec3(0, 0, 0),
    };

    std::vector<float> vertices;
    for (const auto& v : vec)
    {
        vertices.push_back(v[0]);
        vertices.push_back(v[1]);
        vertices.push_back(v[2]);
        std::cout << glm::to_string(v) << std::endl;
    }

    const size_t size = vertices.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, size, vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    checkGLErrors();

    return vao;
}

GLuint create_cube_template_geom()
{
    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const std::vector<glm::vec3> vec_ = {
        glm::vec3(-voxelSize.x,  voxelSize.y, -voxelSize.z),
        glm::vec3( voxelSize.x,  voxelSize.y, -voxelSize.z),
        glm::vec3(-voxelSize.x, -voxelSize.y, -voxelSize.z),
        glm::vec3( voxelSize.x, -voxelSize.y, -voxelSize.z),
        glm::vec3( voxelSize.x, -voxelSize.y,  voxelSize.z),
        glm::vec3( voxelSize.x,  voxelSize.y, -voxelSize.z),
        glm::vec3( voxelSize.x,  voxelSize.y,  voxelSize.z),
        glm::vec3(-voxelSize.x,  voxelSize.y, -voxelSize.z),
        glm::vec3(-voxelSize.x,  voxelSize.y,  voxelSize.z),
        glm::vec3(-voxelSize.x, -voxelSize.y, -voxelSize.z),
        glm::vec3(-voxelSize.x, -voxelSize.y,  voxelSize.z),
        glm::vec3( voxelSize.x, -voxelSize.y,  voxelSize.z),
        glm::vec3(-voxelSize.x,  voxelSize.y,  voxelSize.z),
        glm::vec3( voxelSize.x,  voxelSize.y,  voxelSize.z),
    };

    std::vector<float> vec;
    for (const auto& v : vec_)
    {
        vec.push_back(v[0]);
        vec.push_back(v[1]);
        vec.push_back(v[2]);
    }

    const size_t size = vec.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, size, vec.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    checkGLErrors();

    return vao;
}

GLuint create_cube_template()
{
    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const std::vector<unsigned char> vec = {
        FTL,
        FTR,
        FBL,
        FBR,
        BBR,
        FTR,
        BTR,
        FTL,
        BTL,
        FBL,
        BBL,
        BBR,
        BTL,
        BTR,
    };

    const size_t size = vec.size() * sizeof(unsigned char);
    glBufferData(GL_ARRAY_BUFFER, size, vec.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 1, GL_UNSIGNED_BYTE, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    checkGLErrors();

    return vao;
}

GLuint load_test_texture()
{
    // Pixel store matters here because each row must start on the
    // defined alignment. For example:
    // GL_RGB with unsigned byte and 9 pixels = 27 bytes, but the next
    // row will start at byte 32 if GL_PACK/UNPACK_ALIGNMENT is 8

    GLuint test_texture = 0;
    glGenTextures(1, &test_texture);
    glBindTexture(GL_TEXTURE_2D, test_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    const uint8_t data[] = {
        0xff, 0xff, 0x00, // Left
        0x00, 0xff, 0xff, // Back
        0x00, 0xff, 0x00, // Top
        0x00, 0x00, 0xff, // Bottom
        0xff, 0x00, 0xff, // Right
        0xff, 0x00, 0x00, // Front
    };

    glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGB,
            24,
            16,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            &grass_texture_data[0]);
    return test_texture;
};

GLFWwindow* window;
GLuint cube_vao;
GLuint point_vao;
Shader voxelShader;
bool sortCulled = false;
bool mouseIsEnabled = true;
bool addBlock = false;
bool removeBlock = false;
bool collisionOn = false;
bool jump = false;
bool updatePosition = true;
bool forceUpdateChunks = false;
bool showPreviewCube = false;

void mouse_fn(GLFWwindow*, int button, int action, int)
{
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
    {
        addBlock = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
    {
        removeBlock = true;
    }
}

void key_fn(GLFWwindow*, int key, int, int action, int)
{
    if (key == GLFW_KEY_U && action == GLFW_PRESS)
    {
        forceUpdateChunks = true;
    }
    else if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        collisionOn = !collisionOn;
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        jump = true;
    }
    else if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        updatePosition = !updatePosition;
    }
    else if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        mouseIsEnabled = !mouseIsEnabled;
        const auto cursor = mouseIsEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN;
        glfwSetInputMode(window, GLFW_CURSOR, cursor);
        printf("Mouse enabled %d\n", mouseIsEnabled);
    }
    else if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        showPreviewCube = !showPreviewCube;
    }
    else if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        sortCulled = !sortCulled;
        printf("Sort culled %d\n", sortCulled);
    }
}

void debugCallback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar *message,
        const void *userParam)
{
    printf("OPENGL DEBUG MESSAGE: %s\n", message);
}

int totalCubes = 0;
int cullCount = 0;

int main(int, char**)
{
    // Initialize the library
    if (!glfwInit())
        return -1;

    // Set the version of OpenGL we want to use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, desired_major_version);   
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, desired_minor_version);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(window_width, window_height, "Cubes", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    const int maj = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    const int min = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    std::cout << "OpenGL version " << maj << "." << min << std::endl;

    // Called when the user changes the window size
    glfwSetWindowSizeCallback(window, window_resize);

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSwapInterval(1);

    const bool shaders_loaded = voxelShader.LoadShaders("glsl/voxel.vert", "glsl/voxel.frag");

    if (!shaders_loaded)
    {
        std::cerr << "Failed to load shaders" << std::endl;
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);
    //glDisable(GL_MULTISAMPLE);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    int data[2];
    glGetIntegerv(GL_POINT_SIZE_RANGE, &data[0]);
    printf("Point size range %d %d\n", data[0], data[1]);

    camera.SetPerspective(45.0f, float(window_width)/window_height, 1.0f, 300.0f);
    camera.SetPos(glm::vec3(0, 0, 0));

    cube_vao = create_cube_template();
    point_vao = create_point_template();

    TextureTracker preview_cube;
    const uint8_t single_loc[] = {
        0, 0, 0, 1
    };
    const uint8_t single_mat[] = {
        0
    };
    preview_cube.UploadTextureInfo(single_loc, single_mat, 1);

    glBindVertexArray(cube_vao);

    voxelShader.Bind();

    checkGLErrors();
    glDebugMessageCallback(&debugCallback, NULL);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

    glm::dvec2 cursor;
    glfwGetCursorPos(window, &cursor.x, &cursor.y);
    glm::dvec2 cursor_last = cursor;
    
    JobSystem jobs;
    jobs.Init();
    jobs.Start();

    glfwSetKeyCallback(window, key_fn);
    glfwSetMouseButtonCallback(window, mouse_fn);

    const GLuint test_texture = load_test_texture();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, test_texture);

    double previous_time = glfwGetTime();
    double sim_time = 0;
    int cubeCount = 0;
    double start_time = glfwGetTime();
    int frames = 0;
    float cullTime = 0;
    float swapTime = 0;
    double intersect_time = 0;
    double totalFrameTime = glfwGetTime();
    float updateWorldCenterTime = 0;
    double lastFPS = glfwGetTime();
    double renderTime = 0;
    glm::vec3 blockPos1;
    // Loop until the user closes the window
    ChunkManager chunkManager(glm::ivec3(8, 8, 8));
    timeit("Chunk Manager init", [&]() {
        chunkManager.init();
    });
    std::vector<const Chunk*> nonCulledObjects;
    //glClearColor(0.39215686274509803, 0.5843137254901961, 0.9294117647058824, 0);
    glClearColor(1.0, 1.0, 1.0, 0.0);
    while (!glfwWindowShouldClose(window))
    {
        // Get the elapsed time since the last update
        double current_time = glfwGetTime();
        const double dt = current_time - previous_time;//std::min(current_time - previous_time, SIM_MAX_DT);
        const double t = current_time;
        previous_time = current_time;

        sim_time += dt;
        if (sim_time > SIM_MAX_DT)
        {
            sim_time = SIM_MAX_DT;
        }

        while (sim_time >= SIM_DT)
        {
            glfwGetCursorPos(window, &cursor.x, &cursor.y);
            const glm::dvec2 cursor_diff = cursor - cursor_last;
            cursor_last = glm::dvec2(window_width/2, window_height/2);

            sim_time -= SIM_DT;

            const auto canMoveTo = [&](const glm::vec3& p) {
                cube_info* ci = chunkManager.getBlockAt(p);
                return ci != nullptr && ci->type == CubeType::Air;
            };

            if (mouseIsEnabled)
            {
                
                float speed = 5*SIM_DT;
                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                {
                    speed *= 4.f;
                }

                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                {
                    camera.MoveForward(speed);
                    if (collisionOn && !canMoveTo(camera.GetPosition()))
                    {
                        camera.MoveForward(-speed);
                    }
                }

                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                {
                    camera.MoveForward(-speed);
                    if (collisionOn && !canMoveTo(camera.GetPosition()))
                    {
                        camera.MoveForward(speed);
                    }
                }

                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                {
                    camera.Strafe(-speed);
                    if (collisionOn && !canMoveTo(camera.GetPosition()))
                    {
                        camera.Strafe(speed);
                    }
                }

                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                {
                    camera.Strafe(speed);
                    if (collisionOn && !canMoveTo(camera.GetPosition()))
                    {
                        camera.Strafe(-speed);
                    }
                }

                camera.Rotate(cursor_diff.x*M_PI*2.0*SIM_DT*0.1, 
                              cursor_diff.y*M_PI*2.0*SIM_DT*0.1);

                glfwSetCursorPos(window, window_width/2, window_height/2);
            }

            if (collisionOn)
            {
                const glm::vec3 gravity(0, -0.1f * dt, 0);
                const glm::vec3 cpos = camera.GetPosition() - camera.GetUp()*3.f;
                cube_info* ci = chunkManager.getBlockAt(cpos);
                if (canMoveTo(cpos + gravity))
                {
                    camera.SetPos(cpos + gravity);
                }
            }

            const glm::mat4 view = glm::inverse(camera.GetView());
            const auto pred = [](const ChunkManager::SearchResult& res) {
                return res.node->ci.type != CubeType::Air;// && res.full_dist < 10.0f;
            };
            const glm::vec3 rorig1 = glm::vec3(view*glm::vec4(0, 0, 0, 1)) - glm::vec3(0.5);
            const glm::vec3 rorig2 = glm::vec3(view*glm::vec4(0, 0, -1, 1)) - glm::vec3(0.5);
            const glm::vec3 rdir  = rorig2 - rorig1;
            const Ray ray(rorig1, rdir);

            const double intersect_start = glfwGetTime();
            const auto res = chunkManager.intersect_ray(ray, pred);
            intersect_time = (glfwGetTime() - intersect_start)*1000.0;

            const glm::vec3 cpos = camera.GetPosition();
            if (res.result.intersected)
            {
                blockPos1 = glm::floor(res.result.ray.origin) + glm::vec3(neighbor_offset(res.face));
            }
            else
            {
                blockPos1 = glm::vec3(-1);
            }
            const glm::ivec3 blockPos = blockPos1;
            if (addBlock && blockPos1 != glm::vec3(-1))
            {
                addBlock = false;
                cube_info ci;
                ci.type = CubeType::Solid;
                timeit("Add time + print", [&]() {
                    printf("At loc %d %d %d\n", blockPos.x, blockPos.y, blockPos.z);
                    printf("Added cube ? %d\n\n", chunkManager.setBlockAt(blockPos, ci));
                });
            }

            if (removeBlock && blockPos1 != glm::vec3(-1))
            {
                removeBlock = false;
                cube_info ci;
                ci.type = CubeType::Air;
                const glm::ivec3 removePos = blockPos1 - glm::vec3(neighbor_offset(res.face));
                timeit("Remove time + print", [&]() {
                    printf("At loc %d %d %d\n", removePos.x, removePos.y, removePos.z);
                    printf("Removed cube ? %d\n\n", chunkManager.setBlockAt(removePos, ci));
                });
            }
        }

        ++frames;
        if (current_time - lastFPS > 5)
        {
            printf("FPS %f - for cubes %d - culled %d/%d\n",
                    frames/5.f, cubeCount, cullCount, (int)chunkManager.chunks().size());
            printf("%d cubes/s %d cubes/ms\n",
                    cubeCount*frames,
                    cubeCount*frames/1000);
            printf("Cull time %fms\n", cullTime);
            printf("Render time %fms\n", renderTime);
            printf("Update world center time %fms\n", updateWorldCenterTime);
            printf("Intersect time %fms\n", intersect_time);
            double unaccountedTime = (totalFrameTime - (cullTime+renderTime+updateWorldCenterTime));
            printf("Unaccounted time %fms\n", unaccountedTime);
            printf("Total frame time %fms\n", totalFrameTime);
            printf("Swap time %fms\n", swapTime);
            printf("Swap+Total %fms\n", swapTime+totalFrameTime);

            lastFPS = current_time;
            frames = 0;
        }

        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkGLErrors();

        const glm::mat4 mvp = camera.GetProj()*camera.GetView();
        glUniformMatrix4fv(voxelShader["MVP"], 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform1i(voxelShader["cubeLocs"], 0);
        //glUniform1i(voxelShader["cubeMats"], 1);
        glUniform1i(voxelShader["texSamp"], 2);
        //glUniform1f(voxelShader["time"], current_time - start_time);
        checkGLErrors();

        if (showPreviewCube && blockPos1 != glm::vec3(-1))
        {
            const glm::ivec3 blockPos2 = blockPos1;
            preview_cube.Bind();
            glUniform1f(voxelShader["alpha"], 0.5f);
            glUniform3f(voxelShader["offset"], blockPos2.x, blockPos2.y, blockPos2.z);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 14, 1);
        }

        glUniform1f(voxelShader["alpha"], 1.0f);

        cubeCount = 0;
        cullCount = 0;

        if (updatePosition)
        {
            timeit_noprint(updateWorldCenterTime, [&]() {
                chunkManager.updateWorldCenter(camera.GetPosition());
            });
        }

        if (forceUpdateChunks)
        {
            forceUpdateChunks = false;
            for (const auto& chunk : chunkManager.chunks())
            {
                chunk.second->update();
            }
        }

        timeit_noprint(cullTime, [&]() {
                // TODO - keep track of non-visible ones and ignore them until
                // they change to speed up the culling process

                const frustum_planes planes = extract_frustum_planes(mvp);
                const auto stats = chunkManager.GetOccluder().cull(planes, nonCulledObjects);
                cubeCount = stats.cubeCount;
                cullCount = stats.cullCount;

                /*
                if (sortCulled)
                {
                    // Sort by position to camera
                    std::sort(nonCulledObjects.begin(),
                            nonCulledObjects.end(),
                            [&](const auto* obj1, const auto* obj2) {
                                const glm::vec3 o1c = obj1->aabb.center();
                                const glm::vec3 o2c = obj2->aabb.center();
                                const glm::vec3 cpos = camera.GetPosition();
                                return glm::length2(o1c - cpos) < glm::length2(o2c - cpos);
                            });
                }
                */
        });

        const double renderStartTime = glfwGetTime();
        for (const auto& robj : nonCulledObjects)
        {
            robj->textures.Bind();
            glUniform3f(voxelShader["offset"], robj->offset.x, robj->offset.y, robj->offset.z);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 14, robj->renderCount());
        }
        renderTime = (glfwGetTime() - renderStartTime)*1000.0;

        checkGLErrors();

        totalFrameTime = (glfwGetTime() - current_time)*1000.0;

        timeit_noprint(swapTime, [&]() {
            // Poll for and process events
            glfwPollEvents();

            // Swap front and back buffers
            glfwSwapBuffers(window);
        });
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
