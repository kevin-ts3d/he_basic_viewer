#pragma once

#include <unordered_map>
#include <vector>

#include <glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"

#include <A3DSDKIncludes.h>

////////////////////////////////////////////////////////////////////////////////
// Represents a drawable object in the GPU.
// Created from an `A3DMeshData` instance during `he_mesh_data_to_opengl()`.
struct oglSceneObject {
    mat4x4  mat_transform_model;  // Transformation matrix for the object.
    vec4    color;                // Color of the object
    GLuint  gl_vao;               // OpenGL Vertex Array Object ID.
    GLsizei gl_indices_count;     // Number of indices in the buffer.

};

////////////////////////////////////////////////////////////////////////////////
// Holds information about all objects and resources for drawing.
// Uses an associative array to link each representation item to its drawable object.
struct oglTraverseData {
    std::vector<oglSceneObject> objects; // All GPU objects in the scene.

    // Associates each representation item with its corresponding `SceneObject`.
    std::unordered_map<int, std::pair<GLuint, GLsizei>> ri_to_gl;
};

////////////////////////////////////////////////////////////////////////////////
/// Window/Graphics API Functions
////////////////////////////////////////////////////////////////////////////////
void        ogl_rendering_loop(GLFWwindow*, GLuint, const oglSceneObject*, size_t);
GLuint      ogl_rendering_prepare();
GLFWwindow* ogl_rendering_prepare_window();
void        ogl_rendering_error_callback(int, const char*);
void        ogl_rendering_key_callback(GLFWwindow*, int, int, int, int);
void        ogl_rendering_cleanup(GLuint);
void        ogl_rendering_cleanup_window(GLFWwindow*);
GLuint      ogl_rendering_to_gpu(const std::vector<GLuint>& index_buffer, const std::vector<GLdouble>& vertex_buffer, const std::vector<GLdouble>& normal_buffer);
