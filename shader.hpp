/*
 * Shader stuff
 */
#pragma once
#include <GL/glew.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#define MAX_NUM_DIRLIGHTS 5
#define MAX_NUM_POINTLIGHTS 5
#define MAX_SHADER_LENGTH (262144)

namespace UNIFORM{
    enum LOCATIONS
    {
        MODELVIEW_MAT=0,
        NORMAL_MAT,
        PROJ_MAT,
        TEX,
        count
    };
    /*
       uniform mat4 modelViewMat;
       uniform mat4 normalMat;
       uniform mat4 projMat;
       */
    //Keep them in the same order of UNIFORM_LOCATIONS above
    const std::string names[] =
    {
        "modelViewMat",
        "normalMat",
        "projMat",
        "tex"
    };
}

namespace UNIFORM_BLOCKS{
    enum UNIFORBLOCK_INDICES
    {
        MATERIAL=0,
        DIR_LIGHTS,
        BONES_TRANS,
        BONES_WEIGHTS,
        count
    };
    const std::string names[] =
    {
        "Material",
        "DirLights",
        "BoneTrans",
        "BoneWeights"
    };
    const GLint binding[]=
    {
        0,
        1,
        2,
        3
    };
}
class Shader
{
    GLuint programme;
    bool parseFileToStr( const char* file_name, char* shader_str, int max_len);
    void printShaderInfoLog(GLuint shader_index);
    bool createShader (const char* file_name, GLuint* shader, GLenum type);
    bool isProgrammeValid (GLuint sp);
    void printProgrammeInfoLog (GLuint sp) ;
    bool createProgramme (GLuint compute, GLuint *programme);
    bool createProgramme (GLuint vert, GLuint geom, GLuint frag, GLuint* programme);
    bool createProgramme (GLuint vert,  GLuint frag, GLuint* programme);

    std::vector<GLint> Ulocations;
    std::vector<GLint> UBIndices;
    bool getIndicesNLocations();
public:
    Shader()
    {
        ;
    }
    bool createProgrammeFromFiles(const char* kernel_file_name);
    bool createProgrammeFromFiles ( const char* vert_file_name, const char* geometry_file_name ,const char* frag_file_name);
    bool createProgrammeFromFiles ( const char* vert_file_name, const char* frag_file_name);
    bool use();
    GLuint getProgramme() const;

    //Functions that set values to shader
    bool setModelViewMat(const glm::mat4 &modelView);
    bool setNormalMat(const glm::mat4 &normalMat);
    bool setProjMat(const glm::mat4 &projMat);
    bool setNumDirLights(const int &num);
    bool setTexture(const GLint &tex);
    bool bindMaterial(const GLuint &buffer);
    bool bindDirLights(const GLuint &buffer);
    bool bindBoneTrans(const GLuint &buffer);
    bool bindBoneWeights(const GLuint &buffer);
    ~Shader()
    {
        glDeleteProgram(programme);
    }
};
