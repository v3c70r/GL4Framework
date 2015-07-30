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

//Each frame is an array of global(to this mesh) transformation for each bones
typedef std::vector<glm::mat4> Frame;
struct Animation
{
    std::string name;
    GLuint numBones;
    std::vector<Frame> frames;
};


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
        COUNT
    };
protected:
    vector<Animation> animations;
    GLuint numOfFaces;
    GLuint numOfVertices;
    GLuint texture;
public:
    const int MAX_NUM_BONES;
    const int BONES_PER_VERTEX;
    MeshNode(): MAX_NUM_BONES(20), BONES_PER_VERTEX(4)
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
    void setBoneTrans(const GLfloat* trans, const GLuint &numBones);
    void setVertices(const GLfloat *vertices);
    void setWeights(const GLuint *IDs, const GLfloat* weights);
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
    void update();
    void draw();
    void addAnimation(Animation anim);
};


