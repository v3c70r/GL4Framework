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
    GLuint VAO;
    GLuint *BUFFER;
    Object* parent;
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
    Object():
        transMat(glm::mat4x4(1.0)),
        VAO(0),
        BUFFER(nullptr) ,
        parent(nullptr), 
        name("Sans nom")
    {}
    virtual ~Object(){};
    virtual void draw()=0;
    void setTransMat(const glm::mat4x4 &m){transMat = m;}

    /*
     * Bind Object UBOs to shader
     * Since every object may have different buffer objects,
     * it's better to implement this function in each derived classes.
     */
    virtual void bindShader(Shader *)=0;

    /*
     * Object need to be update at every frame.
     * Including positions( model matrix), and physics
     */
    virtual void update()=0;       
    void setParent(Object *obj){parent = obj;}
    const glm::mat4& getLocalTransMat() const{return transMat;}
};
