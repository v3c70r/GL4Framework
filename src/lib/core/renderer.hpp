/*
 * Base class for renderer
 */
#pragma once
#include "object.h"
#include <vector>
#include <string>
#include <sstream>
#include <core/shaderManager.h>
#include "light.hpp"
#include "camera.h"

class Renderer
{
protected:
    /* No need to clean it up, scene class will do all the garbage collection work*/
    std::vector<Object*> objects;
    ShaderManager shaders;
public:
    Renderer(){}
    virtual void addObject(Object *obj) {objects.push_back(obj);}

    /**
     * @brief Remove an object from this renderer
     *
     * @param name Name of the object
     */
    void removeObject(const std::string &name){ 
        for (std::vector<Object*>::iterator it = objects.begin(); it!=objects.end();)
        {
            if ((*it)->getName() == name )
                it = objects.erase(it);
            else 
                it++;
        }
    }
    /**
     * @brief Return a python list of objects in a renderer 
     *
     * @return Objects bond to this renderer in string
     */
    std::string queryObjs() const
    {
        
        if (objects.size() == 0)
            return std::string("[ ]");

        std::stringstream ss;
        ss<<"[";
        for (const auto& object: objects)
            ss<<"'"<<object->getName()<<"', ";
        ss<<"]";
        return ss.str();
    }

    // Interfaces
    virtual ~Renderer(){};
    virtual void render() const=0;
    virtual void setupShaders()=0;

    /**
     * @brief Setup light to this renderer
     *
     * @param lights const reference to a light manager
     */
    virtual void setupLight(const LightManager &lights)
    { 
        for (const auto& shader: shaders.getShaderMap())
            lights.bindToShader(shader.second);
    }

    /**
     * @brief Setup camera for this renderer
     *
     * @param camera const pointer to camera
     */
    virtual void setupCamera(const Camera* camera)
    {
        for (const auto& shader: shaders.getShaderMap())
            camera->bindToShader(shader.second);
    }
};
