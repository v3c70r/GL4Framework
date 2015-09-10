#pragma once
#include "renderer.hpp"
#include "shader.hpp"
class RayTracer : public Renderer
{
private:
    Shader *compShdr;       //ComputShader for raytracing
    Shader quadShdr;        //Quad shader
    GLuint texture;         //Texture for raytracing output

    GLuint quadVAO;         //A quad to draw textrue
    GLuint quadVBO;         //A quad to draw textrue
    GLint width;
    GLint height;
    GLint texID;
public:
    RayTracer(const int &width, const int &height);
    ~RayTracer()
    {
        glDeleteTextures(1, &texture);
        glDeleteBuffers(1, &quadVAO);
        glDeleteVertexArrays(1, &quadVAO);
    }
    void setCompShader( Shader *s) { compShdr = s;}
    void render() const override;
};


