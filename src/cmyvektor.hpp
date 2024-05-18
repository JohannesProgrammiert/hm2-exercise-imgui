#ifndef CMYVEKTOR_H_
#define CMYVEKTOR_H_
/**
 * @file cmyvektor.hpp
 *
 * @brief Templated CMyVektor struct with all features requested by the
 * exercise.
 *
 * There are three exercises. They cannot be uploaded because they are
 * intellectual property of the TH Aachen.
 *
 * @author Johannes Schiffer
 * @date 03-05-2024
 */
#include <array>
#include <cmath>
#include <iostream>

/* Forward declaration */
template <std::size_t N> class CMyVektor;

/**
 * Supported function pointer type.
 *
 * A function that assigns an unambiguous `double`-value to an
 * N-dimensional vector x.
 */
template <std::size_t N> using FunctionPtr = double (*)(const CMyVektor<N> &x);

/**
 * `std::array` of `double`s with some extra operations required for gradient
 * descent optimization.
 *
 * # Task 1
 *
 * - A vector of dimension n is constructed by calling the
 * std::array<double, N>() constructor, which is inherited using the 'using'
 * keyword. The template argument 'N' is the dimension. Example: `CMyVektor<2>
 * vec2 = {0.0, 0.0};`
 * - The dimension is requested by std::array<T>::size(), which is inherited.
 * Example: `vec2.size()`
 * - A component can be written by std::array<T>::at() or operator[] which are
 * inherited. Example: `vec2.at(0)`, `vec2[0]`.
 * - A component can be read the same way (at() or []).
 *
 * @tparam N Dimension of the vector.
 */
template <std::size_t N> struct CMyVektor : public std::array<double, N> {
  /** Task 2: Make gradient vector from input vector with function pointer. */
  [[nodiscard]] CMyVektor gradient(FunctionPtr<N> funktion) const;

  /** Euclidean norm of vector. */
  [[nodiscard]] double norm() const;

  /* Inherit []-operator from std::array */
  using std::array<double, N>::operator[];
};

/* NOTE: templated function must be implemented in header file instead of
 * source. */
/* ------------ IMPLEMENTATION ----------------------------------------- */
template <std::size_t N>
CMyVektor<N> CMyVektor<N>::gradient(FunctionPtr<N> funktion) const {
  /* h-value used in gradient calculation. */
  static constexpr double H = 10.0e-8;
  CMyVektor<N> ret;
  /* iterate target (gradient) elements */
  for (std::size_t i = 0; i < N; i++) {
    /* Need vector `x` with element at index i replaced by `x(i) + H`. */
    CMyVektor arg = *this;
    arg[i] += H;
    ret[i] = (funktion(arg) - funktion(*this)) / H;
  }
  return ret;
};

template <std::size_t N> double CMyVektor<N>::norm() const {
  double arg = 0.0;
  for (auto const &e : *this) {
    arg += std::pow(e, 2);
  }
  return std::sqrt(arg);
};

/* Implement operators. */
/** Scalar product */
template <std::size_t N> CMyVektor<N> operator*(double lambda, CMyVektor<N> a) {
  CMyVektor<N> ret;

  for (std::size_t i = 0; i < N; i++) {
    ret[i] = a[i] * lambda;
  }

  return ret;
}

/** Vector sum */
template <std::size_t N>
CMyVektor<N> operator+(CMyVektor<N> a, CMyVektor<N> b) {
  CMyVektor<N> ret;

  for (std::size_t i = 0; i < N; i++) {
    ret[i] = a[i] + b[i];
  }

  return ret;
}

/** Stream operator to print CMyVektor. */
template <std::size_t N>
std::ostream &operator<<(std::ostream &stream, const CMyVektor<N> &x) {
  stream << "CMyVektor{";
  for (const auto &e : x) {
    stream << e << ", ";
  }
  stream << "}";
  return stream;
}

#endif // CMYVEKTOR_H_
