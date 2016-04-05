#include <objects/GLSPH/fluid.h>
#include <GLFW/glfw3.h>
#include <stdexcept>


GLFluid::GLFluid() : MAX_NUM_POINTS(6556)
{
    if (glfwGetCurrentContext() == nullptr)
        throw std::runtime_error("Context is not created");

    // Allocate resources 
    glGenBuffers (COUNT, BUFFER);

    // INDICES and CELLIDS
    glBindBuffer (GL_ARRAY_BUFFER, BUFFER[INDICES]);
    glBufferData (GL_ARRAY_BUFFER, MAX_NUM_POINTS*sizeof(uint), 0, GL_DYNAMIC_DRAW);
    glBindBuffer (GL_ARRAY_BUFFER, BUFFER[CELLIDS]);
    glBufferData (GL_ARRAY_BUFFER, MAX_NUM_POINTS*sizeof(uint), 0, GL_DYNAMIC_DRAW);

    // POSITIONS
    glBindBuffer (GL_ARRAY_BUFFER, BUFFER[POSITIONS]);
    glBufferData (GL_ARRAY_BUFFER, MAX_NUM_POINTS*sizeof(uint), 0, GL_DYNAMIC_DRAW);

    
}
