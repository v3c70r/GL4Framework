#include "rendererManager.h"
void RendererManager::addRenderer( Renderer* r, const std::string &name)
{
    //dupliation check
    for (std::map<std::string, Renderer*>::iterator it=rendererMap.begin();
            it!=rendererMap.end();it++)
        if (it->first == name || it->second == r)
            throw std::runtime_error("Duplicated renderer");
    rendererMap[name] = r;
}

void RendererManager::assignObj2Renderer(Object* obj, const std::string &rendererName)
{
    if (rendererMap.find(rendererName) == rendererMap.end())
        throw std::runtime_error("No such renderer");
    else 
        rendererMap[rendererName]->addObject(obj);
}

std::string RendererManager::queryRenderObjInfo() const
{
    //return a python dictionary of renderer and pointers
    std::stringstream ss;
    ss<<"{";
    for (std::map<std::string, Renderer*>::const_iterator it=rendererMap.begin(); it!=rendererMap.end(); it++)
    {
        ss<<"'"<<it->first<<"': "<<it->second->queryObjs()<<", ";
    }
    ss<<"}";
    return ss.str();
}
