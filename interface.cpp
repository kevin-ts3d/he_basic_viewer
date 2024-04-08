#include "interface.hpp"

void send_to_openGL(TraverseData data_traverse)
{

    ///////////////////////////
   // Draw the scene on-screent

   ///////////////////////////////////////////
   // Initialize window and graphics resources
    GLFWwindow* window = ogl_rendering_prepare_window();
    GLuint      program = ogl_rendering_prepare();
    
    //Adapt data to OpenGL
    oglTraverseData ogl_data_traverse;
    for (int i = 0; i < data_traverse.objects.size(); i++)
    {
        oglSceneObject object;

        auto pair = ogl_mesh_data_to_rendering(data_traverse.objects[i].mesh);
        object.gl_vao = pair.first;
        object.gl_indices_count = pair.second;
        mat4x4_dup(object.mat_transform_model, data_traverse.objects[i].mat_transform_model);
        vec4_dup(object.color, data_traverse.objects[i].color);

        ogl_data_traverse.objects.push_back(object);
    }

    ogl_rendering_loop(window, program, ogl_data_traverse.objects.data(), data_traverse.objects.size());

    /////////////////////////////////////////////
    // Clean up all window and graphics resources
    ogl_rendering_cleanup(program);
    ogl_rendering_cleanup_window(window);
   
}


////////////////////////////////////////////////////////////////////////////////
/// Pivot function that sends a mesh represented by `mesh_data` into the GPU.
/// The Graphics API uses OpenGL buffer.
/// This function first prepares the data for the buffer memory and stores the
/// buffer identifier into `data_traverse`.
/// The identifiers are used later on for drawing by `rendering_loop()`.
std::pair<GLuint, GLsizei> ogl_mesh_data_to_rendering(A3DMeshData const mesh_data)
{
    std::vector<GLdouble> vertex_buffer(mesh_data.m_pdCoords, mesh_data.m_pdCoords + mesh_data.m_uiCoordSize);
    std::vector<GLdouble> normal_buffer(mesh_data.m_pdNormals, mesh_data.m_pdNormals + mesh_data.m_uiNormalSize);
    
    // Count the total number of indices, which is equal to 3 times the sum of triangle count per face
    // The buffer objects will have at max n_indices indices
    size_t n_indices = 0;
    for (A3DUns32 face_i = 0; face_i < mesh_data.m_uiFaceSize; ++face_i) {
        n_indices += 3 * mesh_data.m_puiTriangleCountPerFace[face_i];
    }
    std::vector<GLuint> index_buffer(mesh_data.m_puiVertexIndicesPerFace, mesh_data.m_puiVertexIndicesPerFace + n_indices);

    GLuint renderable_id = ogl_rendering_to_gpu(index_buffer, vertex_buffer, normal_buffer);
    return { renderable_id, (GLsizei)index_buffer.size() };
}
