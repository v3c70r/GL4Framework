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
#include <glm/gtc/type_ptr.hpp>
class Importer
{
private:    
    Scene *scene;
    std::string base;
    std::string fileName;
    void importScene(aiNode *pNode,const aiScene* as);
public:
    Importer(Scene *s=nullptr):scene(s){}
    void setScene(Scene *s){scene = s;}
    void import(std::string fileName, std::string parentName);
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

