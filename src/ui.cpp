#include "ui.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include <stdexcept>

void GuiHandle::glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

GuiHandle::GuiHandle() {
  glfwSetErrorCallback(glfw_error_callback);

  if (glfwInit() == GLFW_FALSE) {
    throw std::runtime_error("Could not initialize GLFW");
  }

  // GL 3.3 + GLSL 130
  static const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only

  // Create window with graphics context
  this->glfw_window = glfwCreateWindow(
      1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
  if (this->glfw_window == nullptr) {
    throw std::runtime_error("Could not create GLFW window");
  }
  glfwMakeContextCurrent(this->glfw_window);
  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(this->glfw_window, true);
  if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
    throw std::runtime_error("Could not initialize ImGui OpenGL backend");
  }
}

GuiHandle::~GuiHandle() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(this->glfw_window);
  glfwTerminate();
}
