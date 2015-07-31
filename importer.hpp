#pragma once
/**
 * Import 3D assets to scene
 * (Also a testing field for exceptions, and RAII)
 */
#include "scene.h"
#include "meshNode.hpp"
#include <string>
#include <exception>
#include <iostream>
#include <map>
#include <cstring>
#include <glm/gtc/type_ptr.hpp>
#define NO_BONE 9999

class Importer
{
    //bone related datastructures
    static const int NUM_BONES_PER_VERT=4;
    struct VertexBoneData
    {
        GLuint numBones;
        GLuint IDs[NUM_BONES_PER_VERT];
        GLfloat weights[NUM_BONES_PER_VERT];
        VertexBoneData()
        {
            numBones = 0;
            for (auto i=0; i<NUM_BONES_PER_VERT; i++)
            {
                IDs[i] = NO_BONE;
                weights[i] = 0.0f;
            }
        }
        void addBone(GLuint ID, float weight)
        {
            if (numBones < NUM_BONES_PER_VERT) {
                IDs[numBones] = ID;
                weights[numBones] = weight;
                numBones++;
            }
        }
    };
    struct BoneInfo
    {
        aiMatrix4x4 BoneOffset;
        aiMatrix4x4 FinalTrans;
    };
    struct Bone
    {
        vector<BoneInfo> boneInfos;
    };
    std::map<std::string, int> boneMapping;
    std::vector<BoneInfo> boneInfos;
    int numBones;


private:    
    Scene *scene;
    std::string base;
    std::string fileName;
    void importScene(aiNode *pNode,const aiScene* as);
public:
    Importer(Scene *s=nullptr):scene(s), numBones(0) {}
    void setScene(Scene *s){scene = s;}
    void import(std::string fileName, std::string parentName);

    /** Import meshes and bones **/
    void import(std::string fileName, Object *parent=nullptr);
};

inline glm::mat4 aiMatToGlmMat(const aiMatrix4x4 &aiM)
{
    float mat[16];
    mat[0] = aiM.a1;
    mat[1] = aiM.a2;
    mat[2] = aiM.a3;
    mat[3] = aiM.a4;
    mat[4] = aiM.b1;
    mat[5] = aiM.b2;
    mat[6] = aiM.b3;
    mat[7] = aiM.b4;
    mat[8] = aiM.c1;
    mat[9] = aiM.c2;
    mat[10] = aiM.c3;
    mat[11] = aiM.c4;
    mat[12] = aiM.d1;
    mat[13] = aiM.d2;
    mat[14] = aiM.d3;
    mat[15] = aiM.d4;
    return glm::transpose(glm::make_mat4(mat));
}

inline void parseToRoot(vector<int> tree, int curPoint)
{
    std::cout<< curPoint<<std::endl;
    if (curPoint == -1) return;
    parseToRoot(tree, tree[curPoint]);
}

inline glm::mat4 getGlobalTrans(const vector<vector<glm::mat4>> &anim,const vector<int> &parList, int curPoint, int frameIdx)
{
    if (parList[curPoint] == -1)
        return anim[curPoint][frameIdx];
    else
        return getGlobalTrans(anim, parList, parList[curPoint],frameIdx) * anim[curPoint][frameIdx];
}

