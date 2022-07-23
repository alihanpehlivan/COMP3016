#include "StdAfx.h"

// Linking External Libraries
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32_x64_Static.lib") // #define GLEW_STATIC

#ifdef _DEBUG
#pragma comment(lib, "glfw3_Debug_x64.lib")
#pragma comment(lib, "fmt_Debug_x64.lib")
#pragma comment(lib, "spdlog_Debug_x64.lib")
#else
#pragma comment(lib, "glfw3_Release_x64.lib")
#pragma comment(lib, "fmt_Release_x64.lib")
#pragma comment(lib, "spdlog_Release_x64.lib")
#endif

// GLFW Callback Definitions
void __GLFW_FrameBufferSizeCallBack(GLFWwindow* window, int width, int height);
void __GLFW_ProcessInputCallBack(GLFWwindow* window);
void __GLFW_ErrorCallBack(int error, const char* description);

// settings
const unsigned int VP_WIDTH = 2048;
const unsigned int VP_HEIGHT = 1080;

bool show_raincode = false;

///////////
ImVec4 clear_color = ImVec4(25 / 255.0f, 25 / 255.0f, 25 / 255.0f, 1.00f);

static void __ImGuiRender()
{
    // Create the frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static float f = 0.0f;
    static int counter = 0;

    // Beginning of the window
    ImGui::Begin("Font Render Settings");
    
    // ---- Handle our TextManager's TextInstances
    TextManager::Instance().ImGuiProcess();

    // ----
    ImGui::Spacing();
    ImGui::Checkbox("Show RainCode Demo", &show_raincode);

    // ----
    ImGui::Separator();
    ImGui::Text("Background Settings:");
    ImGui::ColorEdit3("Color", (float*)&clear_color); // Edit 3 floats representing a color
    
    // ----
    ImGui::Separator();
    ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // End of the frame
    ImGui::End();

    // Prepare RenderDrawData
    ImGui::Render();

    // Present RenderData
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Windows Entry Point
int WINAPI WinMain(HINSTANCE hInstance,		// HANDLE TO AN INSTANCE.  This is the "handle" to YOUR PROGRAM ITSELF.
	HINSTANCE hPrevInstance,				// USELESS on modern windows (totally ignore hPrevInstance)
	LPSTR szCmdLine,						// Command line arguments.  similar to argv in standard C programs
	int iCmdShow)							// Start window maximized, minimized, etc.
{
    // Setup or custom logger
    //-----------------------
    if (!Log::Init("FontRenderer"))
        return 1;

    // Init glfw
    //----------
    glfwSetErrorCallback(__GLFW_ErrorCallBack);

    if (!glfwInit())
    {
        LOG_ERROR("can not initialize glfw!");
        return 1;
    }

    // Decide GL+GLSL versions
    //------------------------
    const char* glsl_version = "#version 330 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create window with graphics context
    //------------------------------------
    GLFWwindow* window = glfwCreateWindow(VP_WIDTH, VP_HEIGHT, "Custom Font Renderer Example", NULL, NULL);
    if (window == NULL)
    {
        LOG_ERROR("can not create glfw window.");
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, __GLFW_FrameBufferSizeCallBack);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    //-------------------------
    bool err = glewInit() != GLEW_OK;
    if (err)
    {
        LOG_ERROR("failed to initialize OpenGL loader!");
        return 1;
    }

    // Setup ImGui context
    //--------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup ImGui style
    //------------------
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    //---------------------------------
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
    {
        LOG_ERROR("failed to initialize glfw OpenGL for ImGui");
        return 1;
    }

    if (!ImGui_ImplOpenGL3_Init(glsl_version))
    {
        LOG_ERROR("failed to initialize OpenGL for ImGui");
        return 1;
    }

    // Initialize Text Manager (and Launch FONS context too)
    //------------------------------------------------------
    if (!TextManager::Instance().Initialize(VP_WIDTH, VP_HEIGHT))
    {
        LOG_ERROR("failed to initialize TextManager (font texture atlas)");
        return 1;
    }

    TextManager::Instance().AddFont("DroidSerif-Regular.ttf");
    TextManager::Instance().AddFont("emoticons.otf");
    TextManager::Instance().AddFont("NotoSansJP-Regular.otf");

    // Demo Fonts
    float fXPos = 720;
    float fMid = VP_HEIGHT / 2 - 50.0f;

    TextManager::Instance().CreateTextLine("The Quick Brown Fox…", fXPos, fMid, 190.0f, encodeRGBA(1, 255, 1, 255), 0.0f, 0.0f);
    TextManager::Instance().CreateTextLine("Jived fox nymph grabs quick waltz.", fXPos, fMid+50.0f, 50.0f, encodeRGBA(255, 127, 0, 255), 0.0f, 15.0f);
    TextManager::Instance().CreateTextLine("The five boxing wizards jump quickly.", fXPos, fMid+100.0f, 50.0f, encodeRGBA(0, 255, 164, 255), 0.0f, 0.0f);
    TextManager::Instance().CreateTextLine("Jackdaws love my big sphinx of quartz.", fXPos, fMid+150.0f, 50.0f, encodeRGBA(255, 1, 1, 255), 0.0f, 0.0f);
    TextManager::Instance().CreateTextLine("Pack my box with five dozen liquor jugs.", fXPos, fMid+200.0f, 50.0f, encodeRGBA(255, 0, 255, 255), 0.0f, 5.0f);
    TextManager::Instance().CreateTextLine("How vexingly quick daft zebras jump!", fXPos, fMid+350.0f, 158.0f, encodeRGBA(255, 1, 1, 255), 5.0f, 0.0f);

    // Define the viewport dimensions
    glViewport(0, 0, VP_WIDTH, VP_HEIGHT);

    // Enable Blending
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Transparent
    //glDisable(GL_DEPTH_TEST); // Disable depth test

    // Begin RENDERING!
    //----------
    while (!glfwWindowShouldClose(window))
    {
        // 1. Poll and handle events (inputs, window resize, etc.)
        //--------------------------------------------------------
        glfwPollEvents();

        // 2. Handle Window Input Callbacks (esc, mouse, etc.)
        //----------------------------------------------------
        __GLFW_ProcessInputCallBack(window);

        // 3. Clear the screen buffer and set the background color
        //--------------------------------------------------------
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Transparent

        // 4. First render our TextLines, will be underneath of ImGui
        //-----------------------------------------------------------
        TextManager::Instance().Update(); // Preferably call this function somewhere else!
        TextManager::Instance().Render();

        if (show_raincode)
            TextManager::Instance().RenderRainCode(1.0f / 60.0f);

        // 5. Now Render ImGui on top of the TextLines
        //--------------------------------------------
        __ImGuiRender();

        // 6. Swap GL buffers
        //-------------------
        glfwSwapBuffers(window);
    }

    // Shutdown ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Shutdown TextManager
    TextManager::Instance().Destroy();

    // Terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// glfw: whenever error occures this callback function execute
// -----------------------------------------------------------
void __GLFW_ErrorCallBack(int error, const char* description)
{
	LOG_ERROR("GLFW error occured! errcode: {} details: {}", error, description);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void __GLFW_ProcessInputCallBack(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void __GLFW_FrameBufferSizeCallBack(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}