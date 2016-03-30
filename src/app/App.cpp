#include "App.hpp"

using namespace LOG;

GLFWwindow* App::pWindow = nullptr;
int App::windowWidth = 1920;
int App::windowHeight = 1080;
Scene App::scene;

App::App()
{
    ;
}
void App::_glfwErrHandler(int error, const char * description)
{
    fputs (description, stderr);
    writeLogErr ("%s\n", description);
}
bool App::startGL()
{
    writeLog ("starting GLFW %s", glfwGetVersionString ());

    glfwSetErrorCallback (_glfwErrHandler);
    if (!glfwInit ()) {
        fprintf (stderr, "ERROR: could not start GLFW3\n");
        return false;
    }
    // uncomment these lines if on Apple OS X
#ifdef __APPLE__
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    windowWidth=mode->width;
    windowHeight=mode->height;
    windowWidth=500;
    windowHeight = 500;

    //glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    //glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    //glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    //glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    //glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
        

    //pWindow = glfwCreateWindow( windowWidth,  windowHeight, "MySPH", glfwGetPrimaryMonitor(), nullptr);
    pWindow = glfwCreateWindow( windowWidth,  windowHeight, "MySPH", nullptr, nullptr);
    //pWindow = glfwCreateWindow(640, 480, "", NULL, NULL);
    //glfwSetWindowSize(pWindow, 2560, 1600);
    //pWindow = glfwCreateWindow( windowWidth,  windowHeight, "MySPH", nullptr, nullptr);
    if (!pWindow) {
        fprintf (stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent (pWindow);
    //glfwWindowHint (GLFW_SAMPLES, 4);
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit ();
    // get version info
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);
    writeLog ("renderer: %s\nversion: %s\n", renderer, version);
    return true;
}

void App::init()
{
    std::cout<<"Init APP\n";
    assert(restartLog());
    assert(startGL());
    InitImGui(windowWidth, windowHeight);

    int windowWidth, windowHeight;
    glfwGetFramebufferSize(pWindow, &windowWidth, &windowHeight);
    scene.init(windowWidth, windowHeight);

    /*--------------------Scene stuffs--------------------*/

    //Importer importer(&scene);
    //importer.import("./meshes/StartFile.dae");
    //importer.import("./meshes/Su-35_SuperFlanker/su-35.dae");
    //importer.import("./meshes/StartFile.dae");
    //importer.import("./meshes/mine.dae");
    //scene.addFluidSys("fluid_1");
    //Points* fluidSys = dynamic_cast<Points*>(scene.getObject("fluid_1"));
    //fluidSys->insertParsFromOBJ("./meshes/bunny.obj", 90.0, 1);
    //fluidSys->insertParsFromOBJ("./meshes/bunny.obj", 10.0, 0);
    //fluidSys->insertCUBE();
    //scene.addFluidSys("fluid_1");
#ifdef CUDA
    Object* fluid = new Points();
    dynamic_cast<Points*>(fluid)->init();
    fluid->setName("fluid_1");
    fluid->setParent(nullptr);

    //Init shader
    PointRendererTem *fluidTemp = new PointRendererTem(1920, 1200);
    Shader *shader = scene.getShaderManager().addShader("./shaders/pointTemp_vs.glsl", "./shaders/pointTemp_gs.glsl", "./shaders/pointTemp_fs.glsl", "TEMP_SHADER");
    scene.getRendererManager().addRenderer(fluidTemp, "TEMP_RENDERER");
    scene.getCamera()->bindToShader(shader);
    fluidTemp->setShader(shader);
    //Init renderer
    
    scene.addObject(fluid);
    scene.getRendererManager().assignObj2Renderer(fluid, "TEMP_RENDERER");
//dynamic_cast<Points*>(fluid)->insertParsFromOBJ("./meshes/bunny.obj", 90.0, 1);
    dynamic_cast<Points*>(fluid)->insertCUBEE();
#endif
    
    
    std::cout<<scene.getTreeView();
    glfwGetFramebufferSize(pWindow, &windowWidth, &windowHeight);
    scene.updateProjMat(windowWidth, windowHeight);
    /*--------Init lights-----------*/
    scene.getLightManager().addLight(glm::vec4(0.0, 0.0, 1.0, 0.0));
    scene.getLightManager().addLight(glm::vec4(0.0, 0.0, -1.0, 0.0));
    /*----------Init renderers-------*/
    /*---------Forward Renderer-------*/
    ForwardRenderer *fwRendererMesh = new ForwardRenderer;
    Shader* shdr = scene.getShaderManager().addShader("./shaders/mesh_vs.glsl", "./shaders/mesh_fs.glsl", "deformMeshShader");
    scene.getCamera()->bindToShader(shdr);
    scene.getLightManager().bindToShader(shdr);
    fwRendererMesh->setShader(shdr);
    scene.getRendererManager().addRenderer(fwRendererMesh, "FW_STATIC_MESH_R");
    /*-------Forward Renderer with LBS------*/
    ForwardRenderer *fwRendererLBS = new ForwardRenderer;
    shdr = scene.getShaderManager().addShader("./shaders/defMesh_vs.glsl", "./shaders/mesh_fs.glsl", "meshShader");
    scene.getLightManager().bindToShader(shdr);
    fwRendererLBS->setShader(shdr);
    scene.getCamera()->bindToShader(shdr);
    scene.getRendererManager().addRenderer(fwRendererLBS, "FW_LBS_MESH_R");

    /*-------Deferred Renderer-----------*/
    DeferredRenderer* dfRendererMesh = new DeferredRenderer(windowWidth, windowHeight);
    shdr = scene.getShaderManager().addShader("./shaders/deferredGeo_vs.glsl", "./shaders/deferredGeo_fs.glsl", "deffered");
    dfRendererMesh->setGeometryShader(shdr);
    scene.getCamera()->bindToShader(shdr);
    scene.getLightManager().bindToShader(shdr);
    scene.getRendererManager().addRenderer(dfRendererMesh, "DF_MESH_R");


    /*-----------------------------register callbacks-----------------*/
    glfwSetWindowSizeCallback (pWindow, _glfw_window_size_callback);
    glfwSetMouseButtonCallback(pWindow, mouseButton);
    glfwSetCursorPosCallback(pWindow,mouseMotion);
    glfwSetKeyCallback(pWindow, keyCallBack);
    glfwSetScrollCallback(pWindow, scrollCallback);

    PyConsole &console = PyConsole::getInstance();
    console.setScene(&scene);
    console.runConsole();
}
void App::_glfw_window_size_callback (GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight  = height;
    glfwGetFramebufferSize(pWindow, &windowWidth, &windowHeight);
    scene.updateProjMat(windowWidth, windowHeight);
}
void App::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        switch (action)
        {
            case GLFW_PRESS:
                double x, y;
                glfwGetCursorPos(pWindow,&x, &y);
                //start Draging
                ((Arcball*)(scene.getCamera()))->dragStart((int)x, (int)y);
                break;
            case GLFW_RELEASE:
                ((Arcball*)(scene.getCamera()))->dragEnd();
                break;
        }
    }
}
void App::mouseMotion(GLFWwindow *window, double x, double y)
{
    if (! ((Arcball*)(scene.getCamera()))->isDragged()) 
		return;
    ((Arcball*)(scene.getCamera()))->rotate(x, y);
    ((Arcball*)(scene.getCamera()))->dragUpdate(x, y);
	
}
void App::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //Points * p = (Points*)scene.getObject("fluid_1");
    if (action == GLFW_PRESS)
    {
        switch(key)
        {
            //case GLFW_KEY_T:
            //    p->insertCUBE(glm::vec3(7,0,0), glm::vec3(0,0,0));break;
            case GLFW_KEY_ESCAPE:   //exit
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            default:
                break;
        }
    }
}
void App::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    scene.getCamera()->addTranslation(glm::vec3(0.0, 0.0, yoffset*0.5));
}
void App::run()
{
    glClearColor(0.5, 0.5, 0.5, 1.0);
    int width, height;
    while (!glfwWindowShouldClose (pWindow))
    {
        PyConsole::getInstance().callFunctions();
        UpdateImGui(pWindow);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwGetFramebufferSize(pWindow, &width, &height);
        glViewport(0, 0, width, height);

        ImGui::Text("Frambuffer size %d, %d", width, height);

        // Calculate and show frame rate
        static float ms_per_frame[120] = { 0 };
        static int ms_per_frame_idx = 0;
        static float ms_per_frame_accum = 0.0f;
        ms_per_frame_accum -= ms_per_frame[ms_per_frame_idx];
        ms_per_frame[ms_per_frame_idx] = ImGui::GetIO().DeltaTime * 1000.0f;
        ms_per_frame_accum += ms_per_frame[ms_per_frame_idx];
        ms_per_frame_idx = (ms_per_frame_idx + 1) % 120;
        const float ms_per_frame_avg = ms_per_frame_accum / 120;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", ms_per_frame_avg, 1000.0f / ms_per_frame_avg);

        scene.drawScene();
        //Render GUI
        glDisable(GL_BLEND);
        ImGui::Render();

        glfwSwapInterval(0);
        glfwSwapBuffers(pWindow);
        glfwPollEvents();
    }
}
void App::deactivate()
{
    glfwTerminate();
}

