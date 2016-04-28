#include "forwardRenderer.hpp"
#include <iostream>

void ForwardRenderer::addObject(Object *obj) 
{
    
    glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BLOCKS::binding[UNIFORM_BLOCKS::MATERIAL], buffer);
    objects.push_back(obj);
}
void ForwardRenderer::render() const
{
    for (auto &obj: objects)
    {
        Shader *shdr = shaders.getShader(SHDR_NAME);
        obj->update();
        glUseProgram(shdr->getProgramme());
        obj->draw();
        glUseProgram(0);
    }
};

void ForwardRenderer::setShader(Shader *s)
{
    if (s)
        shaders.addShader(s, SHDR_NAME);
    else
        throw std::runtime_error("nullptr shader dettected");
}
void ForwardRenderer::setShader(std::string vs, std::string fs)
{
    if (shaders.addShader(vs, fs, SHDR_NAME)==nullptr)
        throw std::runtime_error("nullptr shader dettected");
}

