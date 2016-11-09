/*
 * Computational Mesh class using open mesh
 */
#pragma once
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <objects/object.h>

//typedef OpenMesh::TriMesh_ArrayKernelT<> TriMesh;
//
//class ComputationalMeshNode: public Object
//{
//    struct MESH_BUFFERS
//    {
//        enum ENUM
//        {
//            VERTICES=0,
//            NORMALS,
//            COUNT
//        };
//    };
//    struct MESH_ATTRS
//    {
//        enum ENUM
//        {
//            VERTICES = 0,
//            NORMALS,
//            COUNT
//        };
//    };
//    TriMesh mesh;
//    GLuint buffers[MESH_BUFFERS::COUNT];
//public:
//    //Initialize computational shader with a file
//    ComputationalMeshNode(std::string fileName, std::string nodeName);
//
//    //Implementing iterfaces
//    ~ComputationalMeshNode();
//    void draw();
//    void bindShader(Shader *);
//    void update();
//};

