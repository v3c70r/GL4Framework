#include "App.hpp"
#include <core/imgui_impl_glfw_gl3.h>
#include "camera/fpsCamera.h"

using namespace LOG;

GLFWwindow* App::pWindow = nullptr;
int App::windowWidth = 1920;
int App::windowHeight = 1080;
Scene App::scene;
int App::_joystick = -1;

App::App() {}
void App::_glfwErrHandler(int error, const char* description)
{
    fputs(description, stderr);
    writeLogErr("%s\n", description);
}
bool App::startGL()
{
    writeLog("starting GLFW %s", glfwGetVersionString());

    glfwSetErrorCallback(_glfwErrHandler);
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return false;
    }
    // uncomment these lines if on Apple OS X
    //#ifdef _APPLE_
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //#endif

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    windowWidth = mode->width;
    windowHeight = mode->height;
    windowWidth = 500;
    windowHeight = 500;

    pWindow =
        glfwCreateWindow(windowWidth, windowHeight, "MySPH", nullptr, nullptr);
    if (!pWindow) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return false;
    }
    ImGui_ImplGlfwGL3_Init(pWindow, true);
    glfwMakeContextCurrent(pWindow);
    glewExperimental = GL_TRUE;
    glewInit();
    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER);  // get renderer string
    const GLubyte* version = glGetString(GL_VERSION);    // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);
    writeLog("renderer: %s\nversion: %s\n", renderer, version);
    return true;
}

void App::init()
{
    std::cout << "Init APP\n";
    assert(restartLog());
    assert(startGL());

    int windowWidth, windowHeight;
    glfwGetFramebufferSize(pWindow, &windowWidth, &windowHeight);
    scene.init(windowWidth, windowHeight);

    Object* fluid = new Points();
    dynamic_cast<Points*>(fluid)->init();
    fluid->setName("fluid_1");
    fluid->setParent(nullptr);

    // Init shader
    PointRendererTem* fluidTemp = new PointRendererTem(1920, 1200);
    Shader* shader = scene.getShaderManager().addShader(
        "./shaders/pointTemp_vs.glsl", "./shaders/pointTemp_gs.glsl",
        "./shaders/pointTemp_fs.glsl", "TEMP_SHADER");
    scene.getRendererManager().addRenderer(fluidTemp, "TEMP_RENDERER");
    scene.getCamera()->bindToShader(shader);
    fluidTemp->setShader(shader);
    // Init renderer

    scene.addObject(fluid);
    scene.getRendererManager().assignObj2Renderer(fluid, "TEMP_RENDERER");
    dynamic_cast<Points*>(fluid)->insertParsFromOBJ("tyra.obj", 10.0,
                                                    0);

    glfwGetFramebufferSize(pWindow, &windowWidth, &windowHeight);
    scene.updateProjMat(windowWidth, windowHeight);
    /*--------Init lights-----------*/
    scene.getLightManager().addLight(glm::vec4(0.0, 0.0, 1.0, 0.0));
    scene.getLightManager().addLight(glm::vec4(0.0, 0.0, -1.0, 0.0));
    /*----------Init renderers-------*/
    /*-----------------------------register callbacks-----------------*/
    glfwSetWindowSizeCallback(pWindow, _glfw_window_size_callback);
    glfwSetMouseButtonCallback(pWindow, mouseButton);
    glfwSetCursorPosCallback(pWindow, mouseMotion);
    glfwSetKeyCallback(pWindow, keyCallBack);
    glfwSetScrollCallback(pWindow, scrollCallback);

    PyConsole& console = PyConsole::getInstance();
    console.setScene(&scene);
    console.runConsole();
}
void App::_glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glfwGetFramebufferSize(pWindow, &windowWidth, &windowHeight);
    scene.updateProjMat(windowWidth, windowHeight);
}
void App::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        switch (action) {
            case GLFW_PRESS:
                break;
            case GLFW_RELEASE:
                break;
        }
    }
}
void App::mouseMotion(GLFWwindow* window, double x, double y) {}
void App::keyCallBack(GLFWwindow* window, int key, int scancode, int action,
                      int mods)
{
    Points* p = (Points*)scene.getObject("fluid_1");
    FpsCamera* c = (FpsCamera*)(scene.getCamera());
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_W:
                c->forward(1.0);
                break;
            case GLFW_KEY_S:
                c->forward(-1.0);
                break;
            case GLFW_KEY_A:
                c->strafe(-1.0);
                break;
            case GLFW_KEY_D:
                c->strafe(1.0);
                break;
            case GLFW_KEY_F:
                p->insertParsFromOBJ(
                    "assets/meshes/Su-35_SuperFlanker/Su-35_SuperFlanker.obj",
                    20, 1);
                break;
            case GLFW_KEY_T:
                p->insertCUBE(glm::vec3(7, 0, 0), glm::vec3(0, 0, 0));
                break;
            case GLFW_KEY_ESCAPE:  // exit
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            default:
                break;
        }
        c->update();
    }
}
void App::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // scene.getCamera()->addTranslation(glm::vec3(0.0, 0.0, yoffset*0.5));
}
void App::run()
{
    glClearColor(0.5, 0.5, 0.5, 1.0);
    int width, height;
    _joystick = findJoystick(
        std::string("Sony Computer Entertainment Wireless Controller"));
    while (!glfwWindowShouldClose(pWindow)) {
        PyConsole::getInstance().callFunctions();
        ImGui_ImplGlfwGL3_NewFrame();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwGetFramebufferSize(pWindow, &width, &height);
        glViewport(0, 0, width, height);

        ImGui::Text("Frambuffer size %d, %d", width, height);
        ImGui::Text("Connected joystick %d: %s", _joystick,
                    glfwGetJoystickName(_joystick));
        handleJoystick(_joystick);

        // Calculate and show frame rate
        static float ms_per_frame[120] = {0};
        static int ms_per_frame_idx = 0;
        static float ms_per_frame_accum = 0.0f;
        ms_per_frame_accum -= ms_per_frame[ms_per_frame_idx];
        ms_per_frame[ms_per_frame_idx] = ImGui::GetIO().DeltaTime * 1000.0f;
        ms_per_frame_accum += ms_per_frame[ms_per_frame_idx];
        ms_per_frame_idx = (ms_per_frame_idx + 1) % 120;
        const float ms_per_frame_avg = ms_per_frame_accum / 120;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    ms_per_frame_avg, 1000.0f / ms_per_frame_avg);

        scene.drawScene();
        // Render GUI
        glDisable(GL_BLEND);
        ImGui::Render();

        glfwSwapInterval(0);
        glfwSwapBuffers(pWindow);
        glfwPollEvents();
    }
}

