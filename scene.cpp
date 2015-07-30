#include "scene.h"

/*
 * Init scene, 
 * setting default settings
 * need to be called after OpenGL context intialized
 */

void Scene::init()
{
    //initialzie default shaders
    addShader("./shaders/defMesh_vs.glsl", "./shaders/mesh_fs.glsl");
    //initialize lights
    lights = new LightManager();
    lights->addLight(glm::vec4(0.0, 0.0, 1.0, 0.0));
    lights->bindToShader(shaderPointers[BUILD_IN_SHADERS::MESH]);
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
    for (auto i=0; i<objectPointers.size(); i++) 
    {
        (objectPointers[i])->update();
        (objectPointers[i])->setModelViewMat(camera->getViewMat());
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

void Scene::import(std::string fileName, Object* parent)
{
}

void Scene::addShader(std::string vsFile, std::string gsFile, std::string fsFile)
{
    Shader *shdr = new Shader;
    GLuint dummyVAO;
    glGenVertexArrays(1, &dummyVAO);
    glBindVertexArray(dummyVAO);
    assert(shdr->createProgrammeFromFiles(vsFile.c_str(), gsFile.c_str(), fsFile.c_str()));
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &dummyVAO);
    shaderPointers.push_back(shdr);
}
void Scene::addShader(std::string vsFile,  std::string fsFile)
{
    Shader *shdr = new Shader;
    GLuint dummyVAO;
    glGenVertexArrays(1, &dummyVAO);
    glBindVertexArray(dummyVAO);
    assert(shdr->createProgrammeFromFiles(vsFile.c_str(), fsFile.c_str()));
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &dummyVAO);
    shaderPointers.push_back(shdr);
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
    for (auto i=0; i<shaderPointers.size(); i++)
        shaderPointers[i]->setProjMat(camera->getProjectionMat());
}

void Scene::addDirLight(const glm::vec4 &dir)
{
    lights->addLight(dir);
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


