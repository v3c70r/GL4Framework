/*
 * Renderer manager which manage renders
 */
#pragma once
#include <core/renderer.hpp>
#include <map>
#include <string>
class RendererManager
{
    std::map<std::string, Renderer*> rendererMap_;
public:
    std::map<std::string, Renderer*> &
        getRendereMap() {return rendererMap_;}
    void addRenderer(Renderer* r, const std::string &name);
    ~RendererManager()
    {
        for (auto &renderer: rendererMap_)
            delete renderer.second;
    }
    Renderer* getRenderer(const std::string &name) const
    {
        if (rendererMap_.find(name) == rendererMap_.end())
            return nullptr;
        else
            return rendererMap_.at(name);
    }
    void renderAll() const
    {
        for (const auto &renderer: rendererMap_)
            renderer.second->render();
    }

    /** assign an object to renderer*/
    void assignObj2Renderer(Object* obj, const std::string &rendererName);
    std::string queryRendererObjInfo() const;
};
