#pragma once
#include <limits>

/** find a ratio of integers which best matches a floating point number.
 * This is useful for tuning a PWM.
 *
 * It sometimes gives something that isn't the absolute best due to numerical error in the 1/double computation.
 * One can extend it with something that computes the estimate on each pass and records the best as the iteration seeks it. That will nearly double the computational burden so we won't do that in this class.
 * Another variation would be a class for trying to reduce a ratio of integers to fit into a smaller integer representation.
 *
 * Note: during development the ratio was easily inverted by minor changes to the initialization, which leans a tad illegitimately on the iterative step to save on some code.
 * The terse variable names match the wikipedia article this math was extracted from.
 */
class ContinuedFractionRatioGenerator {
public:
  double fraction;
  unsigned an;
  //built by recursion relationship, where each step depends on prior 2, so we need a memory that is 3 deep.
  unsigned h[3];
  unsigned k[3];
  /** how large the numerator or denominator may be.
   * The minimal constraint is to not overflow the computer representation.
   * This is usually a power of 2.
   */
  unsigned limit;
  constexpr unsigned maxWorkingBits = std::numeric_limits<decltype(h[0])>::digits;

  /** create one for learning the algorithm, then call restart followed by looping on step inspecting numerator and denominator as you go. */
  ContinuedFractionRatioGenerator();

  /** create and iterate. By the time this returns it holds the best ratio. */
  ContinuedFractionRatioGenerator(double ratio, unsigned limit = 0) {
    restart(ratio, limit);
    best();
  }

  /** numerator and denominator will be <limit when iteration stops. for hardware use the limit value should be ((1<<numbits)-1)
   * @param ratio is expected to be positive, if not hilarity ensues. @returns an OK to iterate which is not yet sane, ignore it. */
  bool restart(double ratio, unsigned limit = ~0U);

  /** @returns whether the number format allows a more precise computation, if so then computation is done and has been stored internally.*/
  bool step();

  /** @returns a number greater than denominator if input value is greater than 1 */
  unsigned numerator() {
    return k[1];
  }

  /** @returns a number greater than numerator if input value is less than 1 */
  unsigned denominator() {
    return h[1];
  }

  /** best approximation, used for testing the algorithm, the desired output is  */
  double approximation();

  void run();

  /** calls steps() until it says to stop then @returns approximation() */
  double best();

private:
  bool split();

  bool bump(unsigned hk[]);
};
