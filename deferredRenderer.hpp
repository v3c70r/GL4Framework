/*
 * Deffered renderer for meshes
 */
#pragma once
#include "renderer.hpp"
#include "shader.hpp"

class DeferredRenderer: public Renderer
{
private:
    struct TEXTURES
    {
        enum ENUM
        {
            POSITION=0,
            DIFFUSE,
            NORMAL,
            TEXCOORD,
            DEPTH,
            COUNT
        };
    };
    struct SHADERS
    {
        enum ENUM
        {
            GEOMETRY=0,
            COUNT
        };
    };
    GLuint textures[TEXTURES::COUNT];
    GLuint FBO;
    Shader *shdrs[SHADERS::COUNT];
    void renderGeometryPass() const;
    void renderLightPass() const;
    GLint width, height;
public:
    /**
     * Constructor, need OpenGL context created
     * created
     */
    DeferredRenderer()  =   delete;
    DeferredRenderer(const int &, const int &);

    /** Set geometry shader*/
    void setGeometryShader(Shader *);
    void render() const override;
};
