#include "arcball.h"
#include <core/ImguiConfig.hpp>

void Arcball::get3dPos(glm::vec3 &output, const glm::ivec2 &pos )
{
    output = glm::vec3(1.0*pos.x/viewport[2]*2 - 1.0,
            1.0*pos.y/viewport[3]*2 - 1.0,
            0);
    output.y = -output.y;

    output.z = 0.5;
    output = glm::vec3((glm::vec4(output, 1.0) * translationMat * rotationMat));
}

void Arcball::dragStart(const int &x, const int &y)
{
    dragged = true;
    dragUpdate(x, y);
}

void Arcball::dragUpdate(const int &x, const int &y)
{
    if (dragged)
    {
        startPos.x = x;
        startPos.y = y;
    }
}

void Arcball::rotate(const int &x, const int &y )
{
    if (x != startPos.x && y != startPos.y)
    {
        glm::vec3 currPos;
        glm::vec3 prePos;
        get3dPos(currPos, glm::ivec2(x,y));
        get3dPos(prePos, startPos);
        glm::vec3 rotAxis = glm::cross(prePos, currPos);
        float angle = acos(fmin(1.0, glm::dot(glm::normalize(prePos), glm::normalize(currPos))));
        rotationMat = glm::rotate(rotationMat, angle, glm::normalize(rotAxis));
    }
}

/**
 * @brief Rotate arcball with joystick
 *
 * @param x joystick x axis within [-1.0, 1.0]
 * @param y joystick y axis within [-1.0, 1.0]
 */
void Arcball::rotateJoystick( const float &x, const float &y)
{
    glm::vec3 prePos(0.0, 0.0, -1.0);
    glm::vec3 currPos(std::trunc(x*10)/10.0, std::trunc(y*10)/10.0, -1.0);
    if (currPos != prePos)
    {
        glm::vec3 rotAxis = glm::cross(prePos, currPos);
        ImGui::Text("Rotaxis %f, %f, %f\n", rotAxis.x, rotAxis.y, rotAxis.z);
        float angle = 0.1 * (x*x+y*y);
        rotationMat = glm::rotate(rotationMat, angle, glm::normalize(rotAxis));
    }
}
bool Arcball::isDragged()
{
    return dragged;
}
void Arcball::dragEnd()
{
    dragged= false;
}

void Arcball::zoomIn()
{
    translationMat = glm::translate(translationMat, glm::vec3(0.0f, 0.0f, -0.1f));
}

void Arcball::zoomOut()
{
    translationMat = glm::translate(translationMat, glm::vec3(0.0f, 0.0f, 0.1f));
}

