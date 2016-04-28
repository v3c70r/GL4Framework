/*
 * Forward renderer
 * This renderer contains simple one pass shader
 */
#include <core/renderer.hpp>
#include <stdexcept>

class ForwardRenderer: public Renderer
{
    const std::string SHDR_NAME;
    ShaderConfig cfg;
public:
    ForwardRenderer():SHDR_NAME{"SHDR_FORWARD"}
    {
        // TODO: Add uniforms
        cfg.addUniform("Tex");
        cfg.addUniformBlock("Material");
    }
    ForwardRenderer(Shader *s):ForwardRenderer()
    {
        s->setConfig(cfg);
        setShader(s); 
    }
    ForwardRenderer(std::string v, std::string f):ForwardRenderer()
    {
        setShader(v, f);
    }

    void addObject(Object *obj) override;
    void render() const override;
    void setShader(Shader *);
    void setShader(std::string vs, std::string fs);
    ~ForwardRenderer(){}
};
