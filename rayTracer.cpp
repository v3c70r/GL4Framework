#include "rayTracer.h"


RayTracer::RayTracer( const int &width, const int &height)
{
    this->width = width;
    this->height = height;
    //Init texture
    glGenTextures(1,  &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Init quad
    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };

    glGenVertexArrays (1, &quadVAO);
    glBindVertexArray (quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*18, g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray (0);      //first attrib, the array
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    assert(quadShdr.createProgrammeFromFiles("./shaders/screen_vs.glsl", "./shaders/screen_fs.glsl"));
    glUseProgram(quadShdr.getProgramme());
    texID = glGetUniformLocation(quadShdr.getProgramme(), "colorTexture");
    glUniform1i(texID, 0);
    glUseProgram(0);
}

void RayTracer::render() const
{
    glUseProgram(compShdr->getProgramme());
    glBindImageTexture( 0, 
            texture,
            0,
            GL_FALSE,
            0,
            GL_READ_WRITE,
            GL_RGBA32F);
    glDispatchCompute(256, 256, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);
    
    
    glUseProgram(quadShdr.getProgramme());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glUseProgram(0);
}


