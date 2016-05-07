/*
 * Forward renderer
 * This renderer contains simple one pass shader
 */
#include <core/renderer.hpp>

class ForwardRenderer: public Renderer
{
    const std::string SHDR_NAME_;
    ShaderConfig cfg_;
public:
    ForwardRenderer():SHDR_NAME_{"SHDR_FORWARD"}
    {
        // TODO: Add uniforms
        cfg_.addUniform("tex");
        cfg_.addUniformBlock("Material");
    }
    ForwardRenderer(Shader *s):ForwardRenderer()
    {
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
    ~ForwardRenderer() override{}
};
