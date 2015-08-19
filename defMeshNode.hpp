#pragma once
#include "meshNode.hpp"

//Each frame is an array of global(to this mesh) transformation for each bones
typedef std::vector<glm::mat4> Frame;
typedef std::vector<Frame> Animation;
static int frameIdx=0;

class DefMeshNode: public MeshNode
{
private:
    struct MESH_ATTR
    {
        enum ENUM
        {
            //attrb with fixed locations
            VERTICES=0,
            NORMALS,
            TEXCOORDS,
            WEIGHTS,
            IDS,
            INDICES,
            //uniform buffers
            MATERIAL,
            BONES_TRANS,
            OBJ_MATS,
            COUNT
        };
    };
    vector<Animation> animations;
    const int MAX_NUM_BONES;
    const int BONES_PER_VERTEX;

public:
    DefMeshNode(): MAX_NUM_BONES(20), BONES_PER_VERTEX(4)
    {
    }
    void init(GLuint nFaces, GLuint nVertices) override;

    void setBoneTrans(const GLfloat* trans, const GLuint &numBones);
    void setWeights(const GLuint *IDs, const GLfloat* weights);
    void addAnimation(Animation anim);

    void update() override;
    void bindShader(Shader *) override;
        
};
