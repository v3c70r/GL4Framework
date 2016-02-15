#pragma once
#include <core/renderer.hpp>
#include <core/shader.hpp>
class RayTracer : public Renderer
{
private:
    Shader *compShdr;       //ComputShader for raytracing
    Shader quadShdr;        //Quad shader
    GLuint texture;         //Texture for raytracing output
    GLuint timeBuffer;

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
        glDeleteBuffers(1, &timeBuffer);
        glDeleteVertexArrays(1, &quadVAO);
    }
    void setCompShader( Shader *s) { compShdr = s;}
    void render() const override;
};


