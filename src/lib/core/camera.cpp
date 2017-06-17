#include "camera.h"

void Camera::init()
{
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, UBO_SIZE, 0, GL_DYNAMIC_DRAW);
    //set number of lights to zero
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void Camera::addRotation(const glm::quat &rot)
{
    //Gonna implement later
}
void Camera::addRotation(const glm::mat4 &rot)
{
    rotationMat = rot * rotationMat;
}
void Camera::addRotation(const float &angle, const glm::vec3 &axis)
{
    rotationMat = glm::rotate(rotationMat, angle, axis);
}
void Camera::addTranslation(const glm::vec3 &t)
{
    translationMat = glm::translate(translationMat, t);
}

glm::mat4 Camera::getViewMat() const
{
    return rotationMat * translationMat ; 
}
void Camera::setTranslationMat(const glm::mat4 &t)
{
    translationMat = t;
    
}
void Camera::setRotationMat(const glm::mat4 &r)
{
    rotationMat = r;
}

void Camera::updateProjectionMat(int w, int h)
{
    int ratio = 1;
    aspect =(float)w / (float)h;
    glViewport(0.0, 0.0, (float)w*ratio, (float)h*ratio);
    glGetIntegerv(GL_VIEWPORT, viewport);       //update viewport
    projectionMat = glm::perspective(fov, aspect, near, far);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, PROJ_MAT_OFFSET, MAT_SIZE, &projectionMat[0][0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::updateViewMat()
{
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, VIEW_MAT_OFFSET, MAT_SIZE, &(rotationMat*translationMat)[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, VIEW_MAT_INV_OFFSET, MAT_SIZE, &(glm::inverse(rotationMat*translationMat))[0][0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::mat4 Camera::getProjectionMat() const
{
    return projectionMat;
}

void Camera::bindToShader(Shader  * shdr)
{
    shdr->bindCameraMats(UBO);
}
