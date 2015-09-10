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
    lights.addLight(glm::vec4(0.0, 0.0, 1.0, 0.0));
    lights.addLight(glm::vec4(0.0, 0.0, -1.0, 0.0));
    setCamera(CAMERA_ARCBALL, glm::vec3(0.0, 0.0, -15.0), glm::mat4x4(1.0));
    camera->init();

    Shader * shdr = nullptr;

    //Raytracer
    RayTracer* rayTracer = new RayTracer(wWidth, wHeight);
    shdr = shaders.addShader("./shaders/kernels/rayTracing.glsl", "RAY_TRACING");
    camera->bindToShader(shdr);
    rayTracer->setCompShader(shdr);
    renderers.addRenderer(rayTracer, "RayTracer");

    //default renderer for static meshes
    //ForwardRenderer *fwRendererMesh = new ForwardRenderer;
    //shdr = shaders.addShader("./shaders/mesh_vs.glsl", "./shaders/mesh_fs.glsl", "deformMeshShader");
    //camera->bindToShader(shdr);
    //lights.bindToShader(shdr);
    //fwRendererMesh->setShader(shdr);
    //renderers.addRenderer(fwRendererMesh, "FW_STATIC_MESH_R");


    ////default renderer for LBS meshes
    //ForwardRenderer *fwRendererLBS = new ForwardRenderer;
    //shdr = shaders.addShader("./shaders/defMesh_vs.glsl", "./shaders/mesh_fs.glsl", "meshShader");
    //lights.bindToShader(shdr);
    //fwRendererLBS->setShader(shdr);
    //camera->bindToShader(shdr);
    //renderers.addRenderer(fwRendererLBS, "FW_LBS_MESH_R");

    //Deferred Renderer
    //DeferredRenderer* dfRendererMesh = new DeferredRenderer(wWidth, wHeight);
    //shdr = shaders.addShader("./shaders/deferredGeo_vs.glsl", "./shaders/deferredGeo_fs.glsl", "deffered");
    //dfRendererMesh->setGeometryShader(shdr);
    //lights.bindToShader(shdr);
    //renderers.addRenderer(dfRendererMesh, "DF_MESH_R");



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

void Scene::addDirLight(const glm::vec4 &dir)
{
    lights.addLight(dir);
}

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


