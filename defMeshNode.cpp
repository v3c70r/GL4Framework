#include "defMeshNode.hpp"
/*
 * Overwriting init function to add init GPU location for bones
 */
void DefMeshNode::init(GLuint nFaces, GLuint nVertices){
    std::cout<<"init in DefMeshNode\n";
    VBO = new GLuint[MESH_ATTR::COUNT];
    numOfFaces = nFaces;
    numOfVertices = nVertices;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(MESH_ATTR::COUNT, VBO);
    GLfloat *emptyFloat=new GLfloat[3*numOfVertices];
    GLuint *emptyUint=new GLuint[3*numOfFaces];

    glBindBuffer(GL_ARRAY_BUFFER, VBO[MESH_ATTR::VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*numOfVertices, emptyFloat, GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_ATTR::VERTICES);
    glVertexAttribPointer(MESH_ATTR::VERTICES, 3, GL_FLOAT, 0, 0, 0);

    //Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[MESH_ATTR::NORMALS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*numOfVertices, emptyFloat, GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_ATTR::NORMALS);
    glVertexAttribPointer(MESH_ATTR::NORMALS, 3, GL_FLOAT, 0, 0, 0);

    //texCoords
    glBindBuffer(GL_ARRAY_BUFFER, VBO[MESH_ATTR::TEXCOORDS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*2*numOfVertices, emptyFloat, GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_ATTR::TEXCOORDS);
    glVertexAttribPointer(MESH_ATTR::TEXCOORDS, 2, GL_FLOAT, 0, 0, 0);

    //Bones Weights
    glBindBuffer(GL_ARRAY_BUFFER, VBO[MESH_ATTR::WEIGHTS]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[MESH_ATTR::WEIGHTS]);
    glEnableVertexAttribArray(MESH_ATTR::WEIGHTS);
    glVertexAttribPointer(MESH_ATTR::WEIGHTS, 4, GL_FLOAT, 0, 0, 0);

    //Bone IDs
    glBindBuffer(GL_ARRAY_BUFFER, VBO[MESH_ATTR::IDS]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[MESH_ATTR::IDS]);
    glEnableVertexAttribArray(MESH_ATTR::IDS);
    glVertexAttribPointer(MESH_ATTR::IDS, 4, GL_FLOAT, 0, 0, 0);

    //indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[MESH_ATTR::INDICES]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*3*numOfFaces, emptyUint, GL_STATIC_DRAW);

    //Bones Transformations
    glBindBuffer(GL_UNIFORM_BUFFER, VBO[MESH_ATTR::BONES_TRANS]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof (GLfloat) *16* MAX_NUM_BONES, 0, GL_DYNAMIC_DRAW);


    //clean up
    glBindVertexArray(0);
    //Uniform buffer
    glBindBuffer(GL_UNIFORM_BUFFER, VBO[MESH_ATTR::MATERIAL]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16, 0, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    delete []emptyFloat;
    delete []emptyUint;
}

//Overwriting to bind bone trans
void DefMeshNode::setShader(Shader *s)
{
    shader = s;
    shader->bindMaterial(VBO[MESH_ATTR::MATERIAL]);
    shader->bindBoneTrans(VBO[MESH_ATTR::BONES_TRANS]);
}
void DefMeshNode::addAnimation(Animation anim)
{
    animations.push_back(anim);
}
void DefMeshNode::setBoneTrans(const GLfloat* trans, const GLuint &numBones)
{
    glBindBuffer(GL_UNIFORM_BUFFER, VBO[MESH_ATTR::BONES_TRANS]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16* numBones,trans ,GL_DYNAMIC_DRAW);
}
void DefMeshNode::update()
{
    shader->setModelViewMat(modelView);
    shader->setNormalMat(glm::transpose(glm::inverse(modelView)));
    shader->setTexture(0);
    shader->bindMaterial(VBO[MESH_ATTR::MATERIAL]);

    if (animations.size() == 0) return;
    setBoneTrans( &(animations[0][frameIdx][0][0][0]), 20);
    frameIdx = ( frameIdx + 1 )%(animations[0].size()-1);
    shader->bindBoneTrans(VBO[MESH_ATTR::BONES_TRANS]);
}
void DefMeshNode::setWeights(const GLuint *IDs, const GLfloat* weights)
{
    //Weights are in VAO, so it's safe
    glBindBuffer(GL_ARRAY_BUFFER, VBO[MESH_ATTR::IDS]);
    glBufferData(GL_ARRAY_BUFFER, numOfVertices * 4 * sizeof(GLuint), IDs, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[MESH_ATTR::WEIGHTS]);
    glBufferData(GL_ARRAY_BUFFER, numOfVertices * 4 * sizeof(GLfloat), weights, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
