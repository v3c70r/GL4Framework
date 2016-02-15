/*
 * Forward renderer
 */
#include <core/renderer.hpp>
#include <core/shader.hpp>
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
