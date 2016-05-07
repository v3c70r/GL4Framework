/*
 * Renderer manager which manage renders
 */
#pragma once
#include <core/renderer.hpp>
#include <map>
#include <string>
class RendererManager
{
    std::map<std::string, Renderer*> rendererMap;
public:
    void addRenderer(Renderer* r, const std::string &name);
    ~RendererManager()
    {
        for (auto &renderer: rendererMap)
            delete renderer.second;
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
        for (const auto &renderer: rendererMap)
            renderer.second->render();
    }

    /** assign an object to renderer*/
    void assignObj2Renderer(Object* obj, const std::string &rendererName);
    std::string queryRendererObjInfo() const;
};
