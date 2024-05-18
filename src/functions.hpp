#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
/**
 * @file functions.hpp
 *
 * @brief Math function definitions from exercises.
 *
 * Each function maps an N-dimensional vector to a 1-dimensional value.
 *
 * @author Johannes Schiffer
 * @date 03-05-2024
 */
#include "cmyvektor.hpp"
#include <cmath>

namespace functions {
/** Task 3: f(x) */
static inline auto f(const CMyVektor<2> &x) -> double {
  const auto &x_val = x[0];
  const auto &y_val = x[1];
  return std::sin(x_val * y_val) + std::sin(x_val) + std::cos(y_val);
}

/** Task 3: g(x) */
static inline auto g(const CMyVektor<3> &x) -> double {
  const auto &x1 = x[0];
  const auto &x2 = x[1];
  const auto &x3 = x[2];
  return -(2.0 * std::pow(x1, 2) - 2.0 * x1 * x2 + std::pow(x2, 2) +
           std::pow(x3, 2) - 2.0 * x1 - 4.0 * x3);
}
} // namespace functions

#endif // FUNCTIONS_H_
