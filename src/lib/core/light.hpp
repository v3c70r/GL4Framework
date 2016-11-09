#pragma once
//  classes for directional light, point light and spot ligth
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <objects/object.h>
#include <vector>

/*
 * ==============Lights in Shaders==========
 *
*/
const int LIGHT_UNIFORM_BUFFER=0;
class DirectionalLight 
{
private:
    glm::vec4 dir;
public:
    DirectionalLight(glm::vec4 d):dir(d){}
};


class LightManager
{
    private:
        int numLights;
        GLuint UBO;
        const int MAX_DLIGHT;
    public:
        void init();
        LightManager();
        bool addLight(const glm::vec4 &dir);
        bool removeLight(int idx);
        void bindToShader(Shader *shdr) const;
};





