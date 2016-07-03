#include "deferredRenderer.hpp"
#include <stdexcept>
#include <GL/glew.h>
#include <iostream>
DeferredRenderer::DeferredRenderer(const GLuint &width, const GLuint &height):GEO_SHDR_NAME_("SHDR_GEO")
{
    width_ = width;
    height_ = height;
    cfg_.addUniform("tex");
    cfg_.addUniformBlock("Material");


    // Gen fbo
    glGenFramebuffers(1, &FBO_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO_);

    // Gen textures
    glGenTextures(TEXTURES::COUNT, textures_);

    // Init color textures
    for (int i=0; i<TEXTURES::DEPTH; i++)
    {
        glBindTexture(GL_TEXTURE_2D, textures_[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, textures_[i], 0);
    }
    //Depth
    glBindTexture(GL_TEXTURE_2D, textures_[TEXTURES::DEPTH]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textures_[TEXTURES::DEPTH], 0);

    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    GLenum drawBuffers[] = { 
        GL_COLOR_ATTACHMENT0, 
        GL_COLOR_ATTACHMENT1, 
        GL_COLOR_ATTACHMENT2, 
        GL_COLOR_ATTACHMENT3 
    }; 

    glDrawBuffers(TEXTURES::DEPTH, drawBuffers); //TEXTURES::DEPTH = number of color texrtures

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        LOG::writeLogErr("Faild to init framebuffer\n");
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
DeferredRenderer::DeferredRenderer(std::string geoVS, std::string geoFS, const int& w, const int& h): DeferredRenderer(w, h)
{
    Shader* shdr = shaders_.addShader(geoVS, geoFS, GEO_SHDR_NAME_);
    shdr->setConfig(cfg_);
}

void DeferredRenderer::setGeometryShader(Shader *s)
{
    s->setConfig(cfg_);
    shaders_.addShader(s, GEO_SHDR_NAME_);
}

void DeferredRenderer::renderGeometryPass() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto &obj: objects_)
    {
        obj->update();
        Shader* shdr = shaders_.getShader(GEO_SHDR_NAME_);
        glUseProgram(shdr->getProgramme());
        obj->draw();
        glUseProgram(0);
    }
}


void DeferredRenderer::renderLightPass() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO_);

    //GLsizei halfWidth = width_/2;
    //GLsizei halfHeight = height_/2;

//    //Position pass
//    glReadBuffer(GL_COLOR_ATTACHMENT0);
//    glBlitFramebuffer(0, 0, width, height,
//            0, 0, halfWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
//
//    //Diffuse pass
//    glReadBuffer(GL_COLOR_ATTACHMENT1);
//    glBlitFramebuffer(0, 0, width, height,
//            0, halfHeight, halfWidth, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
//
//    //Normal Pass
//    glReadBuffer(GL_COLOR_ATTACHMENT2);
//    glBlitFramebuffer(0, 0, width, height,
//            halfWidth, halfHeight, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
//
    //Final pass
    //glReadBuffer(GL_DEPTH_COMPONENT);
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glBlitFramebuffer(0, 0, width_, height_,
            0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_LINEAR); 

}

void DeferredRenderer::render() const
{
    if (objects_.size() == 0) return;
    renderGeometryPass();
    renderLightPass();
}
void DeferredRenderer::addObject(Object *obj)
{
    objects_.push_back(obj);
    obj->bindShader(shaders_.getShader(GEO_SHDR_NAME_));
}
DeferredRenderer::~DeferredRenderer()
{
    glDeleteTextures(TEXTURES::COUNT, textures_);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &FBO_);
}
void DeferredRenderer::resize(GLuint w, GLuint h)
{
    width_ = w; height_ = h;
    for (int i=0; i<TEXTURES::DEPTH; i++)
    {
        glBindTexture(GL_TEXTURE_2D, textures_[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, textures_[TEXTURES::DEPTH]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
}

