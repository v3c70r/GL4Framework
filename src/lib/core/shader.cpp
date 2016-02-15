#include "log.hpp"
#include "shader.hpp"
#include <iostream>
using namespace LOG;
bool Shader::parseFileToStr(
        const char* file_name, char* shader_str, int max_len)
{
    shader_str[0] = '\0'; // reset string
    FILE* file = fopen (file_name , "r");
    if (!file) {
        writeLogErr ("ERROR: opening file for reading: %s\n", file_name);
        return false;
    }
    int current_len = 0;
    char line[2048];
    strcpy (line, ""); // remember to clean up before using for first time!
    while (!feof (file)) {
        if (NULL != fgets (line, 2048, file)) {
            current_len += strlen (line); // +1 for \n at end
            if (current_len >= max_len) {
                writeLogErr (
                        "ERROR: shader length is longer than string buffer length %i\n",
                        max_len
                        );
            }
            strcat (shader_str, line);
        }
    }
    if (EOF == fclose (file)) { // probably unnecesssary validation
        writeLogErr ("ERROR: closing file from reading %s\n", file_name);
        return false;
    }
    return true;
}

void Shader::printShaderInfoLog(GLuint shader_index)
{
    int max_length = 2048;
    int actual_length = 0;
    char log[2048];
    glGetShaderInfoLog (shader_index, max_length, &actual_length, log);
    printf ("shader info log for GL index %i:\n%s\n", shader_index, log);
    writeLog ("shader info log for GL index %i:\n%s\n", shader_index, log);
}
bool Shader::createShader (const char* file_name, GLuint* shader, GLenum type)
{
    writeLog ("creating shader from %s...\n", file_name);
    char shader_string[MAX_SHADER_LENGTH];
    assert (parseFileToStr (file_name, shader_string, MAX_SHADER_LENGTH));
    *shader = glCreateShader (type);
    const GLchar* p = (const GLchar*)shader_string;
    glShaderSource (*shader, 1, &p, NULL);
    glCompileShader (*shader);
    // check for compile errors
    int params = -1;
    glGetShaderiv (*shader, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params) {
        writeLogErr ("ERROR: GL shader index %i did not compile\n", *shader);
        printShaderInfoLog (*shader);
        return false; // or exit or something
    }
    writeLog ("shader compiled. index %i\n", *shader);
    return true;
}
bool Shader::isProgrammeValid (GLuint sp) {
    glValidateProgram (sp);
    GLint params = -1;
    glGetProgramiv (sp, GL_VALIDATE_STATUS, &params);
    if (GL_TRUE != params) {
        writeLogErr ("program %i GL_VALIDATE_STATUS = GL_FALSE\n", sp);
        printProgrammeInfoLog (sp);
        return false;
    }
    writeLog ("program %i GL_VALIDATE_STATUS = GL_TRUE\n", sp);
    return true;
}
void Shader::printProgrammeInfoLog (GLuint sp) 
{
    int max_length = 2048;
    int actual_length = 0;
    char log[2048];
    glGetProgramInfoLog (sp, max_length, &actual_length, log);
    printf ("program info log for GL index %u:\n%s", sp, log);
    writeLog ("program info log for GL index %u:\n%s", sp, log);
}
bool Shader::createProgramme (GLuint vert, GLuint geom ,GLuint frag, GLuint* programme) {
    *programme = glCreateProgram ();
    writeLog (
            "created programme %u. attaching shaders %u, %u and %u...\n",
            *programme,
            vert,
            geom,
            frag
            );
    glAttachShader (*programme, vert);
    glAttachShader (*programme, geom);
    glAttachShader (*programme, frag);
    // link the shader programme. if binding input attributes do that before link
    glLinkProgram (*programme);
    GLint params = -1;
    glGetProgramiv (*programme, GL_LINK_STATUS, &params);
    if (GL_TRUE != params) {
        writeLogErr (
                "ERROR: could not link shader programme GL index %u\n",
                *programme
                );
        printProgrammeInfoLog (*programme);
        return false;
    }
    assert (isProgrammeValid (*programme));
    // delete shaders here to free memory
    glDeleteShader (vert);
    glDeleteShader (geom);
    glDeleteShader (frag);
    return true;
}
bool Shader::createProgramme (GLuint compute, GLuint* programme)
{
    *programme = glCreateProgram();
    writeLog (
            "created compute programme %u. attaching shaders %u ...\n",
            *programme,
            compute
            );
    glAttachShader(*programme, compute);
    glLinkProgram(*programme);
    GLint params = -1;
    glGetProgramiv (*programme, GL_LINK_STATUS, &params);
    glGetProgramiv (*programme, GL_LINK_STATUS, &params);
    if (GL_TRUE != params) {
        writeLogErr (
                "ERROR: could not link shader programme GL index %u\n",
                *programme
                );
        printProgrammeInfoLog (*programme);
        return false;
    }
    glDeleteShader(compute);
    return true;
}
bool Shader::createProgramme (GLuint vert,  GLuint frag, GLuint* programme)
{
    *programme = glCreateProgram ();
    writeLog (
            "created programme %u. attaching shaders %u and %u...\n",
            *programme,
            vert,
            frag
            );
    glAttachShader (*programme, vert);
    glAttachShader (*programme, frag);
    // link the shader programme. if binding input attributes do that before link
    glLinkProgram (*programme);
    GLint params = -1;
    glGetProgramiv (*programme, GL_LINK_STATUS, &params);
    if (GL_TRUE != params) {
        writeLogErr (
                "ERROR: could not link shader programme GL index %u\n",
                *programme
                );
        printProgrammeInfoLog (*programme);
        return false;
    }
    assert (isProgrammeValid (*programme));
    // delete shaders here to free memory
    glDeleteShader (vert);
    glDeleteShader (frag);
    return true;
}
bool Shader::createProgrammeFromFiles(const char* kernel_file_name)
{
    GLuint computeShader;
    assert( createShader(kernel_file_name, &computeShader, GL_COMPUTE_SHADER));
    assert(createProgramme(computeShader, &programme));
    assert (getIndicesNLocations());
    return true;
}

