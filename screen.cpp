#include "screen.hpp"
#define TEXTURE_WIDTH 1920
#define TEXTURE_HEIGHT 1080

bool Screen::init(int texWidth, int texHeight, std::string vsFile, std::string fsFile)
{

    // Initialize two textures
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, texWidth, texHeight, 0,GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);


    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, texWidth, texHeight, 0,GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //-------Render to texutre ------

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,  GL_DEPTH_COMPONENT, texWidth, texHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);


    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);


    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout<<glCheckFramebufferStatus(GL_FRAMEBUFFER)<<std::endl;
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };

    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    // Create and compile our GLSL program from the shaders
    assert(screenShader.createProgrammeFromFiles(vsFile.c_str(),  fsFile.c_str()));
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*18, g_quad_vertex_buffer_data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray (0);      //first attrib, the array
    quad_programID = screenShader.getProgramme();
    texID = glGetUniformLocation(quad_programID, "colorTexture");
    depthID = glGetUniformLocation(quad_programID, "depthTexture");
    timeID = glGetUniformLocation(quad_programID, "time");

    glBindVertexArray(0);

    //Compute shader
    assert(computeShader.createProgrammeFromFiles("./shaders/kernels/rayTracingComplete.glsl"));
    csIDs[0] = glGetUniformLocation(computeShader.getProgramme(), "eye");
    csIDs[1] = glGetUniformLocation(computeShader.getProgramme(), "ray00");
    csIDs[2] = glGetUniformLocation(computeShader.getProgramme(), "ray10");
    csIDs[3] = glGetUniformLocation(computeShader.getProgramme(), "ray01");
    csIDs[4] = glGetUniformLocation(computeShader.getProgramme(), "ray11");
    csIDs[5] = glGetUniformLocation(computeShader.getProgramme(), "blendFactor");
    csIDs[6] = glGetUniformLocation(computeShader.getProgramme(), "time");
    csIDs[7] = glGetUniformLocation(computeShader.getProgramme(), "bounceCount");
    
    return true;

}

void Screen::useScreen(const int &x, const int &y)
{
    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0,0,x,y); // Render on the whole framebuffer, complete from the lower left corner to the upper right
}

void Screen::renderScreen(const int &x,const int &y, Camera *c ,GLuint fbo)
{
    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0,0,x,y); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    // Clear the screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Do the computation buffer to modify the texture
    glUseProgram(computeShader.getProgramme());
    //setup camera
    glm::mat4 invViewProjMat = glm::inverse((c->getViewMat() * c->getProjectionMat()));
    glm::vec3 eyePos = -glm::transpose(glm::mat3(c->getViewMat()))*glm::vec3(c->getViewMat()[3]);
    //eyePos = glm::vec3(c->getProjectionMat() * glm::vec4(eyePos, 1.0));
    //eyePos = glm::vec3(0.0, 5.0, 0.0);
    glm::vec3 ray00 = glm::vec3(glm::vec4(-1.0, -1.0, 0.0, 1.0) * invViewProjMat) - eyePos;
    glm::vec3 ray10 = glm::vec3(glm::vec4(1.0, -1.0, 0.0, 1.0) * invViewProjMat) - eyePos;
    glm::vec3 ray01 = glm::vec3(glm::vec4(-1.0, 1.0, 0.0, 1.0) * invViewProjMat) - eyePos;
    glm::vec3 ray11 = glm::vec3(glm::vec4(1.0, 1.0, 0.0, 1.0) * invViewProjMat) - eyePos;
    float blendFac = -1.0;
    float time = glfwGetTime();
    std::cout<<time<<std::endl;
    glUniform3fv(csIDs[0], 1, &eyePos[0]);
    glUniform3fv(csIDs[1], 1, &ray00[0]);
    glUniform3fv(csIDs[2], 1, &ray10[0]);
    glUniform3fv(csIDs[3], 1, &ray01[0]);
    glUniform3fv(csIDs[4], 1, &ray11[0]);
    glUniform1f(csIDs[5], blendFac);
    glUniform1f(csIDs[6], time);
    glUniform1i(csIDs[7], 1);

    glBindImageTexture( 0, 
            colorTexture,
            0,
            GL_FALSE,
            0,
            GL_READ_WRITE,
            GL_RGBA32F);
    //glDispatchCompute(2048, 2048, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Use our shader
    glUseProgram(quad_programID);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glUniform1i(texID, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(depthID, 1);
    // Set our "renderedTexture" sampler to user Texture Unit 0
    glUniform1f(timeID, (float)(glfwGetTime()*10.0f) );
    // 1rst attribute buffer : vertices
    glBindVertexArray(vao);
    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
    glBindVertexArray(0);

}
void Screen::deactivate()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &colorTexture);
	glDeleteBuffers(1, &quad_vertexbuffer);
    glDeleteVertexArrays(1, &vao);
}

