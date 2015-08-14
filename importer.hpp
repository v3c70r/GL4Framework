#pragma once
/**
 * Import 3D assets to scene
 * (Also a testing field for exceptions, and RAII)
 */
#include "scene.h"
#include "meshNode.hpp"
#include "defMeshNode.hpp"
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
            else
                throw std::runtime_error("To many bones for vertex");
        }
    };
    struct BoneInfo
    {
        aiMatrix4x4 BoneOffset;
        aiMatrix4x4 FinalTrans;
    };
    std::map<std::string, int> boneMapping;
    std::vector<BoneInfo> boneInfos;
    vector<vector<glm::mat4>> loadAnimation(const aiScene* s, aiAnimation *anim);
    aiMatrix4x4 getTransMatByTime(aiNodeAnim* ch, float time);
    int numBones;

    //helper functions
    inline aiVector3D aiLERP(const aiVector3D &start, const aiVector3D &end, float factor)
    {
        return factor*start+(1.0f-factor)*end;
    }
    inline void print_aiMatrix(const aiMatrix4x4 &m) const
    {
        std::cout<<"====PRINTING MATRIX=====\n";
        std::cout<<m.a1<<"\t"<<m.a2<<"\t"<<m.a3<<"\t"<<m.a4<<std::endl;
        std::cout<<m.b1<<"\t"<<m.b2<<"\t"<<m.b3<<"\t"<<m.b4<<std::endl;
        std::cout<<m.c1<<"\t"<<m.c2<<"\t"<<m.c3<<"\t"<<m.c4<<std::endl;
        std::cout<<m.d1<<"\t"<<m.d2<<"\t"<<m.d3<<"\t"<<m.d4<<std::endl;
    }
    aiMatrix4x4 getGlobalTransFromLocalTrans(const std::vector<aiMatrix4x4> &localTrans, aiNode *n);


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

/**Convert assimp matrix to glm matrix
 * Rom major to column major**/
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

inline void printAiTree(aiNode * pNode, int level)
{
    if (pNode)
    {
        std::string s(level, '-');
        std::cout<<s<<pNode->mName.C_Str()<<std::endl;
        for (auto i=0; i<pNode->mNumChildren; i++)
            printAiTree(pNode->mChildren[i], level+1);
    }
}