int App::findJoystick(std::string joystickName)
{
    for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++) {
        if (glfwJoystickPresent(i) == GLFW_TRUE) {
            if (std::string(glfwGetJoystickName(i)) == joystickName) {
                LOG::writeLogErr("Found\n");
                return i;
            }
        }
    }
    return -1;
}

void App::handleJoystick(int joystick)
{
    if (joystick == -1) return;
    int axisCount = 0;
    const float* axis = ::glfwGetJoystickAxes(joystick, &axisCount);
    int btnCount = 0;
    const unsigned char* btns = ::glfwGetJoystickButtons(joystick, &btnCount);
    FpsCamera* c = (FpsCamera*)(scene.getCamera());
    Points* p = (Points*)scene.getObject("fluid_1");
#if __APPLE__
    c->pitch(axis[3] / 50.0);
    c->yaw(axis[2] / 50.0);
    c->forward(-axis[1] / 10.0);
    c->strafe(axis[0] / 10.0);
    ImGui::Text("Axis %f, %f, %f, %f\n", axis[3], axis[2], axis[1], axis[0]);
#else
    c->pitch(axis[5] / 150.0);
    c->yaw(axis[2] / 150.0);
    c->forward(-axis[1] / 90.0);
    c->strafe(axis[0] / 90.0);
#endif
    glm::vec3 origin = c->getCameraPosition();
    glm::vec3 dir = -c->getLookAtDir() * 200.0f;
    //ImGui::Text("Camera position: %s\nEye vector: %s", .c_str(),
    //        glm::to_string().c_str());
    if (btns[7]) p->insertOne(origin, dir, 90);
    if (btns[0]) p->insertCUBEE();

    c->update();
}

void App::deactivate()
{
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
