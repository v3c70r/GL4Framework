#include "rendererManager.h"
void RendererManager::addRenderer( Renderer* r, const std::string &name)
{
    //dupliation check
    for ( auto const& it: rendererMap_)
        if (it.first == name || it.second == r)
            throw std::runtime_error("Duplicated renderer");
    rendererMap_[name] = r;
}

void RendererManager::assignObj2Renderer(Object* obj, const std::string &rendererName)
{
    if (rendererMap_.find(rendererName) == rendererMap_.end())
        throw std::runtime_error("No such renderer");
    else 
        rendererMap_[rendererName]->addObject(obj);
}

std::string RendererManager::queryRendererObjInfo() const
{
    //return a python dictionary of renderer and pointers
    std::stringstream ss;
    ss<<"{";
    //for (std::map<std::string, Renderer*>::const_iterator it=rendererMap_.begin(); it!=rendererMap_.end(); it++)
    for (auto const &it : rendererMap_)
    {
        ss<<"'"<<it.first<<"': "<<it.second->queryObjs()<<", ";
    }
    ss<<"}";
    return ss.str();
}
