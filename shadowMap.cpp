#include "shadowMap.h"


bool ShadowMap::init(int texWidth, int texHeight, std::string vsFile, std::string fsFile)
{
    std::cout<<"Befor:"<<glGetError()<<std::endl;

    // Initialize two textures
    glGenTextures(MAX_NUM_LIGHTS, shadowTex);

    for (GLuint i=0; i<MAX_NUM_LIGHTS; i++)
    {
        glBindTexture(GL_TEXTURE_2D, shadowTex[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, texWidth, texHeight, 0,GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);




    glGenFramebuffers(MAX_NUM_LIGHTS, fbo);
    for (GLuint i=0; i<MAX_NUM_LIGHTS; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo[i]);
        glDrawBuffer(GL_NONE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex[i], 0);


        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout<<glCheckFramebufferStatus(GL_FRAMEBUFFER)<<std::endl;
            return false;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    GLuint vao;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
    // Create and compile our GLSL program from the shaders
    assert(shadowShader.createProgrammeFromFiles(vsFile.c_str(),  fsFile.c_str()));
    glDeleteVertexArrays(1, &vao);
    glUseProgram(shadowShader.getProgramme());
    MVPlocation= glGetUniformLocation(shadowShader.getProgramme(), "depthMVP");
    glUseProgram(0);
    std::cout<<"After:"<<glGetError()<<std::endl;
    return true;
}


void ShadowMap::setMVP( const glm::mat4 &mvp)
{
    glUniformMatrix4fv(MVPlocation, 1,GL_FALSE, &mvp[0][0]);
}



