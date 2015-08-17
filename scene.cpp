#include "scene.h"

/*
 * Init scene, 
 * setting default settings
 * need to be called after OpenGL context intialized
 */

void Scene::init()
{
    //initialzie default shaders
    lights.init();
    lights.addLight(glm::vec4(0.0, 0.0, 1.0, 0.0));
    lights.addLight(glm::vec4(0.0, 0.0, -1.0, 0.0));
    setCamera(CAMERA_ARCBALL, glm::vec3(0.0, 0.0, -15.0), glm::mat4x4(1.0));
    camera->init();

    Shader * shdr = nullptr;
    camera->bindToShader(shdr);
    shdr = shaders.addShader("./shaders/defMesh_vs.glsl", "./shaders/mesh_fs.glsl", "deformMeshShader");
    lights.bindToShader(shdr);

    shdr = shaders.addShader("./shaders/mesh_vs.glsl", "./shaders/mesh_fs.glsl", "meshShader");
    lights.bindToShader(shdr);

    camera->bindToShader(shdr);

}

void Scene::drawScene() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);
    glm::mat4 viewMat=camera->getViewMat();
    camera->updateViewMat();
    for (auto i=0; i<objectPointers.size(); i++) 
    {
        (objectPointers[i])->update();
        (objectPointers[i])->draw();
    }
}
void Scene::addMeshes(std::string fileName, Object* parent)
{
    MeshNode *meshNode = new MeshNode();
    meshNode->loadSimpleOBJ(fileName);
    meshNode->setParent(parent);
    objectPointers.push_back(meshNode);
}



void Scene::setCamera(unsigned int type, glm::vec3 transVec, glm::mat4 rotMat)
{
    switch (type)
    {
        case CAMERA_ARCBALL:
            camera = new Arcball();
            break;
        default:
            camera = new Arcball();
            break;
    }
    camera->addRotation(rotMat);
    camera->addTranslation(transVec);
}


void Scene::updateProjMat(int W, int H)
{
    camera->updateProjectionMat(W, H);
}

void Scene::addDirLight(const glm::vec4 &dir)
{
    lights.addLight(dir);
}

//Not for framework right now
//void Scene::addFluidSys(std::string name, Object* parent)
//{
//    Points* pSPH = new Points();
//    (pSPH)->init();
//    
//    Shader *shdr = new Shader;
//    shdr->createProgrammeFromFiles("./shaders/point_vs.glsl", "./shaders/point_gs.glsl","./shaders/point_fs.glsl");
//    pSPH->setShader(shdr);
//    pSPH->setParent(parent);
//    shaderPointers.push_back(shdr);
//    objectPointers.push_back(pSPH);
//    pSPH->setName(name);
//}

std::string Scene::getTreeView() const
{
    std::stringstream ss;
    for (int i=0; i<objectPointers.size(); i++)
        ss<<objectPointers[i]->getName()<<std::endl;
    return ss.str();
}

Object* Scene::getObject(std::string name)
{
    for (auto i=0; i<objectPointers.size(); i++)
        if ( objectPointers[i]->getName() == name)
            return objectPointers[i];
    return nullptr;
}
Object* Scene::getObject(int index)
{
    return objectPointers[index];
}


