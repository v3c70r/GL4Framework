#pragma once
#include <GL/glew.h>
#include <objects/object.h>

class GLFluid: public Object
{
private:
    const size_t MAX_NUM_POINTS;
    enum VertexBuffers
    {
        INDICES=0,
        CELLIDS,
        POSITIONS_VELOCITIES,
        COUNT
    };
    const GLenum TYPES[COUNT] = 
    {
    };
    const size_t BF_SZs[COUNT] =
    {
        sizeof(GLuint) * MAX_NUM_POINTS,
        sizeof(GLuint) * MAX_NUM_POINTS,
        sizeof(GLfloat) * MAX_NUM_POINTS * 8,
    };
    const GLenum USAGES[COUNT] = 
    {
        GL_DYNAMIC_DRAW,
        GL_DYNAMIC_DRAW,
        GL_DYNAMIC_DRAW,
    };
protected:
    GLuint numPoints;   
    GLuint pongVAO;
    GLuint pingVAO;
    bool isPing;
public:
    GLFluid();

    // Implementing interfaces
    void bindShader(Shader *) override;
    void draw() override;
};
