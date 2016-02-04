#include "forwardRenderer.hpp"
#include <iostream>

void ForwardRenderer::render() const
{
    for (auto i=0; i<objects.size(); i++)
    {
        Object *obj = objects[i];
        obj->update();
        obj->bindShader(shdr);
        glUseProgram(shdr->getProgramme());
        obj->draw();
        glUseProgram(0);
    }
};

void ForwardRenderer::setShader(Shader *s)
{
    if (s)
        shdr=s;
    else
        throw std::runtime_error("nullptr shader setted");

}
