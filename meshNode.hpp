#pragma once
#include <GL/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>
#include <fstream>
#include "splitstring.h"
#include <vector>
#include <iostream>
#include <IL/il.h>
#include <IL/ilu.h>
#include "object.h"
#include <assimp/scene.h>
#include <assimp/material.h>

namespace MESH_ATTR{
    enum MESH_ATTR
    {
        VERTICES=0,
        NORMALS,
        TEXCOORDS,
        INDICES,
        MATERIAL,
        WEIGHTS,
        BONES_TRANS,
        COUNT
    };
}

class MeshNode: public Object
{
protected:
    GLuint numOfFaces;
    GLuint numOfVertices;
    GLuint texture;
public:
    const int MAX_NUM_BONES;
    MeshNode(): MAX_NUM_BONES(20)
    {
        VBO = new GLuint[MESH_ATTR::COUNT];
    }

    ~MeshNode()
    {
        unbind();
        glDeleteBuffers(MESH_ATTR::COUNT, VBO);
        glDeleteVertexArrays(1, &VAO);
        glDeleteTextures(1, &texture);
        delete []VBO;
    }
    void init(GLuint nFaces, GLuint nVertices, GLuint numBones);
    void setVertices(const GLfloat *vertices);
    void setWeights(const GLfloat* weights);
    void setNormals(const GLfloat *normals);
    void setTexCoord(const GLfloat *texCoord);
    void setIndices(const GLuint *indices);
    void loadSimpleOBJ(std::string objFile);
    void loadTexture(const std::string &fileName);
    void setMaterial(const aiMaterial *mat);
    void setShader(Shader *s);
    void bind()
    {
        glBindVertexArray(VAO);
    }
    void unbind()
    {
        glBindVertexArray(0);
    }
    void draw();
};


