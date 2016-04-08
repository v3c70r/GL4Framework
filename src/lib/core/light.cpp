#include "light.hpp"

void LightManager::init()
{
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, (MAX_DLIGHT+1)*sizeof(GLfloat)*4, 0, GL_DYNAMIC_DRAW);
    //set number of lights to zero
    glBufferSubData(GL_UNIFORM_BUFFER, MAX_DLIGHT*sizeof(GLfloat)*4, sizeof(GLint), &numLights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

LightManager::LightManager(): numLights(0), MAX_DLIGHT(5)
{
}

bool LightManager::addLight(const glm::vec4 &dir)
{
    if (numLights >= MAX_DLIGHT)
        return false;
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, numLights*sizeof(GLfloat)*4, sizeof(GLfloat)*4, &dir[0]);
    numLights++;
    glBufferSubData(GL_UNIFORM_BUFFER, MAX_DLIGHT*sizeof(GLfloat)*4, sizeof(GLint), &numLights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return true;
}

//TODO: Finish this function
bool LightManager::removeLight(int idx)
{
    if (numLights <= 0 || idx >= numLights)
        return false;
    return true;
}

void LightManager::bindToShader(Shader *shdr) const
{
    shdr->bindDirLights(UBO);
}
