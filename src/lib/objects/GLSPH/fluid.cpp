#include <objects/GLSPH/fluid.h>
#include <GLFW/glfw3.h>
#include <stdexcept>


GLFluid::GLFluid() : MAX_NUM_POINTS(6556)
{
    if (glfwGetCurrentContext() == nullptr)
        throw std::runtime_error("Context is not created, please init this object after initialization of OpenGL context");
    // Allocate resources 
    glGenBuffers (COUNT, BUFFER);


    for (size_t i=0; i<COUNT; i++)
    {
        glBindBuffer ( GL_ARRAY_BUFFER, BUFFER[i] );
        glBufferData ( GL_ARRAY_BUFFER, BF_SZs[i], 0,  USAGES[i]); 
    }

    // Setting up VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray (VAO);
    glBindBuffer( GL_ARRAY_BUFFER, BUFFER[POSITIONS_VELOCITIES]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4, nullptr);
}
void GLFluid::draw()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, numPoints);
    glBindVertexArray(0);
}
