#include <core/renderer.hpp>

#pragma once
class PointRendererTem :public Renderer
{
    private:
        GLuint tempTexture;
        Shader* shader; 
    public:
        PointRendererTem(const int &, const int &);
        ~PointRendererTem(){};
        void LoadTexture(const char* filename);
        void setShader(Shader* s);
        void render() const override;
};
