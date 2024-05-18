#ifndef UI_H_
#define UI_H_
/**
 * @file ui.hpp
 *
 * @brief User Interface (UI) frontend.
 *
 * Draws stuff to the screen and reacts to user input.
 * Uses the ImGui library with GLFW (OpenGL3) as graphics card interface.
 */

#include <GLFW/glfw3.h>
#include <cmath>
#include <cstddef>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

/** Handle user interface handle */
class GuiHandle {
public:
  /** Handle constructor. Will initialize the UI and throw exceptions on
   * failure. */
  GuiHandle();

  /** Handle deconstructor used to clean up on program exit. */
  ~GuiHandle();

  /**
   * Draw one frame of user interface and process input events.
   *
   * @returns 'true' if the user closed the UI window. */
  template <std::size_t RES> auto Update(const double heatmap[RES][RES]) -> bool {
    // Poll and handle events (inputs, window resize, etc.)
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static constexpr int Y_DATA[4] = {1, 5, 6, 22};
    double max = -INFINITY;
    double min = INFINITY;
    for (std::size_t i = 0; i < RES * RES; i++) {
      if (heatmap[0][i] > max) {
        max = heatmap[0][i];
      }
      if (heatmap[0][i] < min) {
        min = heatmap[0][i];
      }
    }

    ImPlot::PushColormap(ImPlotColormap_Viridis);
    if (ImPlot::BeginPlot("Test")) {
      // ImPlot::PlotLine("Line", Y_DATA, 4);
      ImPlot::PlotHeatmap("heat", heatmap[0], RES, RES, min, max, "",
                          ImPlotPoint(0, 0), ImPlotPoint(1, 1),
                          ImPlotHeatmapFlags_None);
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

    return glfwWindowShouldClose(this->glfw_window);
  }

private:
  /** GLFW window handle. Initialized during object construction. */
  GLFWwindow *glfw_window{};

  /** GLFW error callback. Not thread-safe. */
  static void glfw_error_callback(int error, const char *description);
};

#endif // UI_H_
