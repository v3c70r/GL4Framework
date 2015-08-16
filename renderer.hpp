/*
 * Deffered renderer
 */
#pragma once
#include <GL/glew.h>
#include <iostream>
#include <stdexcept>
#include "shader.hpp"

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
    Shader *dirPass;        //Shader for direct rendering pass
    Shader *DRPass;         //Shader for Deffered Rendering pass
    GLuint depthTexture;
public:
    Renderer():
        FBO(0),
        quadVAO(0),
        depthTexture(0)
    {}
    void init(int texWidth, int texHeight);
    void setDirPassShader(Shader *s) {dirPass = s;}
    void setDRPassShader(Shader *s) {DRPass = s;}
    GLuint getFBO() const {return FBO;}
    void render(const GLuint &width, const GLuint &height) const;
};

