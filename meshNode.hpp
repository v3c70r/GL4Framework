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
        COUNT
    };
}

class MeshNode: public Object
{
private:
    GLuint numOfFaces;
    GLuint numOfVertices;
    GLuint texture;
    //Store locations for fast access;
    GLint matBlockIdx;
public:
    MeshNode()
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
    void init(GLuint nFaces, GLuint nVertices);
    void setVertices(const GLfloat *vertices);
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


