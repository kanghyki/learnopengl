#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "context.hpp"

void FramebufferSizeCallbackFunc(GLFWwindow *window, int width, int height);
void OnKeyEvent(GLFWwindow *window, int key, int scancode, int action,
                int mods);
void OnCharEvent(GLFWwindow *window, unsigned int ch);
void OnScroll(GLFWwindow *window, double xoffset, double yoffset);
void OnCursorPos(GLFWwindow *window, double x, double y);
void OnMouseButton(GLFWwindow *window, int button, int action, int modifier);

int main(int argc, char **argv) {
  chdir("../../");
  SPDLOG_INFO("Initialize glfw");
  if (!glfwInit()) {
    const char *description = nullptr;
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
  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                        WINDOW_NAME, nullptr, nullptr);
  if (!window) {
    SPDLOG_ERROR("failed to create glfw window");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    SPDLOG_ERROR("failed to initialize GLAD");
    glfwTerminate();
    return -1;
  }

  auto imgui_ctx = ImGui::CreateContext();
  ImGui::SetCurrentContext(imgui_ctx);
  ImGui_ImplGlfw_InitForOpenGL(window, false);
  ImGui_ImplOpenGL3_Init();
  ImGui_ImplOpenGL3_CreateFontsTexture();
  ImGui_ImplOpenGL3_CreateDeviceObjects();

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallbackFunc);
  glfwSetKeyCallback(window, OnKeyEvent);
  glfwSetCharCallback(window, OnCharEvent);
  glfwSetCursorPosCallback(window, OnCursorPos);
  glfwSetMouseButtonCallback(window, OnMouseButton);
  glfwSetScrollCallback(window, OnScroll);

  auto context = Context::Create();
  if (!context) {
    SPDLOG_ERROR("failed to initialize context");
    glfwTerminate();
    return -1;
  }
  glfwSetWindowUserPointer(window, context.get());

  SPDLOG_INFO("Start render loop");
  while (!glfwWindowShouldClose(window)) {
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    context->Update();
    context->Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  context = nullptr;

  ImGui_ImplOpenGL3_DestroyFontsTexture();
  ImGui_ImplOpenGL3_DestroyDeviceObjects();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext(imgui_ctx);

  glfwTerminate();
  return 0;
}

void FramebufferSizeCallbackFunc(GLFWwindow *window, int width, int height) {
  SPDLOG_INFO("frame buffer size w:{}, h:{}", width, height);
  auto ptr = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
  ptr->ReshapeViewport(width, height);
}

void OnKeyEvent(GLFWwindow *window, int key, int scancode, int action,
                int mods) {
  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
  SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}", key, scancode,
              action == GLFW_PRESS     ? "Pressed"
              : action == GLFW_RELEASE ? "Released"
              : action == GLFW_REPEAT  ? "Repeat"
                                       : "Unknown",
              mods & GLFW_MOD_CONTROL ? "C" : "-",
              mods & GLFW_MOD_SHIFT ? "S" : "-",
              mods & GLFW_MOD_ALT ? "A" : "-");
  auto ptr = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
  ptr->ProcessKeyboardInput(window);
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void OnCharEvent(GLFWwindow *window, unsigned int ch) {
  ImGui_ImplGlfw_CharCallback(window, ch);
}

void OnScroll(GLFWwindow *window, double xoffset, double yoffset) {
  ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
  auto ptr = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
  ptr->ProcessMouseScroll(xoffset, yoffset);
}

void OnCursorPos(GLFWwindow *window, double x, double y) {
  auto ptr = reinterpret_cast<Context *>(glfwGetWindowUserPointer(window));
  ptr->ProcessMouseMove(x, y);
}

void OnMouseButton(GLFWwindow *window, int button, int action, int modifier) {
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);
  auto context = (Context *)glfwGetWindowUserPointer(window);
  double x, y;
  glfwGetCursorPos(window, &x, &y);
  context->ProcessMouseButton(button, action, x, y);
}