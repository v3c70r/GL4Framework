#pragma once
#include <GL/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>
#include <fstream>
#include <core/splitstring.h>
#include <vector>
#include <iostream>
#include <IL/il.h>
#include <IL/ilu.h>
#include <core/object.h>
#include <assimp/scene.h>
#include <assimp/material.h>



class MeshNode: public Object
{
private:
    enum MESH_ATTR
    {
        //attrb with fixed locations
        VERTICES=0,
        NORMALS,
        TEXCOORDS,
        WEIGHTS,
        IDS,
        INDICES,
        //uniform buffers
        MATERIAL,
        BONES_TRANS,
        OBJ_MATS,
        COUNT
    };
protected:
    GLuint numOfFaces;
    GLuint numOfVertices;
    GLuint texture;
    GLuint diffuseTexture;
    GLuint specularTexture;
public:
    MeshNode() {}

    ~MeshNode()
    {
        unbind();
        glDeleteBuffers(MESH_ATTR::COUNT, BUFFER);
        glDeleteVertexArrays(1, &VAO);
        glDeleteTextures(1, &texture);
        delete []BUFFER;
    }
    virtual void init(GLuint nFaces, GLuint nVertices);
    void setVertices(const GLfloat *vertices);
    void setNormals(const GLfloat *normals);
    void setTexCoord(const GLfloat *texCoord);
    void setIndices(const GLuint *indices);
    void loadSimpleOBJ(std::string objFile);
    void loadTexture(const std::string &fileName);
    void setMaterial(const aiMaterial *mat);
    void bindShader(Shader *s) override;
    void bind()
    {
        glBindVertexArray(VAO);
    }
    void unbind()
    {
        glBindVertexArray(0);
    }
    void update() override;
    void draw() override;
    GLuint getMaterialBuffer() const { return BUFFER[MESH_ATTR::MATERIAL];}
    GLuint getModelMatsBuffer() const { return BUFFER[MESH_ATTR::OBJ_MATS];}
};


