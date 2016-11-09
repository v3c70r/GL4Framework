#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <core/shader.hpp>

/*
 * This is the super class of all object in scene
 * An object can be a mesh, a light source etc.
 */
class Object
{
protected:
    glm::mat4 transMat_;   //transformation matrix
    Object *parent_;
    std::string name_;
    /*
     * Get global transformation matrix by bottom up parsing the tree
     */
    glm::mat4x4 getGlobalTransMat()
    {
        glm::mat4x4 res = transMat_;
        Object *p = this->parent_;
        while(p != nullptr)
        {
            res = p->transMat_ * res;
            p = p->parent_;
        }
        return res;
    }
public:
    void setName(std::string n){name_ = n;}
    std::string getName() const { return name_;}
    Object():
        transMat_(glm::mat4x4(1.0)),
        parent_(nullptr), 
        name_("Sans nom")
    {}
    virtual ~Object(){};
    virtual void draw()=0;

    void setTransMat(const glm::mat4x4 &m){transMat_ = m;}

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
    void setParent(Object *obj){parent_ = obj;}
    const glm::mat4& getLocalTransMat() const{return transMat_;}
};
