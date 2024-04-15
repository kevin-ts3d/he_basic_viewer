////////////////////////////////////////////////////////////////////////////////
/// Basic Mesh Viewer                                                        ///
/// #################                                                        ///
///                                                                          ///
/// This sample provides a single source CAD viewer using HOOPS Exchange.    ///
/// The codes makes use of the traversal API and `A3DMeshData` to send mesh  ///
/// data to OpenGL Graphics API                                              ///
///                                                                          ///
/// For more information see:                                                ///
///                                                                          ///
/// * docs.techsoft3d.com/exchange/latest/tutorials/mesh-viewer-sample.html  ///
/// * docs.techsoft3d.com/exchange/latest/tutorials/basic_viewing.html       ///
////////////////////////////////////////////////////////////////////////////////
#include <cassert>
#include <iostream>

#include "interface.hpp"


#define INITIALIZE_A3D_API
#include <A3DSDKIncludes.h>

////////////////////////////////////////////////////////////////////////////////
/// INPUT_FILE
/// Default CAD input file, relative to Exchange sample data folder.
/// To see how the value is used, check the `main()` function.
#define INPUT_FILE "/prc/_micro engine.prc" 

////////////////////////////////////////////////////////////////////////////////
// Recursive tree traversal function used to retrieve drawable entities
void he_traverse_tree(A3DTree* const hnd_tree, A3DTreeNode* const hnd_node, TraverseData* const traverse_data, size_t depth);

////////////////////////////////////////////////////////////////////////////////
// Utility function to convert A3DMiscTransformation object to 4x4 transform matrix
void he_extract_position(A3DTreeNode* const hnd_node, mat4x4 mat_result);

////////////////////////////////////////////////////////////////////////////////
// Utility function to get the net color of an entity
void he_extract_color(A3DTreeNode* const hnd_node, vec4 color_result);


