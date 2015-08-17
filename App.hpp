#pragma once

#include "shader.hpp"
#include "log.hpp"
#include "arcball.h"
#include "ImguiConfig.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "light.hpp"
#include "meshNode.hpp"
#include "scene.h"
#include "shadowMap.h"
#include "importer.hpp"


extern const unsigned int CAMERA_ARCBALL;


class App
{
public:
    App();
    //~App();
    static void init();
    static void run();
    static void deactivate();
private:

    static Scene scene;

    static bool startGL();
    static void setupLight();
    static void updateProj();
    static void updateModelView();
    
    //Window parameters
    static int windowWidth;
    static int windowHeight;
    static GLFWwindow* pWindow;
    //callback functions
    static void _glfwErrHandler(int error, const char * description);
    static void _glfw_window_size_callback (GLFWwindow* window, int width, int height);
    static void mouseButton(GLFWwindow* window, int button, int action, int mods);
    static void mouseMotion(GLFWwindow *window, double x, double y);
    static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

};
