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
    /* Init-state: Customization of initialization value possible. */
    Init,
    /* Mid-calculation: Customization not possible. */
    MidCalculation,
    /* Done: Customization not possible, calculation is done. */
    Done,
  };

  /** GLFW window handle. Initialized during object construction. */
  GLFWwindow *glfw_window{};

  CalcState state{CalcState::Init};

  /** GLFW error callback. Not thread-safe. */
  static void glfw_error_callback(int error, const char *description);
};

#endif // UI_H_
