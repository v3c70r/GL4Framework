#pragma once
#include "shader.hpp"
#include <map>
#include <string>

/*
 * Shader manager
 * A shader is manged by shader names
 */
class ShaderManager
{
    std::map<std::string, Shader*> shaderMap;
public:
    //Add a shader to shader manager
    //you need to new the shader outside
    void addShader(Shader* shdr, const std::string &name);
    /* Compute shader*/
    void addShader(const std::string &computeShaderFile, const std::string &name);
    /* Classic VS and FS*/
    void addShader(const std::string &vsFile, const std::string &fsFile, const std::string &name);
    /* Not that classice VS, GS and FS*/
    void addShader(const std::string &vsFile, const std::string &gsFile, const std::string &fsFile, const std::string &name);
    ~ShaderManager()
    {
        for (std::map<std::string, Shader*>::iterator it=shaderMap.begin(); it!=shaderMap.end(); ++it)
            delete it->second;
    }
    const Shader* getShader(std::string name) const
    {
        if (shaderMap.find(name) == shaderMap.end())
            return nullptr;
        else
            return shaderMap.at(name);
    }

    /*
     * Set projection matrix for all of the shaders
     * TODO: move projection matrix to unifrom block
     */
    void setProjMats()  
    {
    }
};
