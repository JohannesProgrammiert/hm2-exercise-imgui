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
 * @tparam N Dimension of the fixed-size vector.
 */
template <std::size_t N> class CMyVektor : public std::array<double, N> {
private:

  /** Task 2: Make gradient vector from input vector with function pointer.
   *
   * @note Implemented as member function because it is the same.
   */
  auto gradient(double (*funktion)(CMyVektor x)) const -> CMyVektor {
    static constexpr double H = 10.0e-8;
    CMyVektor<N> ret;
    /* iterate target (gradient) elements */
    for (std::size_t i = 0; i < N; i++) {
      /* The first `funktion`-call needs vector `x` with
       * element at index i replaced by `x(i) + H`.
       */
      CMyVektor arg = *this;
      arg[i] += H;
      ret[i] = (funktion(arg) - funktion(*this)) / H;
    }
    return ret;
  };

  /** Calculate euclidean norm of vector. */
  auto norm() const -> double {
    double arg = 0.0;
    for (auto const &e : *this) {
      arg += std::pow(e, 2);
    }
    return std::sqrt(arg);
  };

  /**
   * Helper structure to collect and print iteration data.
   *
   * It is initialized using an N-dimensional point `current`, an N-dimensional
   * function `funktion`, and a step size `step_size`.
   *
   * `funktion(current)` will be calculated as well as the gradient at
   * position `current` (direction of steepest descent).
   *
   * The next point `next` is calculated by following the direction of
   * steepest descent in the vector field `funktion` and the test point `test`
   * is calculated doing the same thing at double step size.
   */
  struct IterationData {
  private:
    /**
     * Associated vector field. An N-dimensional function that assigns an
     * unambiguous value to a given N-dimensional vector.
     */
    double (*funktion)(CMyVektor x);

  public:
    /** Current iteration index. */
    std::size_t index;

    /** Gradient descent step size. */
    double step_size;

    /**
     * Current optimization point.
     *
     * @note First value is vector, second value is function result of vector.
     */
    std::pair<CMyVektor, double> current;

    /** Gradient at current point. */
    CMyVektor current_grad;

    /**
     * Next optimization point.
     *
     * Determined by following the current vector along the
     * gradient with step size.
     *
     * @note First value is vector, second value is function result of vector.
     */
    std::pair<CMyVektor, double> next;

    /**
     * Test optimization point.
     *
     * Determined by following the current vector along the
     * gradient with double step size.
     *
     * @note First value is vector, second value is function result of vector.
     */
    std::pair<CMyVektor, double> test;

    IterationData(const CMyVektor &current_point,
                  double (*funktion)(CMyVektor x), double step_size,
                  std::size_t iteration_index)
        : funktion(funktion), index(iteration_index), step_size(step_size) {

      /* Initialize current point and its value. */
      current = {current_point, funktion(current_point)};
      current_grad = current_point.gradient(funktion);

      /* Initialize next point following the gradient and its value. */
      const auto next_point = current_point + step_size * current_grad;
      next = {next_point, funktion(next_point)};

      /* Initialize test point following the gradient with double step size and
       * its value. */
      const auto test_point = current_point + step_size * 2.0 * current_grad;
      test = {test_point, funktion(test_point)};
    }

    /** Returns 'true' if next iteration step size should be used or 'false' if
     * the current should be used. */
    constexpr auto use_next() const -> bool {
      return next.second <= current.second;
    }

    /** Returns 'true' if test iteration step size should be used or 'false' if
     * the current should be used. */
    constexpr auto use_test() const -> bool {
      return test.second > next.second;
    }

    friend std::ostream &operator<<(std::ostream &stream,
                                    const IterationData &x) {
      stream << "Iteration " << x.index << "\n";
      stream << "\tx             " << x.current.first << "\n";
      stream << "\tlambda        " << x.step_size << "\n";
      stream << "\tf(x)          " << x.current.second << "\n";
      stream << "\tgrad f(x)     " << x.current_grad << "\n";
      stream << "\t||grad f(x)|| " << x.current_grad.norm() << "\n";
      stream << "\tx_neu         " << x.next.first << "\n";
      stream << "\tf(x_neu)      " << x.next.second << "\n\n";
      /* TODO: this is printed everytime, not just when it is used. */
      stream << "Test mit doppelter Schrittweite\n";
      stream << "\tx_test        " << x.test.first << "\n";
      stream << "\tf(x_test)     " << x.test.second << "\n\n";
      return stream;
    }
  };

public:
  /* Inherit all constructors of 'std::vector' */
  using std::array<double, N>::operator[];

  friend std::ostream &operator<<(std::ostream &stream, const CMyVektor<N> &x) {
    stream << "CMyVektor{";
    for (const auto &e : x) {
      stream << e << ", ";
    }
    stream << "}";
    return stream;
  }

  /** Task 3. Recusively maximize `funktion` by gradient descent.
   *
   * @note Implemented in header file because it is templated.
   */
  static auto gradient_descent(const CMyVektor &current,
                               double (*funktion)(CMyVektor x),
                               double step_size = 1.0,
                               std::size_t iteration_index = 0) -> CMyVektor {

    /* Maximum recursion depth. */
    static constexpr std::size_t MAX_ITERATIONS = 25;

    /* Minimum allowed gradient. Stop processing if the current gradient is
     * lower. */
    static constexpr double GRAD_LIMIT = 10.0e-5;

    /* initialize current iteration data */
    const IterationData iteration = {current, funktion, step_size,
                                     iteration_index};

    /* stop processing if max recursion depth reached */
    if (iteration.index == MAX_ITERATIONS) {
      return iteration.current.first;
    }

    /* stop processing if gradient is smaller than limit. */
    if (iteration.current_grad.norm() < GRAD_LIMIT) {
      return iteration.current.first;
    }

    /* For rules see exersize. First test next step size. If result is not
     * better make another test with duplicated step size. Use the best one. */
    if (!iteration.use_next()) {
      if (iteration.use_test()) {
        step_size *= 2.0;
      }
    }

    std::cout << iteration << std::endl;

    /* Make result of optimum. */
    CMyVektor result;
    if (iteration.use_next()) {
      result = gradient_descent(iteration.current.first, funktion,
                                step_size / 2.0, iteration.index + 1);
    } else {
      result = gradient_descent(iteration.next.first, funktion, step_size,
                                iteration.index + 1);
    }
    return result;
  }
};

/* Scalar product */
template <std::size_t N> CMyVektor<N> operator*(double lambda, CMyVektor<N> a) {
  CMyVektor<N> ret;

  for (std::size_t i = 0; i < N; i++) {
    ret[i] = a[i] * lambda;
  }

  return ret;
}

/* Vector sum */
template <std::size_t N>
CMyVektor<N> operator+(CMyVektor<N> a, CMyVektor<N> b) {
  CMyVektor<N> ret;

  for (std::size_t i = 0; i < N; i++) {
    ret[i] = a[i] + b[i];
  }

  return ret;
}
#endif // CMYVEKTOR_H_
