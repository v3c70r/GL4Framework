#include "camera.h"

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
    return translationMat * rotationMat ; 
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
}

glm::mat4 Camera::getProjectionMat() const
{
    return projectionMat;
}
