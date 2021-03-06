#include "scene.h"

/*
 * Init scene, 
 * setting default settings
 * need to be called after OpenGL context intialized
 */

void Scene::init(const GLint &wWidth, const GLint &wHeight)
{
    //initialzie default shaders
    lights.init();
    setCamera(CAMERA_ARCBALL, glm::vec3(0.0f, 0.0f, -15.0f), glm::mat4x4(1.0));
    camera->init();


    //Raytracer
    //RayTracer* rayTracer = new RayTracer(wWidth, wHeight);
    //shdr = shaders.addShader("./shaders/kernels/rayTracing.glsl", "RAY_TRACING");
    //camera->bindToShader(shdr);
    //rayTracer->setCompShader(shdr);
    //renderers.addRenderer(rayTracer, "RayTracer");


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
    camera->updateViewMat();
    renderers.renderAll();
}


//Deprecating
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


std::string Scene::getTreeView() const
{
    std::stringstream ss;
    for (const auto& elem: objectPointers)
        ss<<elem->getName()<<std::endl;
    return ss.str();
}

Object* Scene::getObject(std::string name) const
{
    for (const auto& elem: objectPointers)
        if (elem->getName() == name)
            return elem;
    return nullptr;
}
Object* Scene::getObject(int index) const
{
    return objectPointers[index];
}


