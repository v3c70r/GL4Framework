#pragma once
#include <GL/glew.h>
#include <core/object.h>

enum VertexBuffers
{
    INDICES=0,
    CELLIDS,
    POSITIONS,
    VELOCITIES,
    COUNT
};
class GLFluid: public Object
{
private:
    GLuint numPoints;   
    GLuint pongVAO;
    GLuint pingVAO;
    bool isPing;
    const size_t MAX_NUM_POINTS;
public:
    GLFluid();
};
