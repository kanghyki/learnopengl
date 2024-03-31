#include "Context.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void framebufferSizeCallbackFunc(GLFWwindow* window, int width, int height)
{
    SPDLOG_INFO("frame buffer size w:{}, h:{}", width, height);
    auto ptr = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    ptr->reshape(width, height);
}

void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
        key, scancode,
        action == GLFW_PRESS ? "Pressed" :
        action == GLFW_RELEASE ? "Released" :
        action == GLFW_REPEAT ? "Repeat" : "Unknown",
        mods & GLFW_MOD_CONTROL ? "C" : "-",
        mods & GLFW_MOD_SHIFT ? "S" : "-",
        mods & GLFW_MOD_ALT ? "A" : "-");
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void onCharEvent(GLFWwindow* window, unsigned int ch)
{
    ImGui_ImplGlfw_CharCallback(window, ch);
}

void onScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void onCursorPos(GLFWwindow* window, double x, double y)
{
    auto ptr = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    ptr->processMouseMove(x, y);
}

void onMouseButton(GLFWwindow* window, int button, int action, int modifier)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);
    auto context = (Context *)glfwGetWindowUserPointer(window);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    context->processMouseButton(button, action, x, y);
}

int main(int argc, char** argv)
{
    SPDLOG_INFO("Initialize glfw");
    if (!glfwInit())
    {
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef __OSX__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    SPDLOG_INFO("Create glfw window");
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window)
    {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        SPDLOG_ERROR("failed to initialize GLAD");
        glfwTerminate();
        return -1;
    }

    auto imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
#ifndef __OSX__
    glfwGetFramebufferSize(window, &width, &height);
#endif
    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallbackFunc);
    glfwSetKeyCallback(window, onKeyEvent);
    glfwSetCharCallback(window, onCharEvent);
    glfwSetCursorPosCallback(window, onCursorPos);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetScrollCallback(window, onScroll);

    auto context = Context::create();
    if (!context)
    {
        SPDLOG_ERROR("failed to initialize context");
        glfwTerminate();
        return -1;
    }
    glfwSetWindowUserPointer(window, context.get());

    SPDLOG_INFO("Start render loop");
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        context->processKeyboardInput(window);
        context->render();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    context = nullptr;

    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);

    glfwTerminate();
    return 0;
}