////////////////////////////////////////////////////////////////////////////////
/// MAIN FUNCTION
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{

 
    TraverseData     traverse_data;    //structure to store all the data required for rendering

    //Next section uses HOOPS Exchange to import and extract the data
    {

        /////////////////////////////////////////////////////
        // INITIALIZE HOOPS EXCHANGE AND LOAD THE MODEL FILE.
        A3DSDKHOOPSExchangeLoader he_loader(HE_BINARY_DIRECTORY);
        assert(he_loader.m_eSDKStatus == A3D_SUCCESS);

        A3DImport he_import(HE_DATA_DIRECTORY INPUT_FILE);
        A3DStatus status = he_loader.Import(he_import);
        assert(status == A3D_SUCCESS);
        A3DAsmModelFile* model_file = he_loader.m_psModelFile;

        ////////////////////////////////////////////////////////
        // TRAVERSE THE MODEL TREE
       
        A3DTree* hnd_tree = 0;

        status = A3DTreeCompute(model_file, &hnd_tree, 0);
        assert(status == A3D_SUCCESS);

        A3DTreeNode* hnd_root_node = 0;
        status = A3DTreeGetRootNode(hnd_tree, &hnd_root_node);
        assert(status == A3D_SUCCESS);


        he_traverse_tree(hnd_tree, hnd_root_node, &traverse_data, 0);
       
        A3DTreeCompute(0, &hnd_tree, 0);
    }      
    //End of HOOPS Exchange processing


    //Sending the data to a rendering engine
    send_to_openGL(traverse_data);
   
    return EXIT_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// Recursive traversal function, initially called in main() on the root node.
// The function:
// - Dump the model tree information
// - Extracts any geometry as A3DMeshData 
// - Recursively calls itself on the child nodes
void he_traverse_tree(A3DTree* const hnd_tree, A3DTreeNode* const hnd_node, TraverseData* const traverse_data, size_t depth)
{

    //Start node dump
    std::cout << std::string(2 * depth, ' '); // Print indent

    // Display node's entity name
    std::cout << "Name: ";

    A3DUTF8Char* node_name = 0;
    if (A3DTreeNodeGetName(hnd_node, &node_name) == A3D_SUCCESS && node_name != 0) {
        std::cout << node_name << "; ";
        A3DTreeNodeGetName(0, &node_name);
    }
    else {
        std::cout << "N/A; ";
    }

    // Display node's entity type
    std::cout << "Type: ";

    A3DEntity* pEntity = nullptr;
    A3DTreeNodeGetEntity(hnd_node, &pEntity);
    A3DEEntityType eType = kA3DTypeUnknown;
    if (A3DEntityGetType(pEntity, &eType) == A3D_SUCCESS) {
        const A3DUTF8Char* pTypeMsg = A3DMiscGetEntityTypeMsg(eType);
        std::cout << pTypeMsg << "; ";
    }
    else {
        std::cout << "N/A; ";
    }

    // Extract the geometry as an A3DMeshData and collect their position and color to render it
    A3DMeshData mesh_data;
    A3D_INITIALIZE_DATA(A3DMeshData, mesh_data);
    A3DStatus code = A3DTreeNodeGetGeometry(hnd_tree, hnd_node, A3D_TRUE, &mesh_data, 0);

    if (code == A3D_SUCCESS) {
        MeshObject object;
        he_extract_position(hnd_node, object.matrix_position);
        he_extract_color(hnd_node, object.color);
        object.mesh = mesh_data;
        traverse_data->objects.push_back(object);
    }

    //end of node dump
    std::cout << std::endl;

    // Recursively traverse the child nodes
    A3DUns32 n_children = 0;
    A3DTreeNode** hnd_children = 0;

    code = A3DTreeNodeGetChildren(hnd_tree, hnd_node, &n_children, &hnd_children);
    assert(code == A3D_SUCCESS);
    for (size_t c = 0; c < n_children; ++c) {
        he_traverse_tree(hnd_tree, hnd_children[c], traverse_data, depth+1);
    }
    A3DTreeNodeGetChildren(0, 0, &n_children, &hnd_children);
}


void he_extract_color(A3DTreeNode* const hnd_node, vec4 color_result)
{
    // Get node's color index
    std::cout << "Color: ";

    A3DGraphStyleData node_style;
    A3D_INITIALIZE_DATA(A3DGraphStyleData, node_style);
    if (A3DTreeNodeGetNetStyle(hnd_node, &node_style) == A3D_SUCCESS)
    {
        A3DGraphRgbColorData color;
        A3D_INITIALIZE_DATA(A3DGraphRgbColorData, color);
        if (A3DGlobalGetGraphRgbColorData(node_style.m_uiRgbColorIndex, &color) == A3D_SUCCESS) {

            std::cout << "RGB(" << color.m_dRed << ";" << color.m_dGreen << ";" << color.m_dBlue << ")";

            color_result[0] = color.m_dRed;
            color_result[1] = color.m_dGreen;
            color_result[2] = color.m_dBlue; 
            color_result[3] = 1.0;

            A3DTreeNodeGetNetStyle(0, &node_style);
        }
        else {
            std::cout << "N/A; "; //default color
            color_result[0] = 0.7;
            color_result[1] = 0.7;
            color_result[2] = 0.7;
            color_result[3] = 1.0;
        }
    }
}

void he_extract_position(A3DTreeNode* const hnd_node, mat4x4 mat_result)
{
    //Extract the net transformation for the node
    A3DMiscTransformation* hnd_net_transform = 0;
    A3DTreeNodeGetNetTransformation(hnd_node, &hnd_net_transform);

    ///then computes a column-major 4x4 transformation matrix out
    /// of an `A3DMiscTransformation` entity. 
    /// The result is ready to be send to our graphics API.

    if (hnd_net_transform == 0) {
        mat4x4_identity(mat_result);
    }
    else {
        A3DEEntityType entity_type = kA3DTypeUnknown;
        A3DEntityGetType(hnd_net_transform, &entity_type);
        assert(entity_type == kA3DTypeMiscCartesianTransformation);

        A3DMiscCartesianTransformationData data;
        A3D_INITIALIZE_DATA(A3DMiscCartesianTransformationData, data);
        A3DStatus code = A3DMiscCartesianTransformationGet(hnd_net_transform, &data);
        assert(code == A3D_SUCCESS);


        vec3 x_vector = { (float)data.m_sXVector.m_dX, (float)data.m_sXVector.m_dY, (float)data.m_sXVector.m_dZ };
        vec3 y_vector = { (float)data.m_sYVector.m_dX, (float)data.m_sYVector.m_dY, (float)data.m_sYVector.m_dZ };
        vec3 z_vector;
        vec3_mul_cross(z_vector, x_vector, y_vector);

        double mirror = (data.m_ucBehaviour & kA3DTransformationMirror) ? -1. : 1.;

        mat_result[0][0] = data.m_sXVector.m_dX * data.m_sScale.m_dX;
        mat_result[0][1] = data.m_sXVector.m_dY * data.m_sScale.m_dX;
        mat_result[0][2] = data.m_sXVector.m_dZ * data.m_sScale.m_dX;
        mat_result[0][3] = 0.;

        mat_result[1][0] = data.m_sYVector.m_dX * data.m_sScale.m_dY;
        mat_result[1][1] = data.m_sYVector.m_dY * data.m_sScale.m_dY;
        mat_result[1][2] = data.m_sYVector.m_dZ * data.m_sScale.m_dY;
        mat_result[1][3] = 0.;

        mat_result[2][0] = mirror * z_vector[0] * data.m_sScale.m_dZ;
        mat_result[2][1] = mirror * z_vector[1] * data.m_sScale.m_dZ;
        mat_result[2][2] = mirror * z_vector[2] * data.m_sScale.m_dZ;
        mat_result[2][3] = 0.;

        mat_result[3][0] = data.m_sOrigin.m_dX;
        mat_result[3][1] = data.m_sOrigin.m_dY;
        mat_result[3][2] = data.m_sOrigin.m_dZ;
        mat_result[3][3] = 1.;

        A3DMiscCartesianTransformationGet(0, &data);
    }

}

////////////////////////////////////////////////////////////////////////////////
/// This utility function 



