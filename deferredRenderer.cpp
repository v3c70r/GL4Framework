#include "deferredRenderer.hpp"
#include <stdexcept>
#include <GL/glew.h>
DeferredRenderer::DeferredRenderer(const int &width, const int &height)
{
    this->width = width;
    this->height = height;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

    glGenTextures(TEXTURES::COUNT, textures);

    //Init color textures
    for (auto i=0; i<TEXTURES::DEPTH; i++)
    {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, textures[i], 0);
    }
    //Depth
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURES::DEPTH]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                  nullptr);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textures[TEXTURES::DEPTH], 0);

    GLenum DrawBuffers[] = { 
        GL_COLOR_ATTACHMENT0, 
        GL_COLOR_ATTACHMENT1, 
        GL_COLOR_ATTACHMENT2, 
        GL_COLOR_ATTACHMENT3 
    }; 
    glDrawBuffers(TEXTURES::DEPTH, DrawBuffers);    //TEXTURES::DEPTH = number of color texrtures

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {

        throw std::runtime_error("FB error");
    }
    // restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void DeferredRenderer::setGeometryShader(Shader *s)
{
    shdrs[SHADERS::GEOMETRY] = s;
}

void DeferredRenderer::renderGeometryPass() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto i=0; i<objects.size(); i++)
    {
        Object *obj = objects[i];
        obj->update();
        obj->bindShader(shdrs[SHADERS::GEOMETRY]);
        glUseProgram(shdrs[SHADERS::GEOMETRY]->getProgramme());
        obj->draw();
        glUseProgram(0);
    }
}

void DeferredRenderer::renderLightPass() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);

    GLsizei halfWidth = width/2;
    GLsizei halfHeight = height/2;

    //Position pass
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, width, height,
            0, 0, halfWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    //Diffuse pass
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(0, 0, width, height,
            0, halfHeight, halfWidth, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    //Normal Pass
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glBlitFramebuffer(0, 0, width, height,
            halfWidth, halfHeight, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    //TEX coord pass
    glReadBuffer(GL_COLOR_ATTACHMENT3);
    glBlitFramebuffer(0, 0, width, height,
            halfWidth, 0, width, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR); 

}

void DeferredRenderer::render() const
{
    if (objects.size() == 0) return;
    renderGeometryPass();
    renderLightPass();
}

