
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.hpp"
#include <iostream>
#include "camera.h"
/*
 * A screen for object to render on
 */
class Screen{
private:
    GLuint fbo; // a framebuffer
    GLuint rbo; //render buffer object


    //Textures
    GLuint colorTexture; //texture render to, a texture
    GLuint depthTexture;

    GLuint vao; //vao for quad
    Shader screenShader;
    GLuint quad_programID;
    GLuint texID;
    GLuint depthID;
    GLuint timeID;
    GLuint quad_vertexbuffer;

    //parameters for compute buffer
    Shader computeShader;
    GLint csIDs[8];





public:
    bool init(int texWidth, int texHeight, std::string vsFile, std::string fsFile);
    void useScreen(const int &x,const int &y);       //x, y should be the size of frame buffer incase of retina screen
    void renderScreen(const int &x,const int &y,Camera *c, GLuint fbo=0);
    void deactivate();
    GLuint getFBO() const
    {
        return fbo;
    }
};
