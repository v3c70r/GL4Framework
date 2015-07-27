#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "shader.hpp"

/*
 * This is the super class of all object in scene
 * An object can be a mesh, a light source etc.
 */
class Object
{
protected:
    glm::mat4 transMat;   //transformation matrix
    glm::mat4 modelView;
    GLuint VAO;
    GLuint *VBO;
    Object* parent;
    Shader *shader;
    std::string name;
    /*
     * Get global transformation matrix by bottom up parsing the tree
     */
    glm::mat4x4 getGlobalTransMat()
    {
        glm::mat4x4 res = transMat;
        Object *p = this->parent;
        while(p != nullptr)
        {
            res = p->transMat * res;
            p = p->parent;
        }
        return res;
    }
public:
    void setName(std::string n){name = n;}
    std::string getName() const { return name;}
    Object():transMat(glm::mat4x4(1.0)),  parent(nullptr), name("Sans nom"){}
    virtual ~Object(){};
    virtual void draw()=0;
    void setTransMat(const glm::mat4x4 &m){transMat = m;}
    virtual void setShader(Shader *)=0;
    virtual void update()=0;       //I assume most objects require updates in each frame
    void setParent(Object *obj){parent = obj;}
    void setModelViewMat(const glm::mat4 &mv)
    {
        modelView = mv * getGlobalTransMat();
    }
    const glm::mat4& getLocalTransMat() const{return transMat;}
};
