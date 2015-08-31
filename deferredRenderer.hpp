/*
 * Deffered renderer for meshes
 */
#pragma once
#include "renderer.hpp"

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
            TEXTURE,
            DEPTH,
            COUNT
        };
    };
    struct SHADERS
    {
        enum ENUM
        {
            GEOMETRY=0,
            QUAD,
            COUNT
        };
    };
    GLuint textures[TEXTURES::COUNT];
    GLuint FBO;
    Shaders *shdrs[SHADERS::COUNT];
public:
    /**
     * Constructor, need OpenGL context created
     * created
     */
    DeferredRenderer(const int &, const int &);

    /** Set geometry shader*/
    void setGeometryShader(Shader *);
};
