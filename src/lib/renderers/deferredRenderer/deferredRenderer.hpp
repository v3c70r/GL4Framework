/*
 * Deffered renderer for meshes
 */
#pragma once
#include <core/renderer.hpp>
#include <core/shader.hpp>

class DeferredRenderer: public Renderer
{
private:
    struct TEXTURES {
        enum ENUM
        {
            POSITION=0,
            DIFFUSE,
            NORMAL,
            TEXCOORD,
            DEPTH,
            COUNT
        }; };
    GLuint textures_[TEXTURES::COUNT];
    const std::string GEO_SHDR_NAME_;
    ShaderConfig cfg_;
    GLuint FBO_;
    void renderGeometryPass() const;
    void renderLightPass() const;
    GLuint width_, height_;
public:
    /**
     * Constructor, need OpenGL context created
     * created
     */
    DeferredRenderer()  =   delete;
    DeferredRenderer(const GLuint &, const GLuint &);
    DeferredRenderer(std::string geoVS, std::string geoFS, const int& w, const int& h);
    ~DeferredRenderer() override;

    /** Set geometry shader*/
    void setGeometryShader(Shader *);
    void render() const override;
    void addObject(Object *obj) override;
    void resize(GLuint w, GLuint h);
};
