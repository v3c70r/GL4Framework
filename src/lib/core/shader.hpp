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
#include <map>

#define MAX_NUM_DIRLIGHTS 5
#define MAX_NUM_POINTLIGHTS 5
#define MAX_SHADER_LENGTH (262144)


/**
 * @brief Store uniforms and uniform blocks information of shader program
 */
struct ShaderConfig
{
    std::map<std::string, GLint> uniforms;
    std::map<std::string, GLuint> uniformBlocks;
    ShaderConfig()
    {
        uniformBlocks["DirLights"]=0;
        uniformBlocks["CameraMats"]=1;
        uniformBlocks["ModelMats"]=2;
    }
    bool empty()
    {
        if ( uniformBlocks.size() == 0 && uniforms.size() == 0 ) 
            return true;
        return false;
    }
};

//namespace UNIFORM{
//    enum LOCATIONS
//    {
//        TEX=0,
//        count
//    };
//    /*
//       uniform mat4 modelViewMat;
//       uniform mat4 normalMat;
//       uniform mat4 projMat;
//       */
//    //Keep them in the same order of UNIFORM_LOCATIONS above
//    const std::string names[] =
//    {
//        "tex"
//    };
//}
//
//namespace UNIFORM_BLOCKS{   //or SSBO
//    enum UNIFORBLOCK_INDICES
//    {
//        MATERIAL=0,
//        DIR_LIGHTS,
//        BONES_TRANS,
//        CAMERA_MATS,
//        MODEL_MATS,
//        TIME,
//        count
//    };
//    const std::string names[] =
//    {
//        "Material",
//        "DirLights",
//        "BoneTrans",
//        "CameraMats",
//        "ModelMats",
//        "Time"
//    };
//    const GLint binding[]=
//    {
//        0,
//        1,
//        2,
//        3,
//        4,
//        5
//    };
//}
class Shader
{
    GLuint programme;
    ShaderConfig config;

    bool parseFileToStr( const char* file_name, char* shader_str, int max_len);
    void printShaderInfoLog(GLuint shader_index);
    bool createShader (const char* file_name, GLuint* shader, GLenum type);
    bool isProgrammeValid (GLuint sp);
    void printProgrammeInfoLog (GLuint sp) ;
    bool createProgramme (GLuint compute, GLuint *programme);
    bool createProgramme (GLuint vert, GLuint geom, GLuint frag, GLuint* programme);
    bool createProgramme (GLuint vert,  GLuint frag, GLuint* programme);

public:
    Shader()
    {
        ;
    }
    bool setConfig(const ShaderConfig &);
    const ShaderConfig& getConfig() const {return config;}
    bool createProgrammeFromFiles(const char* kernel_file_name);
    bool createProgrammeFromFiles ( const char* vert_file_name, const char* geometry_file_name ,const char* frag_file_name);
    bool createProgrammeFromFiles ( const char* vert_file_name, const char* frag_file_name);
    bool use();
    GLuint getProgramme() const;

    ~Shader()
    {
        glDeleteProgram(programme);
    }

    //Functions that set values to shader
    //bool setTexture(const GLint &tex);
    //bool bindMaterial(const GLuint &buffer);
    bool bindDirLights(const GLuint &buffer);
    //bool bindBoneTrans(const GLuint &buffer);
    bool bindCameraMats(const GLuint &buffer);
    //bool bindModelMats(const GLuint &buffer);
    //bool bindTime(const GLuint &buffer);
};