bool Shader::createProgrammeFromFiles ( const char* vert_file_name, const char* geometry_file_name ,const char* frag_file_name)
{
    GLuint vertShader;
    GLuint geometryShader;
    GLuint fragShader;
    assert (createShader (vert_file_name, &vertShader, GL_VERTEX_SHADER));
    assert (createShader (geometry_file_name, &geometryShader, GL_GEOMETRY_SHADER));
    assert (createShader (frag_file_name, &fragShader, GL_FRAGMENT_SHADER));
    assert (createProgramme (vertShader, geometryShader ,fragShader, &programme));
    assert (getIndicesNLocations());
    return true;
}
bool Shader::createProgrammeFromFiles ( const char* vert_file_name, const char* frag_file_name)
{
    GLuint vertShader;
    GLuint fragShader;
    assert (createShader (vert_file_name, &vertShader, GL_VERTEX_SHADER));
    assert (createShader (frag_file_name, &fragShader, GL_FRAGMENT_SHADER));
    assert (createProgramme (vertShader, fragShader, &programme));
    assert (getIndicesNLocations());
    return true;
}

bool Shader::use()
{
    glUseProgram(programme);
    return true;
}

GLuint Shader::getProgramme() const
{
    return programme;
}

bool Shader::getIndicesNLocations()
{
    //Uniform Locations
    for (int i=0; i<UNIFORM::count;i++)
        Ulocations.push_back(glGetUniformLocation(programme, UNIFORM::names[i].c_str()));
    //bindings are hard coded in shaders
    for (int i=0; i<UNIFORM_BLOCKS::count; i++)
    {
        //GLint index =glGetProgramResourceIndex(programme, GL_UNIFORM_BLOCK,UNIFORM_BLOCKS::names[i].c_str()) ;
        ////UBIndices.push_back(index);
        ////set binding point
        GLint index = glGetUniformBlockIndex(programme, UNIFORM_BLOCKS::names[i].c_str());
        //std::cout<<"Binding UNIFORM\n" <<"Index: "<<index<<"\t"<<"Binding: "<<UNIFORM_BLOCKS::binding[i]<<"\t"<< UNIFORM_BLOCKS::names[i]<<std::endl;
        writeLog("[Binding UNIFOR] Index: %d \t Binding %d \t %s\n", index, UNIFORM_BLOCKS::binding[i], UNIFORM_BLOCKS::names[i].c_str());
        glUniformBlockBinding(programme ,index, UNIFORM_BLOCKS::binding[i]);
    }
    return true;
}


bool Shader::setTexture(const GLint &tex)
{
    glUseProgram(programme);
    glUniform1i(Ulocations[UNIFORM::TEX], tex);
    glUseProgram(0);
    return true;
}

bool Shader::bindMaterial(const GLuint &buffer)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BLOCKS::binding[UNIFORM_BLOCKS::MATERIAL], buffer);
    return true;
}
bool Shader::bindDirLights(const GLuint &buffer)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BLOCKS::binding[UNIFORM_BLOCKS::DIR_LIGHTS], buffer);
    return true;
}
bool Shader::bindBoneTrans(const GLuint &buffer)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BLOCKS::binding[UNIFORM_BLOCKS::BONES_TRANS], buffer);
    return true;
}

bool Shader::bindCameraMats(const GLuint &buffer)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BLOCKS::binding[UNIFORM_BLOCKS::CAMERA_MATS], buffer);
    return true;
}

bool Shader::bindModelMats(const GLuint &buffer)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BLOCKS::binding[UNIFORM_BLOCKS::MODEL_MATS], buffer);
    return true;
}
bool Shader::bindTime(const GLuint &buffer)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BLOCKS::binding[UNIFORM_BLOCKS::TIME], buffer);
    return true;
}
