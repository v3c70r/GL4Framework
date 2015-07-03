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
#include <glm/gtx/transform.hpp>
//#include "points.cuh"

/*
 * A scene is a place where all objects are put in
 * [X] Meshes
 * [X] Shaders
 * [X] Camera
 * [X] Lights
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
private:
    std::vector<Object*> objectPointers;
    std::vector<Shader*> shaderPointers;
    LightManager *lights;
    Camera *camera;
public:
    Scene():camera(nullptr), lights(nullptr){
    }
    void init();
    void drawScene() const ;
    //Adding object to scene
    void addShader(std::string vsFile, std::string gsFile, std::string fsFile);
    void addShader(std::string vsFile,  std::string fsFile);
    void addMeshes(std::string fileName, Object* parent=nullptr);
    void import(std::string fileName, Object *parent=nullptr);
    //void addFluidSys(std::string name, Object* parent=nullptr);
    void addDirLight(const glm::vec4 &dir);

    Object* getObject(std::string name);
    Object* getObject(int index);
    //set camera
    void setCamera(unsigned int type=CAMERA_ARCBALL, glm::vec3 transVec = glm::vec3(1.0), glm::mat4 rotMat = glm::mat4(1.0) );
    Camera* getCamera() const {return camera;}
    ~Scene() {
        for (auto i=0; i<objectPointers.size(); i++)
            delete objectPointers[i];
        for (auto i=0; i<shaderPointers.size(); i++)
            delete shaderPointers[i];
        delete camera;
        delete lights;
    }

    void setObjShdr(uint objIdx, uint shdrIdx)
    {
        objectPointers[objIdx]->setShader(shaderPointers[shdrIdx]);
    }
    void updateProjMat(int W, int H);
    std::string getTreeView() const;
};


