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
    //whatever
    int flags = aiProcess_Triangulate;
    flags |= aiProcess_JoinIdenticalVertices;
    flags |= aiProcess_GenSmoothNormals;
    flags |= aiProcess_GenUVCoords;
    flags |= aiProcess_TransformUVCoords;
    flags |= aiProcess_RemoveComponent;

    std::string base = fileName.substr(0, fileName.find_last_of("/")+1);

    Assimp::Importer importer;
    const aiScene * aiScn = importer.ReadFile(fileName, flags);
    if (!aiScn)
    {
        std::cout<<"Failed"<<std::endl;
        std::cout<<(importer.GetErrorString())<<std::endl;
        exit(-1);
    }
    
    std::cout<<aiScn->mNumAnimations<<std::endl;
    std::cout<<aiScn->mAnimations[0]->mChannels[0]->mNumRotationKeys<<std::endl;

    for (auto i=0; i<aiScn->mNumMeshes; i++)
    {
        MeshNode *meshNode = new MeshNode();

        aiMesh *curMesh = aiScn->mMeshes[i];
        if (curMesh->HasBones())
        {
            meshNode->init(curMesh->mNumFaces, curMesh->mNumVertices, meshNode->MAX_NUM_BONES);
            vector<float> weights;
            //max check
            if ( meshNode->MAX_NUM_BONES < curMesh->mNumBones)
                std::cout<<"[Warning] Too many bones"<<std::endl;

            weights.resize(meshNode->MAX_NUM_BONES * curMesh->mNumVertices, 0.0);
            int asdf = 0;
            for (int j=0; j<curMesh->mNumBones; j++)
            {
                for (int k=0; k<curMesh->mBones[j]->mNumWeights; k++)
                {
                    aiVertexWeight w = curMesh->mBones[j]->mWeights[k];
                    weights[w.mVertexId*meshNode->BONES_PER_VERTEX+k] =  w.mWeight;
                }
            }
            meshNode->setWeights(&weights[0]);
        }
        else
        {
            meshNode->init(curMesh->mNumFaces, curMesh->mNumVertices, 0);
        }


        meshNode->setVertices((GLfloat*)(curMesh->mVertices));
        vector<float> tempUV;
        tempUV.reserve(curMesh->mNumVertices * 2);
        if (curMesh->mTextureCoords[0])
        {
            for (auto j=0; j<curMesh->mNumVertices;j++)
            {
                tempUV.push_back(curMesh->mTextureCoords[0][j].x);
                tempUV.push_back(1.0 - curMesh->mTextureCoords[0][j].y);
            }
            meshNode->setTexCoord(&tempUV[0]);
        }

        meshNode->setNormals((GLfloat*)(curMesh->mNormals));

        //load material and texture
        aiMaterial *material = aiScn->mMaterials[curMesh->mMaterialIndex];
        aiString path;
        material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path);
        if (path.length != 0)
            meshNode->loadTexture(base+std::string(path.C_Str()));
        meshNode->setMaterial(material);

        vector<GLuint> tempFaces;
        tempFaces.reserve(3*curMesh->mNumFaces);
        for (auto j=0; j<curMesh->mNumFaces; j++)
        {
            tempFaces.push_back(curMesh->mFaces[j].mIndices[0]);
            tempFaces.push_back(curMesh->mFaces[j].mIndices[1]);
            tempFaces.push_back(curMesh->mFaces[j].mIndices[2]);
        }
        meshNode->setIndices(&tempFaces[0]);


        meshNode->setParent(parent);
        if (std::string(curMesh->mName.C_Str()) != "")
            meshNode->setName(std::string(curMesh->mName.C_Str()));
        else
        {
            int splitIdx = fileName.find_last_of("/");
            meshNode->setName(fileName.substr(splitIdx+1, fileName.size() - splitIdx)+std::string("_")+std::to_string(i));
        }
        meshNode->setShader(shaderPointers[0]);
        //set default shader
        objectPointers.push_back(meshNode);
    }
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


