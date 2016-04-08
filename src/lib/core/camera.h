#ifndef CAMERA_H
#define CAMERA_H

#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "shader.hpp"

class Camera
{
protected:
    GLuint UBO;     //Uniform buffer object to pass buffer to shader
    const GLuint UBO_SIZE;
    const GLuint PROJ_MAT_OFFSET;
    const GLuint VIEW_MAT_OFFSET;
    const GLuint VIEW_MAT_INV_OFFSET;
    const GLuint MAT_SIZE;
    glm::mat4 translationMat;
    glm::mat4 rotationMat;
    glm::mat4 viewMat;
    glm::mat4 projectionMat;
    // input variables
    float near;
    float far;
    float fov;
    float aspect ;
    GLint viewport[4];
public:
    Camera():
        UBO(0),
        UBO_SIZE(sizeof(GLfloat) * 48),  //Three mat4s
        PROJ_MAT_OFFSET(0),
        VIEW_MAT_OFFSET(sizeof(GLfloat) * 16),
        VIEW_MAT_INV_OFFSET(sizeof(GLfloat)*32),
        MAT_SIZE(sizeof(GLfloat) * 16),
        translationMat(glm::mat4(1.0)), 
        rotationMat(glm::mat4(1.0)),
        viewMat(glm::mat4(1.0)),
        projectionMat(glm::mat4(1.0)),
        near(1.0),
        far(10000.0),
        fov(1.1693706),
        aspect(1.0)

    {
    }
    void init();
    //Rotation and translations
    void addRotation(const glm::quat &rot);
    void addRotation(const glm::mat4 &rot);
    void addRotation(const float &angle, const glm::vec3 &axis);
    void addTranslation(const glm::vec3 &t);
    void setTranslationMat(const glm::mat4 &t);
    void setRotationMat(const glm::mat4 &r);
    void updateProjectionMat(int w, int h);
    void updateViewMat();
    void bindToShader(Shader *shdr) const;
    glm::mat4 getViewMat() const;
    glm::mat4 getProjectionMat() const;
};
#endif
