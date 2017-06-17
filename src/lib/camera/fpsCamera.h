#pragma once
#include "core/camera.h"
class FpsCamera : public Camera {
protected:
    float roll_;
    float pitch_;
    float yaw_;
    float strafe_;
    float forward_;
    glm::vec3 eyeVector_;

public:
    FpsCamera()
        : roll_(0.0),
          pitch_(0.0),
          yaw_(0.0),
          strafe_(0.0),
          forward_(0.0),
          eyeVector_(glm::vec3(0.0))
    {
    }
    void pitch(float angle) { pitch_ += angle; }
    void yaw(float angle) { yaw_ += angle; }
    void forward(float dist) { forward_ = dist; }
    void strafe(float dist) { strafe_ = dist; }
    void update()
    {
        glm::mat4 matRoll = glm::mat4(1.0f);   // identity matrix;
        glm::mat4 matPitch = glm::mat4(1.0f);  // identity matrix
        glm::mat4 matYaw = glm::mat4(1.0f);    // identity matrix
        // roll, pitch and yaw are used to store our angles in our class
        matRoll = glm::rotate(matRoll, roll_, glm::vec3(0.0f, 0.0f, 1.0f));
        matPitch = glm::rotate(matPitch, pitch_, glm::vec3(1.0f, 0.0f, 0.0f));
        matYaw = glm::rotate(matYaw, yaw_, glm::vec3(0.0f, 1.0f, 0.0f));
        // order matters
        glm::mat4 rotate = matRoll * matPitch * matYaw;
        glm::mat4 cameraMatrix = getViewMat();
        glm::vec3 forward(cameraMatrix[0][2], cameraMatrix[1][2],
                          cameraMatrix[2][2]);
        glm::vec3 strafe(cameraMatrix[0][0], cameraMatrix[1][0],
                         cameraMatrix[2][0]);
        eyeVector_ += (-forward_ * forward + strafe_ * strafe);
        forward_ = 0;
        strafe_ = 0;
        glm::mat4 translate = glm::mat4(1.0f);
        translate = glm::translate(translate, -eyeVector_);
        setRotationMat(rotate);
        setTranslationMat(translate);
    }
    glm::vec3 getCameraPosition() const { return eyeVector_ - glm::vec3(0.0, 2.0, 0.0); }
    glm::vec3 getLookAtDir() const
    {
        glm::mat4 cameraMatrix = getViewMat();
        glm::vec3 forward(cameraMatrix[0][2], cameraMatrix[1][2],
                          cameraMatrix[2][2]);
        //glm::vec3 strafe(cameraMatrix[0][0], cameraMatrix[1][0],
                         //cameraMatrix[2][0]);
        return glm::normalize((forward));
    }
};
