#pragma once
#include <vector>
#include <string>
#include <sstream>
#include "object.h"

#include <objects/meshNode/meshNode.hpp>
#include "shader.hpp"
#include "light.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <camera/arcball/arcball.h>
#include "shaderManager.h"
#include <glm/glm.hpp>
#include <renderers/forwardRenderer/forwardRenderer.hpp>
#include "rendererManager.h"
#include <renderers/deferredRenderer/deferredRenderer.hpp>
#include <renderers/rayTracer/rayTracer.h>
//#include <objects/point/points.cuh>
#include <renderers/pointRenderer/pointRenderTem.hpp>

/*
 * A scene is a place where all objects are put in
 */

namespace BUILD_IN_SHADERS
{
    enum INDICES{
        MESH = 0,
        PARTICLES,
        COUNT
    };
}

class Scene
{
    friend class Importer;
private:
    std::vector<Object*> objectPointers;
    RendererManager renderers;

    ShaderManager shaders;
    LightManager lights;
    Camera *camera; 

public:
    static Scene& getInstance()
    {
        static Scene instance;
        return instance;
    }
    Scene():camera(nullptr){
    }
    void init(const GLint &, const GLint &);
    void drawScene() const ;
    void addObject(Object *pObj){objectPointers.push_back(pObj);}
    //set camera
    void setCamera(unsigned int type=CAMERA_ARCBALL, glm::vec3 transVec = glm::vec3(1.0), glm::mat4 rotMat = glm::mat4(1.0) );
    ~Scene() {
        for (const auto& elem: objectPointers)
            delete elem;
        delete camera;
    }
    void updateProjMat(int W, int H);
    //=========Getters==========
    
    /**
     * @brief Get renderer manager
     *
     * @return reference to the renderer manager
     */
    RendererManager& getRendererManager() { return renderers; }

    /**
     * @brief Get reference to shader manager
     *
     * @return reference to shader manager
     */
    ShaderManager& getShaderManager() { return shaders;}
    Camera* getCamera() {return camera;}
    LightManager& getLightManager() { return lights;}
    std::string getTreeView() const;
    Object* getObject(std::string name) const;
    Object* getObject(int index) const;

    /*Deprecating*/
    void addMeshes(std::string fileName, Object* parent=nullptr);
};


//lil helper functions, recursive inline, hmmm...., hope the compiler is smart enough
//I don't know why I'm doing this
inline void parseDownNodes(const aiNode* pNode, const int &level )
{
    std::cout<<std::string(level, '-')<<pNode->mName.C_Str()<<std::endl;
    if (pNode->mNumChildren == 0)
        return;
    for (size_t i=0; i<pNode->mNumChildren; i++)
        parseDownNodes(pNode->mChildren[i], level+1);
}

