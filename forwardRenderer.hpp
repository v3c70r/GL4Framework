/*
 * Forward renderer
 */
#include "renderer.hpp"
#include "shader.hpp"
#include <stdexcept>

class ForwardRenderer: public Renderer
{
    Shader *shdr;
public:
    ForwardRenderer():
        shdr(nullptr)
    {}

    void render() const override;
    void setShader(Shader *);
    ~ForwardRenderer(){}
};
