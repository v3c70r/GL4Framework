#pragma once
#include "shader.hpp"
#include <map>
#include <string>
#include <iostream>
#include <core/log.hpp>

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
    /*Create shaders and return the point to the shader just created*/
    /* Compute shader*/
    Shader* addShader(const std::string &computeShaderFile, const std::string &name);
    /* Classic VS and FS*/
    Shader* addShader(const std::string &vsFile, const std::string &fsFile, const std::string &name);
    /* Not that classice VS, GS and FS*/
    Shader* addShader(const std::string &vsFile, const std::string &gsFile, const std::string &fsFile, const std::string &name);
    ~ShaderManager()
    {
        for (const auto& shdr: shaderMap)
            delete shdr.second;
    }

    Shader* getShader(const std::string &name) const
    {
        if (shaderMap.find(name) == shaderMap.end())
            return nullptr;
        return shaderMap.at(name);
    }
    void setConfig(const std::string &name, const ShaderConfig &config)
    {
        if ( shaderMap.find(name) == shaderMap.end())
        {
            LOG::writeLogErr("No shader found, not setting shader config\n");
            return;
        }
        shaderMap.at(name)->setConfig(config);
    }
    const std::map<std::string, Shader*>& getShaderMap() const { return shaderMap;}
};
