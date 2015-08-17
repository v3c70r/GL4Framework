/*
 * Base class of renderer
 */
#pragma once
#include "shader.hpp"

class Renderer
{
protected:
    Shader *shdr;
public:
    Renderer():shdr(nullptr){}
    void setShader(Shader *s){shdr = s;};
};
