#include "rendererManager.h"
#include <core/log.hpp>
void RendererManager::addRenderer( Renderer* r, const std::string &name)
{
    //dupliation check
    for ( auto const& it: rendererMap)
        if (it.first == name || it.second == r)
        {
            LOG::writeLogErr("Duplicated renderer %s\n, not adding\n", 
                    name.c_str());
            return;
        }
    rendererMap[name] = r;
}

void RendererManager::assignObj2Renderer(Object* obj, const std::string &rendererName)
{
    if (rendererMap.find(rendererName) == rendererMap.end())
        LOG::writeLogErr("No renderer %s found, object %s is note going to be assigned\n", rendererName.c_str(), obj->getName().c_str());
    else 
        rendererMap[rendererName]->addObject(obj);
}

std::string RendererManager::queryRendererObjInfo() const
{
    //return a python dictionary of renderer and pointers
    std::stringstream ss;
    ss<<"{";
    //for (std::map<std::string, Renderer*>::const_iterator it=rendererMap.begin(); it!=rendererMap.end(); it++)
    for (auto const &it : rendererMap)
    {
        ss<<"'"<<it.first<<"': "<<it.second->queryObjs()<<", ";
    }
    ss<<"}";
    return ss.str();
}
