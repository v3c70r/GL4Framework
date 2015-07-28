#include "importer.hpp"
void Importer::import(std::string fileName, Object *parent)
{
    int flags = aiProcess_Triangulate;
    flags |= aiProcess_JoinIdenticalVertices;
    flags |= aiProcess_GenSmoothNormals;
    flags |= aiProcess_GenUVCoords;
    flags |= aiProcess_TransformUVCoords;
    flags |= aiProcess_RemoveComponent;
    base = fileName.substr(0, fileName.find_last_of("/")+1);
    this->fileName = fileName;
    Assimp::Importer importer;
    const aiScene * aiScn = importer.ReadFile(fileName, flags);
    if( !scene)
    {
        throw std::runtime_error(importer.GetErrorString());
    }

    importScene(aiScn->mRootNode, aiScn);

    for (auto i=0; i<aiScn->mNumAnimations; i++)
    {
        Animation thisAnim;
        //load animations
        aiAnimation *anim  = aiScn->mAnimations[i];
        thisAnim.name = anim->mName.C_Str();
        thisAnim.numBones = anim->mNumChannels;
        std::cout<<anim->mDuration<<std::endl;
        std::cout<<anim->mTicksPerSecond<<std::endl;
        std::cout<<anim->mNumMeshChannels<<std::endl;

        float framesPerSec = 30.0;
        std::vector<std::vector<glm::mat4>> animVec;
        std::vector<aiNode*> nodeMap;
        std::vector<int> parentIdxMap;
        animVec.resize(anim->mNumChannels);
        nodeMap.resize(anim->mNumChannels);
        parentIdxMap.resize(anim->mNumChannels, -1);

        for (auto chID = 0; chID < anim->mNumChannels; chID++)
        {
            aiNodeAnim *ch = anim->mChannels[chID];
            aiNode *root = aiScn->mRootNode;
            nodeMap[chID] = root->FindNode(ch->mNodeName);
            for (auto rID=0; rID<ch->mNumRotationKeys-1; rID++)
            {
                const aiQuaternion& startQ = ch->mRotationKeys[rID].mValue;
                const aiQuaternion& EndQ = ch->mRotationKeys[rID+1].mValue;
                const aiVector3D startPos = ch->mPositionKeys[rID].mValue;
                const aiVector3D endPos = ch->mPositionKeys[rID+1].mValue;
                const aiVector3D startScal = ch->mScalingKeys[rID].mValue;
                const aiVector3D endScal = ch->mScalingKeys[rID+1].mValue;
                float DT = ch->mRotationKeys[rID+1].mTime - ch->mRotationKeys[rID].mTime ;
                float step = 1/(DT*60);
                for (float factor = 0.000000001; factor < 0.9999999999; factor += step)
                {
                    aiQuaternion out;
                    aiQuaternion::Interpolate(out, startQ, EndQ, factor);
                    out = out.Normalize();
                    glm::mat4 rotMat = aiMatToGlmMat(aiMatrix4x4(out.GetMatrix()));
                    aiVector3D outPos = (startPos + endPos)*step ;
                    aiVector3D outScal = (startScal + endScal)*step ;
                    glm::mat4 scalMat = glm::scale(glm::mat4(1.0), glm::vec3(outScal.x, outScal.y, outScal.z));
                    glm::mat4 posMat = glm::translate(glm::mat4(1.0), glm::vec3(outPos.x, outPos.y,outPos.z));
                    animVec[chID].push_back(scalMat*rotMat*posMat);
                }
            }
        }
        //Form parent index
        for(auto i=0; i<anim->mNumChannels; i++)
            for(auto j=0; j<anim->mNumChannels; j++)
                if (nodeMap[i]->mParent == nodeMap[j])
                    parentIdxMap[i] = j;

        std::cout<<glm::to_string(getGlobalTrans(animVec, parentIdxMap, 3, 0));
        vector<vector<glm::mat4>> frameAnimation;


        frameAnimation.resize(animVec[0].size());
        for (auto i=0; i<animVec[0].size(); i++)    //for each frame
            for (auto j=0; j<animVec.size(); j++)   //for each bone
                frameAnimation[i].push_back(getGlobalTrans(animVec,parentIdxMap, j, i));
        thisAnim.frames = frameAnimation;
        ((MeshNode*)scene->objectPointers[0])->addAnimation(thisAnim);
    }
}

void Importer::importScene(aiNode *pNode, const aiScene* as)
{
    //load meshes
    for (auto i=0; i<pNode->mNumMeshes; i++)
    {
        std::cout<<pNode->mName.C_Str()<<std::endl;
        std::cout<<pNode->mNumMeshes<<std::endl;
        MeshNode *meshNode = new MeshNode();
        aiMesh *curMesh =as->mMeshes[pNode->mMeshes[i]];
        //load bones
        if (curMesh->HasBones())
        {
            meshNode->init(curMesh->mNumFaces, curMesh->mNumVertices, meshNode->MAX_NUM_BONES);
            vector<float> weights;
            //max check
            if ( meshNode->MAX_NUM_BONES < curMesh->mNumBones)
                throw std::runtime_error("Too many bones");
            weights.resize(meshNode->MAX_NUM_BONES * curMesh->mNumVertices+1, 0.0);
            weights[0] = (float)curMesh->mNumBones+0.1;
            for (int j=0; j<curMesh->mNumBones; j++)
            {
                for (int k=0; k<curMesh->mBones[j]->mNumWeights; k++)
                {
                    aiVertexWeight w = curMesh->mBones[j]->mWeights[k];
                    weights[w.mVertexId*meshNode->BONES_PER_VERTEX+k+1] =  w.mWeight;
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
        //mesh loaded, setting other vars
        meshNode->setNormals((GLfloat*)(curMesh->mNormals));
        meshNode->setTransMat(aiMatToGlmMat(pNode->mTransformation));
        meshNode->setName(pNode->mName.C_Str());
        //Since it's top-down parse, this node's parents must be in the object list already
        //check for parent
        meshNode->setParent(nullptr);
        for (auto j=0; j<scene->objectPointers.size(); j++)
            if (scene->objectPointers[j]->getName() == pNode->mParent->mName.C_Str())
                meshNode->setParent(scene->objectPointers[j]);
        
        //load material and texture
        aiMaterial *material = as->mMaterials[curMesh->mMaterialIndex];
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


        meshNode->setParent(nullptr);
        if (std::string(curMesh->mName.C_Str()) != "")
            meshNode->setName(std::string(curMesh->mName.C_Str()));
        else
        {
            int splitIdx = fileName.find_last_of("/");
            meshNode->setName(fileName.substr(splitIdx+1, fileName.size() - splitIdx)+std::string("_")+std::to_string(i));
        }
        meshNode->setShader(scene->shaderPointers[0]);
        //set default shader
        scene->objectPointers.push_back(meshNode);
    }
    //Go children
    for (auto i=0; i<pNode->mNumChildren; i++)
        importScene(pNode->mChildren[i], as);
}

