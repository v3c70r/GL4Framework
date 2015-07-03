#ifndef ARCBALL_H
#define ARCBALL_H
#include "camera.h"
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <cmath>
const unsigned int CAMERA_ARCBALL=0;
class Arcball: public Camera
{
private:
    glm::ivec2 startPos;
    glm::vec4 eyePos;
    bool dragged;
    void get3dPos(glm::vec3 &output, const glm::ivec2 &pos );
public:
    void dragStart(const int &x, const int &y);
    void dragEnd();
    void dragUpdate(const int &x, const int &y);
    void rotate(const int &x, const int &y );
    bool isDragged();
    void zoomIn();
    void zoomOut();
};
#endif
