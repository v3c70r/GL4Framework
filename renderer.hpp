/*
 * Deffered renderer
 */
#pragma once
#include <GL/glew.h>
#include <iostream>

class Renderer
{
    struct TEXTURES {
        enum ENUM
        {
            POSITION=0,
            DIFFUSE,
            NORMAL,
            TEXCOORD,
            COUNT
        };
    };
private:
    GLuint FBO;
    GLuint quadVAO;
    GLuint textures[TEXTURES::COUNT];
    //Depth texture is seperated from other textures because it follows another texture process
    GLuint depthTexture;
public:
    void init(int texWidth, int texHeight);
    GLuint getFBO() const {return FBO;}
};

