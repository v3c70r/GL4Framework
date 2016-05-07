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
    std::vector<Object*> objects_;
    ShaderManager shaders_;
public:
    Renderer(){}
    virtual void addObject(Object *obj) {objects_.push_back(obj);}

    /**
     * @brief Remove an object from this renderer
     *
     * @param name Name of the object
     */
    void removeObject(const std::string &name){ 
        for (std::vector<Object*>::iterator it = objects_.begin(); it!=objects_.end();)
        {
            if ((*it)->getName() == name )
                it = objects_.erase(it);
            else 
                it++;
        }
    }
    /**
     * @brief Return a python list of objects_ in a renderer 
     *
     * @return Objects bond to this renderer in string
     */
    std::string queryObjs() const
    {
        
        if (objects_.size() == 0)
            return std::string("[ ]");

        std::stringstream ss;
        ss<<"[";
        for (const auto& object: objects_)
            ss<<"'"<<object->getName()<<"', ";
        ss<<"]";
        return ss.str();
    }

    // Interfaces
    virtual ~Renderer(){};
    virtual void render() const=0;

    /**
     * @brief Setup light to this renderer
     *
     * @param lights const reference to a light manager
     */
    virtual void setupLight(const LightManager &lights)
    { 
        for (const auto& shader: shaders_.getShaderMap())
            lights.bindToShader(shader.second);
    }

    /**
     * @brief Setup camera for this renderer
     *
     * @param camera const pointer to camera
     */
    virtual void setupCamera(const Camera* camera)
    {
        for (const auto& shader: shaders_.getShaderMap())
            camera->bindToShader(shader.second);
    }
};
