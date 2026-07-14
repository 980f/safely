#pragma once


#include <cstdint>
#ifndef __cpp_lib_bit_cast // C++ >= 20
/// copied and specialized and compacted from recent <bit>
constexpr double bit_cast(unsigned long __from) noexcept {
  return __builtin_bit_cast(double, __from); }
#else
#include <bit>
#endif // __cpp_lib_bit_cast

#include <limits>  //bits in the working type

/** find a ratio of integers which best matches a floating point number.
 * This is useful for tuning a PWM.
 *
 * It sometimes gives something that isn't the absolute best due to numerical error in the 1/double computation.
 * One can extend it with something that computes the estimate on each pass and records the best as the iteration seeks it. That will nearly double the computational burden so we won't do that in this class.
 * Another variation would be a class for trying to reduce a ratio of integers to fit into a smaller integer representation.
 *
 * Note: during development the ratio was easily inverted by minor changes to the initialization, which depended a tad illegitimately on the iterative step to save on some code.
 * The terse variable names match the wikipedia article this math was extracted from. Search 'simple continued fraction'.
 */
class ContinuedFractionRatioGenerator {
public:
  using ValueType=unsigned; //could use a template argument, but then the whole of the class would have to be in the header. Instead use a limit appropriate for a shorter unsigned type to get the best ratio for that type.
  constexpr static const unsigned maxWorkingBits = std::numeric_limits<ValueType>::digits;
  constexpr static const double epsilon = bit_cast((1023ul-maxWorkingBits)<<52);//IEEE_64 specific pow(2,-x)

public:
  double fraction;
  uintmax_t an;//the next coefficient of the continued fraction, using maximum precision
  /** how large the numerator or denominator may be.
   * The minimal constraint is to not overflow the computer representation.
   * This is usually a power of 2 - 1.
   */
  ValueType limit;

  //built by recursion relationship, where each step depends on prior 2, so we need a memory that is 3 deep.
  ValueType h[3]; //cumulative numerator
  ValueType k[3]; //cumulative denominator


  /** create one for learning the algorithm, then call restart followed by looping on step inspecting numerator and denominator as you go. */
  ContinuedFractionRatioGenerator();

  /** create and iterate. By the time this returns it holds the best ratio. */
  ContinuedFractionRatioGenerator(double ratio, ValueType limit = 0) {
    restart(ratio, limit);
    best();
  }

  /** numerator and denominator will be <limit when iteration stops. for hardware use the limit value should be ((1<<numbits)-1)
   * @param ratio is expected to be positive, if not hilarity ensues. @returns an OK to iterate which is not yet sane, ignore it. */
  bool restart(double ratio, ValueType limit = ~0U);

  /** @returns whether the number format allows a more precise computation, if so then computation is done and has been stored internally.*/
  bool step();

  /** @returns a number greater than denominator if input value is greater than 1 */
  ValueType numerator() const {
    return h[1];
  }

  /** @returns a number greater than numerator if input value is less than 1 */
  ValueType denominator() const {
    return k[1];
  }

  /** current approximation, used for testing the algorithm */
  double approximation() const;

  /** the average of the last 2 is always closer to the original than either of them */
  double better() const;

  void run();

  /** calls steps() until it says to stop then @returns approximation() */
  double best();

private:
  bool split();

  bool bump(ValueType hk[3]);
};
