#pragma once
#include "shader.hpp"
#include <GL/glew.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>

const GLuint MAX_NUM_LIGHTS=20;

class ShadowMap
{
private:
    GLuint fbo[MAX_NUM_LIGHTS];
    GLuint shadowTex[MAX_NUM_LIGHTS];
    Shader shadowShader;
    GLuint numOfLights;
    GLuint MVPlocation;
public:
    ShadowMap(): numOfLights(0){}
    ~ShadowMap()
    {
        glDeleteTextures(MAX_NUM_LIGHTS, shadowTex);
        glDeleteFramebuffers(MAX_NUM_LIGHTS, fbo);
    }
    bool init(int texWidth, int texHeight, std::string vsFile, std::string fsFile);
    GLuint getFBO(GLuint i) const{return fbo[i];}
    GLuint getTextures(GLuint i) const{return shadowTex[i];}
    GLuint getProgramme() const{ return shadowShader.getProgramme();}
    void setMVP( const glm::mat4 &mvp);
    void setNumOfLights(GLuint n){numOfLights = n;}
    GLuint getNumOfLights() const{return numOfLights;}
};

