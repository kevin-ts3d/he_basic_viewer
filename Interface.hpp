#pragma once

#include <unordered_map>
#include <vector>

#include <glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"
#include <set>

#include <A3DSDKIncludes.h>

#include "ogl_rendering.hpp"


////////////////////////////////////////////////////////////////////////////////
// Represents a drawable object in the GPU.
// Created from an `A3DMeshData` instance during `he_mesh_data_to_opengl()`.
struct MeshObject {
    mat4x4  matrix_position;  // Position of the object.
    A3DMeshData mesh;
    vec4 color;
};

////////////////////////////////////////////////////////////////////////////////
// Holds information about all objects and resources for drawing.
// Uses an associative array to link each representation item to its drawable object.
struct TraverseData {
    std::vector<MeshObject> objects; // All GPU objects in the scene.
};


////////////////////////////////////////////////////////////////////////////////
// OpenGL Inteface
// Prepare data before sending to OpenGL (buffers creation) 
std::pair<GLuint, GLsizei> ogl_mesh_data_to_rendering(A3DMeshData const mesh_data);
void send_to_openGL(TraverseData traverse_data);
