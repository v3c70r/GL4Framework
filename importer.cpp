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
    if( !aiScn)
    {
        throw std::runtime_error(importer.GetErrorString());
    }
    printAiTree(aiScn->mRootNode, 0);
    importScene(aiScn->mRootNode, aiScn);
}

void Importer::importScene(aiNode *pNode, const aiScene* as)
{
    //load meshes
    for (auto i=0; i<pNode->mNumMeshes; i++)
    {
        MeshNode *meshNode = new MeshNode();
        aiMesh *curMesh =as->mMeshes[pNode->mMeshes[i]];
        //load bones
        if (curMesh->HasBones())
        {
            meshNode->init(curMesh->mNumFaces, curMesh->mNumVertices, meshNode->MAX_NUM_BONES);

            std::cout<<"Num Bones: "<<curMesh->mNumBones<<std::endl;
            std::vector<VertexBoneData> vetBoneDatas;
            vetBoneDatas.resize(curMesh->mNumVertices);

            for (auto boneId = 0; boneId < curMesh->mNumBones; boneId++)
            {
                uint BoneIndx = 0;
                aiBone * curBone = curMesh->mBones[boneId];
                std::string BoneName(curBone->mName.C_Str());
                if (boneMapping.find(BoneName) == boneMapping.end()) {  //bone is not found
                    BoneIndx = numBones;
                    numBones++;
                    BoneInfo bi;
                    bi.BoneOffset = curBone->mOffsetMatrix;
                    boneInfos.push_back(bi);
                }
                else{
                    BoneIndx = boneMapping[BoneName];
                }
                boneMapping[BoneName] = BoneIndx;
                //build bone vertex data
                for (auto weightID = 0; weightID < curBone->mNumWeights; weightID++)
                {
                    aiVertexWeight curWeight = curBone->mWeights[weightID];
                    if (abs(curWeight.mWeight - 0.0) < 0.0000001)
                        continue;
                    vetBoneDatas[curWeight.mVertexId].addBone(BoneIndx, curWeight.mWeight);
                }
            }
            //validation number of bones for each vertex;

            //form array
            vector<GLfloat> dWeights;
            dWeights.resize(NUM_BONES_PER_VERT * curMesh->mNumVertices, 0.0);
            vector<GLuint> dIDs;
            dIDs.resize(NUM_BONES_PER_VERT * curMesh->mNumVertices, NO_BONE);
            for (auto vertI=0; vertI<curMesh->mNumVertices; vertI++)
            {
                memcpy(&dWeights[vertI*NUM_BONES_PER_VERT], &vetBoneDatas[vertI].weights[0], sizeof(GLfloat)*NUM_BONES_PER_VERT);
                memcpy(&dIDs[vertI*NUM_BONES_PER_VERT], &vetBoneDatas[vertI].IDs[0], sizeof(GLuint)*NUM_BONES_PER_VERT);
            }
            meshNode->setWeights(&dIDs[0], &dWeights[0]);

            //===load animations
            for (auto animIdx = 0; animIdx < as->mNumAnimations; animIdx++)
            {
                meshNode->addAnimation(loadAnimation(as, as->mAnimations[animIdx]));
            }
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

vector<vector<glm::mat4>> Importer::loadAnimation(const aiScene *s,aiAnimation *anim)
{
    float duration=anim->mDuration;
    float frameRate =60.0;
    float DT = 1.0/frameRate;
    std::cout<<"Duration:"<<duration<<std::endl;
    vector<aiMatrix4x4> localTrans;
    localTrans.resize(boneInfos.size(), aiMatrix4x4());
    vector<vector<glm::mat4>> result;
    result.reserve( (int)frameRate * duration);
    for(float time = 0.0; time<duration; time += DT)
    {
        vector<glm::mat4> frame;
        frame.resize(anim->mNumChannels);
        for (auto chIdx=0; chIdx < anim->mNumChannels; chIdx++)
        {
            aiNodeAnim* curCh = anim->mChannels[chIdx];
            if (boneMapping.find(curCh->mNodeName.C_Str()) != boneMapping.end())        //if this channle exists in the bonemapping
            {
                localTrans[boneMapping[curCh->mNodeName.C_Str()]] = getTransMatByTime(curCh,time);
            }
            else
            {
                boneMapping[curCh->mNodeName.C_Str()] = localTrans.size()-1;
                localTrans.push_back( getTransMatByTime(curCh, time));
            }
        } 
        for (std::map<std::string,int>::iterator it=boneMapping.begin(); it!=boneMapping.end(); ++it)
        {
            aiNode *curNode = s->mRootNode->FindNode(aiString(it->first.c_str()));
            if (it->first == "LowerLegL")
            {
                aiMatrix4x4 i;
                //localTrans[it->second] = i;
            }
            if (curNode)
                frame[ it->second ] = aiMatToGlmMat(getGlobalTransFromLocalTrans(localTrans, curNode) * boneInfos[it->second].BoneOffset);
                //frame[ it->second ] = aiMatToGlmMat(localTrans[it->second]);
        }
        result.push_back(frame);
    }
    std::cout<<"Num Frames:"<<result.size()<<std::endl;
    return result;
}

aiMatrix4x4 Importer::getTransMatByTime(aiNodeAnim* ch, float time)
{
    //rotation
    aiQuaternion preRot;
    aiQuaternion afterRot;
    float factor = 0;
    for (auto keyIdx = 0; keyIdx<ch->mNumRotationKeys-1; keyIdx++) 
        if (time+0.000001 >= ch->mRotationKeys[keyIdx].mTime && time < ch->mRotationKeys[keyIdx+1].mTime){
            preRot = ch->mRotationKeys[keyIdx].mValue;
            afterRot = ch->mRotationKeys[keyIdx+1].mValue;
            factor = (time-ch->mRotationKeys[keyIdx].mTime) / (ch->mRotationKeys[keyIdx+1].mTime - ch->mRotationKeys[keyIdx].mTime);
            break;
        }
    aiQuaternion rotQuat; 
    aiQuaternion::Interpolate(rotQuat, preRot, afterRot, factor);
    aiMatrix4x4 rotM = (aiMatrix4x4(rotQuat.GetMatrix()));
    //std::cout<<"ROT MAT\n";
    //print_aiMatrix(rotM);
    //Scaling
    aiVector3D preScaling;
    aiVector3D afterScaling;
    factor = 0;
    for (auto keyIdx = 0; keyIdx<ch->mNumScalingKeys-1; keyIdx++)
        if (time+0.000001 >= ch->mScalingKeys[keyIdx].mTime && time < ch->mScalingKeys[keyIdx+1].mTime){
            preScaling = ch->mScalingKeys[keyIdx].mValue;
            afterScaling = ch->mScalingKeys[keyIdx+1].mValue;
            factor = (time-ch->mScalingKeys[keyIdx].mTime) / (ch->mScalingKeys[keyIdx+1].mTime - ch->mScalingKeys[keyIdx].mTime);
            break;
        }
    aiVector3D scalingVec=aiLERP(preScaling, afterScaling, factor);
    aiMatrix4x4 scalingM;
    aiMatrix4x4::Scaling(scalingVec, scalingM);
    //std::cout<<"SCAL MAT\n";
    //print_aiMatrix(scalingM);
    //translation
    aiVector3D preTrans;
    aiVector3D afterTrans;
    factor=0;
    for (auto keyIdx = 0; keyIdx<ch->mNumPositionKeys-1; keyIdx++)
        if (time+0.000001 >= ch->mPositionKeys[keyIdx].mTime && time < ch->mPositionKeys[keyIdx+1].mTime){
            preTrans = ch->mPositionKeys[keyIdx].mValue;
            afterTrans = ch->mPositionKeys[keyIdx+1].mValue;
            factor = (time-ch->mPositionKeys[keyIdx].mTime) / (ch->mPositionKeys[keyIdx+1].mTime - ch->mPositionKeys[keyIdx].mTime);
            break;
        }
    aiVector3D transVec = aiLERP(preTrans, afterTrans, factor);
    aiMatrix4x4 transM;
    aiMatrix4x4::Translation(transVec, transM);

    return transM * rotM * scalingM;
}

aiMatrix4x4 Importer::getGlobalTransFromLocalTrans(const std::vector<aiMatrix4x4> &localTrans, aiNode *n)
{
    aiMatrix4x4 i;
    if (!n)
        return i;

    aiNode* pNode = n->mParent;
    if ( boneMapping.find(n->mName.C_Str()) != boneMapping.end()){
        return getGlobalTransFromLocalTrans( localTrans, pNode) * localTrans[ boneMapping[n->mName.C_Str()]] ;
    }
    else return i;
    //if (pNode)
    //{
    //    if (boneMapping.find(n->mName.C_Str()) != boneMapping.end())    //current node is boneNode
    //        return getGlobalTransFromLocalTrans(localTrans, pNode) * localTrans[boneMapping[n->mName.C_Str()]];
    //    else
    //        return getGlobalTransFromLocalTrans(localTrans, pNode) * n->mTransformation;
    //}
    //else
    //{
    //    if (boneMapping.find(n->mName.C_Str()) != boneMapping.end()){
    //        return localTrans[boneMapping[n->mName.C_Str()]];
    //    }
    //    else {
    //        return n->mTransformation;
    //    }
    //}
}
