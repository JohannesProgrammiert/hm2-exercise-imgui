#ifndef UI_H_
#define UI_H_
/**
 * @file ui.hpp
 *
 * @brief User Interface (UI) frontend.
 *
 * Draws stuff to the screen and reacts to user input.
 * Uses the ImGui library with GLFW (OpenGL3) as graphics card interface.
 *
 * @author Johannes Schiffer
 * @date 03-05-2024
 */

#include "functions.hpp"
#include "iteration.hpp"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

/** User interface handle */
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
   * @returns 'true' if the user closed the UI window.
   */
  auto Update() -> bool;

private:
  /**
   * State of steepest-descent calculation.
   */
  enum class CalcState {
    /** Init-state: Customization of initialization value possible. */
    Init,
    /** Calculating: Customization of initialization value not possible. */
    MidCalculation,
    /** Calculation is done. Show result until the user clicks on 'reset'. */
    Done,
  };

  /** GLFW window handle. Initialized during object construction. */
  GLFWwindow *glfw_window{};

  /** Current state of finite state machine. */
  CalcState state{CalcState::Init};

  /** GLFW error callback. Not thread-safe. */
  static void glfw_error_callback(int error, const char *description);

  /** Start vector to start optimization from. */
  CMyVektor<2> start{0.2, -2.1};

  /** Iteration index to visualize. */
  uint8_t iteration{0};

  static constexpr double INIT_STEP_SIZE_F = 1.0;

  /** First gradient descent iteration with index zero. */
  IterationData<2> iteration_data_init{
      IterationData<2>::AtPoint(start, functions::f, INIT_STEP_SIZE_F, 0)};

  /** Heatmap subdivisions per dimension. */
  static constexpr std::size_t RESOLUTION = 64;

  /** Heatmap size in x- and y-direction. */
  static constexpr double HEATMAP_SIZE = 4.0;

  /** Axis tick step size. */
  static constexpr double TICK_SIZE =
      HEATMAP_SIZE / static_cast<double>(RESOLUTION);

  /** Start corner of heatmap. */
  static constexpr double START[2] = {-HEATMAP_SIZE / 2.0, -HEATMAP_SIZE / 2.0};

  /** Heatmap as C-style array in two dimensions. */
  double heatmap[RESOLUTION][RESOLUTION];

  /** Maximum value found in heatmap. */
  double heatmap_max{-INFINITY};

  /** Minimum value found in heatmap. */
  double heatmap_min{INFINITY};
};

#endif // UI_H_
