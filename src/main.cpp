#include <imgui.h>
#include <imgui_impl_glfw.h>
#define GL_SILENCE_DEPRECATION
#include "functions.hpp"
#include "iteration.hpp"
#include "ui.hpp"
#include <iostream>

auto main() -> int {

  /* Calculate results from tasks. */
  static constexpr CMyVektor<2> START_F{0.2, -2.1};
  const CMyVektor<2> result_f = gradient_descent<2>(START_F, functions::f);
  std::cout << result_f << std::endl;

  static constexpr double INIT_STEP_SIZE_G = 0.1;
  static constexpr CMyVektor<3> START_G{0.0, 0.0, 0.0};
  const CMyVektor<3> result_g =
      gradient_descent<3>(START_G, functions::g, INIT_STEP_SIZE_G);
  std::cout << result_g << std::endl;

  /* ======== NON-MANDATORY PART
   * =========================================================== */

  /* Initialize Gui */
  auto gui = GuiHandle();

  /* Update and run Gui */
  while (true) {
    if (gui.Update()) {
      return 0;
    }
  }
  /* =======================================================================================
   */
}
