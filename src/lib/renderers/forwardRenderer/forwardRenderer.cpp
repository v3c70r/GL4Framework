#include "forwardRenderer.hpp"
#include <iostream>

void ForwardRenderer::addObject(Object *obj) 
{
    
    obj->bindShader(shaders_.getShader(SHDR_NAME_));
    objects_.push_back(obj);
}
void ForwardRenderer::render() const
{
    for (auto &obj: objects_)
    {
        Shader *shdr = shaders_.getShader(SHDR_NAME_);
        glUseProgram(shdr->getProgramme());
        obj->update();
        obj->draw();
        glUseProgram(0);
    }
};

void ForwardRenderer::setShader(Shader *s)
{
    if (s)
    {
        shaders_.addShader(s, SHDR_NAME_);
        s->setConfig(cfg_);
        glUniform1i(s->getConfig().uniforms.at("tex"), 0);
    }
    else
        LOG::writeLogErr("Nullptr shader will not be set\n");
}
void ForwardRenderer::setShader(std::string vs, std::string fs)
{
    Shader* shdr = shaders_.addShader(vs, fs, SHDR_NAME_);
    if (shdr != nullptr)
    {
        shdr->setConfig(cfg_);
        glUniform1i(shdr->getConfig().uniforms.at("tex"), 0);
    }
    else
        LOG::writeLogErr("Falied to add shader from file\n");
}

