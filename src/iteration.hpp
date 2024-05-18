#ifndef ITERATION_H_
#define ITERATION_H_
/**
 * @file iteration.hpp
 *
 * @brief Numeric optimization via gradient descent.
 *
 * The struct `IterationData` holds all necessary information of one iteration
 * of the gradient descent optimization method.
 *
 * It may be used to iteratively find
 * the maximum of a function.
 *
 * @author Johannes Schiffer
 * @date 03-05-2024
 */
#include "cmyvektor.hpp"
#include <cstddef>
template <std::size_t N> using FunctionPtr = double (*)(const CMyVektor<N> &x);

/**
 * A point of an N-dimensional function.
 *
 * @tparam N Dimension.
 */
template <std::size_t N> struct Point {

  /** Vector that defines the location in the vector field/preimage. */
  CMyVektor<N> vector{};

  /** Value calculated by an N-dimensional funktion. */
  double value{};

  /**
   * Constructor.
   *
   * @param vector Vector that defines the location in the vector
   * field/preimage.
   * @param funktion N-dimensional function that maps the vector to a value.
   */
  constexpr Point(CMyVektor<N> vector, FunctionPtr<N> funktion)
      : vector(vector), value(funktion(vector)){};

  /* default constructor */
  constexpr Point() = default;
};

/** Stream operator of 'Point'. */
template <std::size_t N>
std::ostream &operator<<(std::ostream &stream, const Point<N> &x) {
  stream << "Point{vector: " << x.vector << ", value: " << x.value << "}";
  return stream;
}

/**
 * Helper structure to collect and print iteration data.
 *
 * It is initialized using an N-dimensional point `current`, an
 * N-dimensional function `funktion`, and a step size `step_size`.
 *
 * `funktion(current)` will be calculated as well as the gradient at
 * position `current` (direction of steepest descent).
 *
 * The next point `next` is calculated by following the direction of
 * steepest descent in the vector field `funktion` and the test point `test`
 * is calculated doing the same thing at double step size.
 *
 * @tparam N Dimension of function pre-image.
 */
template <std::size_t N> struct IterationData {
  constexpr IterationData() = default;
  /**
   * Constructor to calculate gradient descent iteration data at optimization
   * point.
   *
   * @param current_point Current optimization point.
   * @param function Function to optimize.
   * @param step_size Current step size in numeric optimization.
   * @param iteration_index Iteration index. Should increment with each
   * successive iteration.
   */
  [[nodiscard]] static IterationData AtPoint(const CMyVektor<N> &current_point,
                               FunctionPtr<N> function, double step_size,
                               std::size_t iteration_index);

  /**
   * Alternative constructor to construct next iteration from the previous one.
   */
  [[nodiscard]] static IterationData Next(const IterationData &previous);

  /** Current iteration index. */
  std::size_t index;

  /** Gradient descent step size. */
  double step_size;

  /** Current optimization point. */
  Point<N> current{};

  /** Gradient at current point. */
  CMyVektor<N> current_grad{};

  /**
   * Next optimization point.
   *
   * Determined by following the current vector along the
   * gradient with step size.
   */
  Point<N> next{};

  /**
   * Test optimization point.
   *
   * Determined by following the current vector along the
   * gradient with double step size.
   */
  Point<N> test{};

  /** Returns 'true' if next iteration step size should be used or 'false'
   * if the current should be used. */
  [[nodiscard]] inline constexpr auto use_next() const -> bool {
    return next.value > current.value;
  }

  /** Returns 'true' if test iteration step size should be used or 'false'
   * if the current should be used. */
  [[nodiscard]] inline constexpr auto use_test() const -> bool {
    return use_next() && (test.value > next.value);
  }

  /** Maximum number of iteration steps. */
  static constexpr std::size_t MAX_ITERATIONS = 25;

  /** Minimum allowed gradient. Stop processing if the current gradient is
   * lower. */
  static constexpr double GRAD_LIMIT = 10.0e-5;

  /** Returns 'true' if the optimum has been found. It is then the 'current'
   * iteration. */
  [[nodiscard]] inline constexpr auto done() const -> bool {
    return index == MAX_ITERATIONS || current_grad.norm() < GRAD_LIMIT;
  }
  /* Move constructor. */
  constexpr IterationData(const IterationData &&other)
      : funktion(other.funktion), step_size(other.step_size),
        index(other.index), current(other.current),
        current_grad(other.current_grad), next(other.next), test(other.test) {}
  /* Move assignment operator. */
  IterationData<N> &operator=(IterationData<N> &&other);
  /* Copy constructor. */
  constexpr IterationData(const IterationData &other)
      : funktion(other.funktion), step_size(other.step_size),
        index(other.index), current(other.current),
        current_grad(other.current_grad), next(other.next), test(other.test) {}
  /* Copy assignment operator. */
  IterationData<N> &operator=(IterationData<N> &other);

private:
  /**
   * Associated, N-dimensional function that assigns an
   * unambiguous value to a given N-dimensional vector.
   */
  FunctionPtr<N> funktion;
};

