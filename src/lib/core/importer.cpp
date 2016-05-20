#include <core/importer.hpp>
#include <assimp>

void Importer::processNodes_(aiNode* node,
        const aiScene* scene)
{
    for (GLuint i=0;i<node->mNumMeshes; i++)
    {
        // The node object only contains indices to index the actual objects in the scene. 
        // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
        this->meshes.push_back(this->processMesh(mesh, scene));		
    }
    // Depth first search
    for (GLuint i=0; i<node->mNumChildren; i++)
        processNodes_(node->mChildren[i], scene);
}
void Importer::importScene(const std::string &fileName, const Object *parent)
{
    Assimp::Importer aiImporter;
    auto aiScene = aiImporter.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if(!aiScene || aiScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode) // if is Not Zero
    {
        LOG::writeLogErr("Failed to import from %s with Assimp error %s\n", fileName.c_str(), aiImporter.GetErrorString());
        return;
    }
    directory_ = fileName.substr(0, fileName.find_last_of('/'));
    
}
