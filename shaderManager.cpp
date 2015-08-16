#include "shaderManager.h"

/**
 * Note:
 * Please always assign a name to this shader,
 * and also, this name should not exists in the
 * shader map alread.
 *
 * This function is used when you already have a
 * shader assigned in heap space and you want to
 * use this shader manager to manage it.
 *
 * Please use other versions of overloads if you 
 * want to build shader from file
 */
void ShaderManager::addShader(Shader* shdr, const std::string &name)
{
    //Check for if this shader is duplicated
    for (std::map<std::string, Shader*>::iterator it=shaderMap.begin(); it!=shaderMap.end(); ++it)
    {
        if (it->first == name || it->second->getProgramme() == shdr->getProgramme())
            throw std::runtime_error("Duplicated shader in ShaderManager");
        else
            shaderMap[name] = shdr;
    }
}
/* Compute shader*/
void ShaderManager::addShader(const std::string &computeShaderFile, const std::string &name)
{
    Shader *shdr = new Shader;
    GLuint dummyVAO;
    glGenVertexArrays(1, &dummyVAO);
    glBindVertexArray(dummyVAO);
    assert(shdr->createProgrammeFromFiles(computeShaderFile.c_str()));
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &dummyVAO);
    addShader(shdr, name);
}
/* Classic VS and FS*/
void ShaderManager::addShader(const std::string &vsFile, const std::string &fsFile, const std::string &name)
{
    Shader *shdr = new Shader;
    GLuint dummyVAO;
    glGenVertexArrays(1, &dummyVAO);
    glBindVertexArray(dummyVAO);
    assert(shdr->createProgrammeFromFiles(vsFile.c_str(), fsFile.c_str()));
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &dummyVAO);
    addShader(shdr, name);
}
/* Not that classic VS, GS and FS*/
void ShaderManager::addShader(const std::string &vsFile, const std::string &gsFile, const std::string &fsFile, const std::string &name)
{
    Shader *shdr = new Shader;
    GLuint dummyVAO;
    glGenVertexArrays(1, &dummyVAO);
    glBindVertexArray(dummyVAO);
    assert(shdr->createProgrammeFromFiles(vsFile.c_str(), gsFile.c_str(), fsFile.c_str()));
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &dummyVAO);
    addShader(shdr, name);
}
