
#pragma once
#include <core/scene.h>
#include <string>
class Importer
{
    Scene &scene_;
    std::string directory_;
    void processMesh_();
    void processNodes_(aiNode* node,
            const aiScene* scene);
public: 
    explicit Importer(Scene& s):scene_(s){}
    void importScene(const std::string &fileName, const Object *parent=nullptr);
    void importScene(const std::string &fileName, const std::string &parentObjName);
};

