#include "compMesh.hpp"
#include <OpenMesh/Core/IO/MeshIO.hh>

ComputationalMeshNode::ComputationalMeshNode(std::string fileName, std::string nodeName)
{
    if (!OpenMesh::IO::read_mesh(mesh, fileName))
        throw std::runtime_error("Error while loading mesh\n");
    //Allocating GPU data (Vertices and normal buffer)
    //mesh->numOfVertices;
    glGenBuffers(MESH_BUFFERS::COUNT, buffers);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffers[MESH_BUFFERS::VERTICES_NROMALS]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*mesh->numOfVertices, , GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_ATTR::VERTICES);
    glVertexAttribPointer(MESH_ATTR::VERTICES, 3, GL_FLOAT, 0, 0, 0);

    

    
    
    
    
}


/*
#include <OpenMesh/Core/IO/MeshIO.hh>
MyMesh mesh;
if (!OpenMesh::IO::read_mesh(mesh, "some input file")) 
{
std::cerr << "read error\n";
exit(1);
}
do something with your mesh ...
if (!OpenMesh::IO::write_mesh(mesh, "some output file")) 
{
std::cerr << "write error\n";
exit(1);
}
*/

