/*
 * Renderer manager which manage renders
 */
#pragma once
#include "renderer.hpp"
#include <map>
#include <string>
class RendererManager
{
    std::map<std::string, Renderer*> rendererMap;
public:
    void addRenderer(Renderer* r, const std::string &name);
    ~RendererManager()
    {
        for (std::map<std::string, Renderer*>::iterator it=rendererMap.begin();
                it!=rendererMap.end(); it++)
            delete it->second;
    }
    Renderer* getRenderer(const std::string &name) const
    {
        if (rendererMap.find(name) == rendererMap.end())
            return nullptr;
        else
            return rendererMap.at(name);
    }
    void renderAll() const
    {
        for (std::map<std::string, Renderer*>::const_iterator it=rendererMap.begin();
                it!=rendererMap.end(); it++)
            it->second->render();
    }

    /** assign an object to renderer*/
    void assignObj2Renderer(Object* obj, const std::string &rendererName);
    std::string queryRendererObjInfo() const;
};
