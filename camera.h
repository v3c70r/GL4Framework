#ifndef CAMERA_H
#define CAMERA_H

#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera
{
protected:
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
    Camera():translationMat(glm::mat4(1.0)), rotationMat(glm::mat4(1.0)), viewMat(glm::mat4(1.0))
    {
        near = 1.0f;
        far = 10000.0f;
        fov = 1.1693706f;
        aspect = 1.0f;
    }
    //Rotation and translations
    void addRotation(const glm::quat &rot);
    void addRotation(const glm::mat4 &rot);
    void addRotation(const float &angle, const glm::vec3 &axis);
    void addTranslation(const glm::vec3 &t);
    void setTranslationMat(const glm::mat4 &t);
    void setRotationMat(const glm::mat4 &r);
    void updateProjectionMat(int w, int h);
    glm::mat4 getViewMat() const;
    glm::mat4 getProjectionMat() const;
};
#endif
