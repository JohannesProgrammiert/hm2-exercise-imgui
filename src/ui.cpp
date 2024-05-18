/**
 * @file ui.cpp
 *
 * @brief Implementation of UI-related stuff.
 *
 * @author Johannes Schiffer
 * @date 03-05-2024
 */
#include "ui.hpp"
#include "cmyvektor.hpp"
#include "functions.hpp"
#include "imgui.h"
#include "iteration.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include <sstream>
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

  /* Populate heatmap with 2D function values. */
  for (std::size_t y = 0; y < RESOLUTION; y++) {
    for (std::size_t x = 0; x < RESOLUTION; x++) {
      const double x_coord = START[0] + static_cast<double>(x) * TICK_SIZE;
      const double y_coord = START[1] + static_cast<double>(y) * TICK_SIZE;
      /* No idea why but y must be inverted. For the heatmap to match the
       * scatter plot. */
      heatmap[y][x] = functions::f(CMyVektor<2>({x_coord, -y_coord}));
    }
  }
  /* Find maximum and minimum value in heatmap. Used as boundaries for color
   * mapping. */
  for (std::size_t i = 0; i < RESOLUTION * RESOLUTION; i++) {
    /* A bit weird but we know 2-dimensional arrays are in contiguous memory. We
     * can access all elements by the second index only. */
    const double &current = heatmap[0][i];
    if (current > heatmap_max) {
      heatmap_max = current;
    }
    if (current < heatmap_min) {
      heatmap_min = current;
    }
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

auto GuiHandle::Update() -> bool {
  /* Poll and handle events (inputs, window resize, etc.) */
  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  /* Finite state machine.
   *
   * Actually, this could be done cleaner by moving all code into the
   * switch-case statement.
   */
  CalcState next_state = this->state;

  switch (this->state) {
  case CalcState::Init:
    if (ImGui::Button("Start Calculation")) {
      next_state = CalcState::MidCalculation;
    }
    break;
  case CalcState::MidCalculation:
    ImGui::BeginDisabled();
    if (ImGui::Button("Start Calculation")) {
      /* nothing */
    }
    ImGui::EndDisabled();
    break;
  case CalcState::Done:
    if (ImGui::Button("Reset")) {
      next_state = CalcState::Init;
    }
    break;
  }

  if (this->state != CalcState::Init) {
    ImGui::BeginDisabled();
  }

  ImGui::DragScalar("Start x", ImGuiDataType_Double, &start[0], 0.1);
  ImGui::DragScalar("Start y", ImGuiDataType_Double, &start[1], 0.1);

  if (this->state != CalcState::Init) {
    ImGui::EndDisabled();
  }

  if (this->state == CalcState::Init) {
    iteration = 0;
  }

  if (this->state != CalcState::MidCalculation) {
    ImGui::BeginDisabled();
  }

  static constexpr uint8_t IT_MIN = 0;
  static constexpr uint8_t IT_MAX = 24;
  ImGui::SliderScalar("Iteration step", ImGuiDataType_U8, &iteration, &IT_MIN,
                      &IT_MAX);

  if (this->state != CalcState::MidCalculation) {
    ImGui::EndDisabled();
  }

  if (this->state == CalcState::Init) {
    iteration_data_init =
        IterationData<2>::AtPoint(start, functions::f, INIT_STEP_SIZE_F, 0);
  }

  IterationData<2> iteration_data = iteration_data_init;
  if (this->state == CalcState::MidCalculation) {
    for (std::size_t i = 0; i < iteration; i++) {
      if (iteration_data.done()) {
        next_state = CalcState::Done;
        break;
      }
      iteration_data = IterationData<2>::Next(iteration_data);
    }
  }

  if (this->state == CalcState::MidCalculation ||
      this->state == CalcState::Done) {
    std::stringstream ss;
    ss << iteration_data;
    std::string str = ss.str();
    ImGui::Text("%s", str.c_str());
  }

  /* -- Make 2D visualization of functions::f -- */

  /* Populate plot points as C array types. */
  const double opt_x[1] = {iteration_data.current.vector[0]};
  const double opt_y[1] = {iteration_data.current.vector[1]};
  const double next_x[1] = {iteration_data.next.vector[0]};
  const double next_y[1] = {iteration_data.next.vector[1]};
  const double test_x[1] = {iteration_data.test.vector[0]};
  const double test_y[1] = {iteration_data.test.vector[1]};

  ImPlot::PushColormap(ImPlotColormap_Viridis);
  if (ImPlot::BeginPlot("Heatmap")) {
    ImPlot::PlotHeatmap(
        "f(x)", heatmap[0], RESOLUTION, RESOLUTION, heatmap_min, heatmap_max,
        "", ImPlotPoint(START[0], START[1]),
        ImPlotPoint(START[0] + HEATMAP_SIZE, START[1] + HEATMAP_SIZE),
        ImPlotHeatmapFlags_None);
    ImPlot::PlotScatter("Optimum", opt_x, opt_y, 1);
    ImPlot::PlotScatter("Next point", next_x, next_y, 1);
    ImPlot::PlotScatter("Test point", test_x, test_y, 1);
    ImPlot::EndPlot();
  }

  ImGui::Render();
  int display_w, display_h;
  glfwGetFramebufferSize(this->glfw_window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);

  static constexpr ImVec4 CLEAR_COLOR = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  glClearColor(CLEAR_COLOR.x * CLEAR_COLOR.w, CLEAR_COLOR.y * CLEAR_COLOR.w,
               CLEAR_COLOR.z * CLEAR_COLOR.w, CLEAR_COLOR.w);

  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(this->glfw_window);
  this->state = next_state;
  return glfwWindowShouldClose(this->glfw_window);
}