template <std::size_t N>
IterationData<N> &IterationData<N>::operator=(IterationData<N> &&other) {
  this->funktion = other.funktion;
  this->step_size = other.step_size;
  this->index = other.index;
  this->current = other.current;
  this->current_grad = other.current_grad;
  this->next = other.next;
  this->test = other.test;
  return *this;
}
template <std::size_t N>
IterationData<N> &IterationData<N>::operator=(IterationData<N> &other) {
  this->funktion = other.funktion;
  this->step_size = other.step_size;
  this->index = other.index;
  this->current = other.current;
  this->current_grad = other.current_grad;
  this->next = other.next;
  this->test = other.test;
  return *this;
}
template <std::size_t N>
IterationData<N> IterationData<N>::AtPoint(const CMyVektor<N> &current_point,
                                           FunctionPtr<N> funktion,
                                           double step_size,
                                           std::size_t iteration_index) {
  IterationData<N> ret{}; /* Initialize return value. */
  ret.funktion = funktion;
  ret.step_size = step_size;
  ret.index = iteration_index;

  /* Initialize current point and its value. */
  ret.current = Point<N>(current_point, funktion);
  ret.current_grad = current_point.gradient(funktion);

  /* Initialize next point following the gradient and its value. */
  const auto next_point = current_point + step_size * ret.current_grad;
  ret.next = Point<N>(next_point, funktion);

  /* Initialize test point following the gradient with double step size and
   * its value. */
  const auto test_point = current_point + step_size * 2.0 * ret.current_grad;
  ret.test = Point<N>(test_point, funktion);
  return ret;
}

template <std::size_t N>
IterationData<N> IterationData<N>::Next(const IterationData &previous) {
  double next_step_size;
  CMyVektor<N> next_vector;
  /* For rules see exercise. First test next step size. If result is not
   * better make another test with duplicated step size. Use the best one.
   */
  if (previous.use_test()) {
    next_step_size = previous.step_size * 2.0;
    next_vector = previous.test.vector;
  } else if (previous.use_next()) {
    next_step_size = previous.step_size;
    next_vector = previous.next.vector;
  } else {
    /* retry with smaller step size. */
    next_step_size = previous.step_size / 2.0;
    next_vector = previous.current.vector;
  }

  /* initialize next iteration. */
  return AtPoint(next_vector, previous.funktion, next_step_size,
                 previous.index + 1);
}

template <std::size_t N>
std::ostream &operator<<(std::ostream &stream, const IterationData<N> &x) {
  stream << "Iteration " << x.index << "\n";
  stream << "\tx             " << x.current << "\n";
  stream << "\tlambda        " << x.step_size << "\n";
  stream << "\tgrad f(x)     " << x.current_grad << "\n";
  stream << "\t||grad f(x)|| " << x.current_grad.norm() << "\n";
  stream << "\tx_neu         " << x.next << "\n";
  stream << "\tx_test        " << x.test << "\n";
  return stream;
}

/** Task 3. Maximize `funktion` by numeric gradient descent. */
template <std::size_t N>
CMyVektor<N> gradient_descent(const CMyVektor<N> &start_point,
                              FunctionPtr<N> funktion,
                              double start_step_size = 1.0) {

  /* initialize current iteration data */
  auto iteration =
      IterationData<N>::AtPoint(start_point, funktion, start_step_size, 0);
  for (std::size_t _it = 0; _it < IterationData<N>::MAX_ITERATIONS; _it++) {
    std::cout << iteration << std::endl;
    if (iteration.done()) {
      return iteration.current.vector;
    }
    iteration = IterationData<N>::Next(iteration);
  }
  return iteration.current.vector;
}
#endif // ITERATION_H_
