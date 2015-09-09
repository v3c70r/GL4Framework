/*
 * Base class for renderer
 */
#pragma once
#include "object.h"
#include <vector>
#include <string>
#include <sstream>

class Renderer
{
protected:
    /* No need to clean it up, scene class will do all the garbage collection work*/
    std::vector<Object*> objects;
public:
    Renderer(){}
    void addObject(Object *obj) {objects.push_back(obj);}
    void removeObject(const std::string &name){ 
        for (std::vector<Object*>::iterator it = objects.begin(); it!=objects.end();)
        {
            if ((*it)->getName() == name )
                it = objects.erase(it);
            else 
                it++;
        }
    }
    std::string queryObjs() const
    {
        std::stringstream ss;
        ss<<"{";
        for (auto i=0; i<objects.size(); i++)
            ss<<objects[i]->getName()<<" ";
        ss<<"}";
        return ss.str();
    }
    virtual ~Renderer(){};
    virtual void render() const=0;
};
