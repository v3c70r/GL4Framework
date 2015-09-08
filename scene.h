#pragma once
#include <vector>
#include <string>
#include <sstream>
#include "object.h"
#include "meshNode.hpp"
#include "shader.hpp"
#include "light.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "arcball.h"
#include "shaderManager.h"
#include <glm/gtx/transform.hpp>
#include "forwardRenderer.hpp"
#include "rendererManager.h"
#include "deferredRenderer.hpp"

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
    Scene():camera(nullptr){
    }
    void init(const GLint &, const GLint &);
    void drawScene() const ;
    void addObject(Object *) ;

    void addDirLight(const glm::vec4 &dir);

    Object* getObject(std::string name);
    Object* getObject(int index);
    //set camera
    void setCamera(unsigned int type=CAMERA_ARCBALL, glm::vec3 transVec = glm::vec3(1.0), glm::mat4 rotMat = glm::mat4(1.0) );
    Camera* getCamera() const {return camera;}
    ~Scene() {
        for (auto i=0; i<objectPointers.size(); i++)
            delete objectPointers[i];
        delete camera;
    }

    void updateProjMat(int W, int H);

    /*Deprecating*/
    std::string getTreeView() const;
    void addMeshes(std::string fileName, Object* parent=nullptr);
};


//lil helper functions, recursive inline, hmmm...., hope the compiler is smart enough
//I don't know why I'm doing this
inline void parseDownNodes(const aiNode* pNode, const int &level )
{
    std::cout<<std::string(level, '-')<<pNode->mName.C_Str()<<std::endl;
    if (pNode->mNumChildren == 0)
        return;
    for (auto i=0; i<pNode->mNumChildren; i++)
        parseDownNodes(pNode->mChildren[i], level+1);
}

