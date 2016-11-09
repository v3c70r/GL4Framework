#pragma once

#include <core/shader.hpp>
#include <core/log.hpp>
#include <camera/arcball/arcball.h>
#include <core/ImguiConfig.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <core/light.hpp>
#include <objects/meshNode.hpp>
#include <core/scene.h>

// Object requires CUDA
#ifdef CUDA
    #include <objects/point/points.cuh>
#endif

#include <core/importer.hpp>
#include "pyConsole.h"


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

    static void handleJoystick(int joystick);

};
