#include <imgui.h>
#include <imgui_impl_glfw.h>
#define GL_SILENCE_DEPRECATION
#include "cmyvektor.hpp"
#include "ui.hpp"
#include <cmath>
#include <iostream>

/** Task 3: f(x) */
static auto funktion_f(CMyVektor<2> x) -> double;

/** Task 3: g(x) */
static auto funktion_g(CMyVektor<3> x) -> double;

auto main() -> int {

  /* Calculate results from tasks. */
  static constexpr CMyVektor<2> START_F{0.2, -2.1};
  const auto result_f = CMyVektor<2>::gradient_descent(START_F, funktion_f);
  std::cout << result_f << std::endl;

  static constexpr double INIT_STEP_SIZE_G = 0.1;
  static constexpr CMyVektor<3> START_G{0.0, 0.0, 0.0};
  const auto result_g =
      CMyVektor<3>::gradient_descent(START_G, funktion_g, INIT_STEP_SIZE_G);
  std::cout << result_g << std::endl;

  /* ======== NON-MANDATORY PART
   * =========================================================== */

  /* Make 2D visualization of funktion_f */
  static constexpr std::size_t RESOLUTION = 64;
  static constexpr double TICK_SIZE = 1.0 / 20.0;
  double values[RESOLUTION][RESOLUTION];
  for (std::size_t x = 0; x < RESOLUTION; x++) {
    for (std::size_t y = 0; y < RESOLUTION; y++) {
      const double x_coord = result_f[0] - (static_cast<double>(RESOLUTION) / 2.0 + static_cast<double>(x)) * TICK_SIZE;
      const double y_coord = result_f[1] - (static_cast<double>(RESOLUTION) / 2.0 + static_cast<double>(y)) * TICK_SIZE;
      values[x][y] =
          funktion_f(CMyVektor<2>({x_coord, y_coord}));
    }
  }

  /* Initialize Gui */
  auto gui = GuiHandle();

  const double start[2] = {
      result_f[0] - static_cast<double>(RESOLUTION) / 2.0 * TICK_SIZE,
      result_f[1] - static_cast<double>(RESOLUTION) / 2.0 * TICK_SIZE,
  };
  /* Update and run Gui */
  while (true) {
    if (gui.Update<RESOLUTION, TICK_SIZE>(values, start)) {
      return 0;
    }
  }
  /* =======================================================================================
   */
}

static auto funktion_f(CMyVektor<2> x) -> double {
  const auto &x_val = x[0];
  const auto &y_val = x[1];
  return std::sin(x_val * y_val) + std::sin(x_val) + std::cos(y_val);
}

static auto funktion_g(CMyVektor<3> x) -> double {
  const auto &x1 = x[0];
  const auto &x2 = x[1];
  const auto &x3 = x[2];
  return -(2.0 * std::pow(x1, 2) - 2.0 * x1 * x2 + std::pow(x2, 2) +
           std::pow(x3, 2) - 2.0 * x1 - 4.0 * x3);
}
