#include "main.h"
#include "Components/Camera.h"
#include "DebugUI.h"
#include "types.h"
#include "shadow/audio.h"
#include "Runtime.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <imgui_node_editor.h>
#include <imgui_internal.h>

#if BX_PLATFORM_LINUX
#	define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#	define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#	define GLFW_EXPOSE_NATIVE_COCOA
#endif

#define GLFW_INCLUDE_VULKAN
#include <bgfx/platform.h>
#include <GLFW/glfw3native.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
//#include <imgui_impl_vulkan.h>
#include <imgui_impl_bgfx.h>

/*#ifdef SHADOW_DEBUG_BUILD
#define IMGUI_VULKAN_DEBUG_REPORT
#endif*/

namespace ed = ax::NodeEditor;

static ed::EditorContext* g_Context = nullptr;

static bool s_showStats = false;
static bool s_showDebugger = false;
static bool s_showKbShortcuts = false;

static void glfw_errorCallback(int error, const char *description) {
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SLASH && action == GLFW_PRESS)
        s_showKbShortcuts = !s_showKbShortcuts;

    if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
        s_showStats = !s_showStats; // Cool trick to toggle a boolean

    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
        s_showDebugger = !s_showDebugger;
}

static void toggleStats() {
    s_showStats = !s_showStats;
}

static void toggleDebugger() {
    s_showDebugger = !s_showDebugger;
}

static const glm::vec2 SIZE = glm::vec2(1280, 720);

struct Vertex {
    glm::vec3 position;
    u32 color;
};

static Vertex cube_vertices[] = {
        { glm::vec3(-1.0f,  1.0f,  1.0f), 0xff000000 },
        { glm::vec3( 1.0f,  1.0f,  1.0f), 0xff0000ff },
        { glm::vec3(-1.0f, -1.0f,  1.0f), 0xff00ff00 },
        { glm::vec3( 1.0f, -1.0f,  1.0f), 0xff00ffff },
        { glm::vec3(-1.0f,  1.0f, -1.0f), 0xffff0000 },
        { glm::vec3( 1.0f, 1.0f, -1.0f), 0xffff00ff },
        { glm::vec3(-1.0f, -1.0f, -1.0f), 0xffffff00 },
        { glm::vec3( 1.0f, -1.0f, -1.0f), 0xffffffff },
};

static const u16 cube_indices[] = {
        0, 1, 2,
        1, 3, 2,
        4, 6, 5,
        5, 6, 7,
        0, 2, 4,
        4, 2, 6,
        1, 5, 3,
        5, 7, 3,
        0, 4, 1,
        4, 5, 1,
        2, 3, 6,
        6, 3, 7,
};

int Shadow::StartRuntime() {

    int width = 1280;
    int height = 720;

    glfwSetErrorCallback(glfw_errorCallback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(width, height, "Shadow Engine", nullptr, nullptr);

    IMGUI_CHECKVERSION();

    ShadowAudio::initAudioEngine();

    if (!window)
        return 1;

    glfwSetKeyCallback(window, glfw_keyCallback);

    bgfx::renderFrame();

    bgfx::Init init;

    init.platformData.ndt = glfwGetX11Display();
    init.platformData.nwh = (void*)(uintptr_t) glfwGetX11Window(window);

    glfwGetWindowSize(window, &width, &height);
    init.resolution.width = (uint32_t) width;
    init.resolution.height = (uint32_t) height;
    init.resolution.reset = BGFX_RESET_VSYNC;
    if (!bgfx::init(init))
        return 1;

    // Set view 0 to be the same dimensions as the window and to clear the color buffer
    const bgfx::ViewId kClearView = 0;
    bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
    bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

    // ImGui init
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Load DebugUI fonts
//    io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("./caskaydia-cove-nerd-font-mono.ttf", 16.0f);

    ImGui_Implbgfx_Init(255);
    ImGui_ImplGlfw_InitForVulkan(window, true);

    g_Context = ed::CreateEditor();

    // Init stuffs

    Shadow::Camera camera;

    //Mesh* m_mesh = meshLoad("desk.bin");

    int64_t m_timeOffset;
    bgfx::ProgramHandle m_program;
    bgfx::UniformHandle u_time = bgfx::createUniform("u_time", bgfx::UniformType::Vec4);


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Handle Window Resize
        int oldWidth = width;
        int oldHeight = height;
        glfwGetWindowSize(window, &width, &height);
        if (width != oldWidth || height != oldHeight) {
            bgfx::reset((uint32_t) width, (uint32_t) height, BGFX_RESET_VSYNC);
            bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
        }

	//TODO: ImGui should probably be disabled for production use
	//#ifndef SHADOW_DEBUG_BUILD

        // ImGui
        ImGui_Implbgfx_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        if (ImGui::GetIO().MouseClicked[1]) {
            ImGui::OpenPopup("test");
        }

        ImGui::Begin("Shadow Engine");
        //ImGui::Text("HELLO FROM SHADOW ENGINE");
        //ImGui::SmallButton("Test Button");

        if (ImGui::Button("Toggle Stats")) {
            toggleStats();
        }

        if (ImGui::Button("Toggle Debugger")) {
            toggleDebugger();
        }

        ImGui::End();

        ImGui::Begin("Shader Editor");

        ed::SetCurrentEditor(g_Context);
        ed::Begin("My Editor");

        int uniqueId = 1;

        ed::BeginNode(uniqueId++);
            ImGui::Text("This is node %i", uniqueId);
            ed::BeginPin(uniqueId++, ed::PinKind::Input);
                ImGui::Text("In");
            ed::EndPin();
            ImGui::SameLine();
            ed::BeginPin(uniqueId++, ed::PinKind::Output);
                ImGui::Text("Out");
            ed::EndPin();
        ed::EndNode();

        ed::End();
        ImGui::End();

        if (s_showDebugger) {
            ImGui::Begin("Shadow Engine Debugger");

            ImGui::Text("Debugging woo");

            if (ImGui::SmallButton("Close")) {
                //glfwDestroyWindow(window);
                break; // Better Way
            }

            ImGui::End();
        }

        if (s_showKbShortcuts) {
            ImGui::Begin("Keyboard Shortcuts");

            ImGui::Text("F1: Open Debugger");
            ImGui::Text("F3: Open Stats");

            ImGui::End();
        }

        ImGui::Render();

        ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

        // This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0
        bgfx::touch(kClearView);
        // Use debug font to print information about this example.
        bgfx::dbgTextClear();

        if (s_showDebugger) {
            bgfx::dbgTextPrintf(0, 0, 0x8f, "Debugger (Press F1 to close): ");
        } else {
            bgfx::dbgTextPrintf(3, 2, 0x01, "WARNING: DEBUG BUILD OF SHADOW ENGINE");
            bgfx::dbgTextPrintf(3, 3, 0x01, "NOT READY FOR PRODUCTION");
            bgfx::dbgTextPrintf(3, 4, 0x0f, "Press ? for help");
        }

#ifdef SHADOW_DEBUG_BUILD
        const bgfx::Stats *stats = bgfx::getStats();
        bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
#endif

        bgfx::frame();
    }
    
    Shadow::ShutdownRuntime();

    return 0;
}

void Shadow::ShutdownRuntime() {
    ShadowAudio::shutdownAudioEngine();
    bgfx::shutdown();
    ed::DestroyEditor(g_Context);
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    printf("Goodbye from Shadow Engine\n");
}
