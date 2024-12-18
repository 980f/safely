#pragma once

/**
 *  math related functions that either had platform/compiler variations or have platform specific optimizations (such as implementing in assembly code using processor specific instructions).
 */


#include <cmath>
#include "eztypes.h"

//portable nan etc. symbols, our compilers don't seem to agree on these guys, or the syntax is horrible.
const extern double Infinity;
const extern double Nan;

/** specifically nan, not infinity, @see isSignal() */
constexpr bool isNan(double d);

/** isNan(int) exists to allow templates to use isNan for floats without fancy type testing*/
constexpr bool isNan(int) {
  return false;
}

constexpr bool isNan(unsigned) {
  return false;
}

/** is a normalized fp number, excludes zero and signals */
constexpr bool isNormal(double d);

/** is not a value */
constexpr bool isSignal(double d);

/** is either 0 or not a nan. */
constexpr bool isDecent(double d);

/** Note: 'signbit' is a macro in math.h that pertains only to floating point arguments
 * @returns sign of operand, and converts operand to its magnitude, MININT(0x800...) is still MININT and must be interpreted as unsigned to work correctly
 */
template<typename Numerical> constexpr int signabs(Numerical &absolutatus) {
  if (absolutatus < 0) {
    absolutatus = -absolutatus;
    return -1;
  }
  return absolutatus ? 1 : 0;
}

/** Note: 'signbit' is a macro in math.h that pertains only to floating point arguments
 * @returns sign of operand, and converts operand to its magnitude, MININT(0x800...) is still MININT and must be interpreted as unsigned to work correctly
 */
template<typename Numerical> Numerical constexpr absvalue(Numerical absolutatus, int *sign = nullptr) {
  if (absolutatus < 0) {
    if (sign) {
      *sign = -1;
    }
    return -absolutatus;
  }
  if (sign) {
    *sign = absolutatus ? 1 : 0;
  }
  return absolutatus;
} // absvalue

//yet another filter to reconcile platform math.h issues. Make specializations per platform for performance.
template<typename mathy> constexpr int signof(mathy x) {
  if (x < 0) {
    return -1;
  }
  if (x != 0) { //using != instead of > makes NaN's positive instead of 0
    return +1;
  }
  return 0;
}

/** legacy */
constexpr int signum(int anint) {
  return signof(anint);
}

/** @returns positivity as a multiplier */
constexpr int polarity(bool positive) {
  return positive ? 1 : -1;
}

/** @return negative if lhs is < rhs, 0 if lhs==rhs, +1 if lhs>rhs .
 *  to sort ascending if returns + then move lhs to higher than rhs.
 *  todo:1 punt to <=> if it exists.
 */
template<typename mathy> constexpr int compareof(mathy lhs, mathy rhs) {
  return signof(lhs - rhs);
}

/** 'round to nearest' ratio of integers, 0/0 -> 1 */
template<typename Integer> Integer constexpr rate(Integer num, Integer denom) {
  if (denom == 0) {
    return num == 0 ? 1 : 0; //pathological case
  }
  return (num + (denom / 2)) / denom;
}

constexpr unsigned half(unsigned sum) {
  return (sum + 1) / 2;
}

//#rate() function takes unsigned which blows hard when given negative numbers
constexpr int half(int sum) {
  if (sum < 0) { //truncate towards larger magnitude
    return -int(half(unsigned(-sum))); //probably gratuitous but we also shouldn't be calling this with negatives so we can breakpoint here to detect that.
  } //# do not inline with a ternary, we want to be able to breakpoint on above line.
  return (sum + 1) / 2;
}

/** @return the quantity of bins needed to hold num items at denom items per bin. Is susceptible to integer overflow if num and denom are greater than half the integer range */
template<typename Integer, typename Inttoo> Integer constexpr quanta(Integer num, Inttoo denom) {
  if (denom == 0) {
    return num == 0 ? 1 : 0; //pathological case
  }
  return (num + denom - 1) / denom;
}

/** quantity of bins needed to hold num items at denom items per bin */
constexpr unsigned chunks(double num, double denom);


/** protect against garbage in (divide by zero) note: 0/0 is 0, NOT what rate() returns for the similar case.
*/
template<typename NumericTop,typename NumericBottom> constexpr double ratio(NumericTop num, NumericBottom denom) {
  if (denom == 0) { //#exact compare for pathological case
    return double(num); //attempt to make 0/0 be 1 was annoying in many use cases, where the denom was truncated instead of rounded coming from a sensor. should be signed inf or nan.
  }
  return double(num)/double(denom);//converts integers to float before divide, but after testing for zero.
}

/** round to a quantum, to kill trivial trailing DECIMAL digits*/
constexpr double rounder(double value, double quantum) {
  return quantum * chunks(value, quantum);
}

/** @returns canonical value % cycle, minimum positive value
 *  0 <= return < cycle; (the returned value is always in the half-open range defined by 'cycle'
 *
 *  Note: the C '%' operator gives negative out for negative in.
 */
constexpr unsigned modulus(int value, unsigned cycle);

/** @param accum is reduced to a number less than @param length,
 * @returns the number of subtractions that were necessary to do so.
 * Named for use in reporting rotary position from encoder without an index pulse to pick out the revolutions.
 */
template<typename Integrish, typename Integrash> Integrish constexpr revolutions(Integrish &accum, Integrash length) {
  if (length == 0) {
    return accum;
  }
  //todo:1 see if std::div or std::remquo can be applied here, for greater portability or whatever.
  Integrish cycles = accum / length;
  accum %= length;
  return cycles;
}

/** standard math lib's f_r_exp does a stupid thing for some args, we wrap it here and fix that.*/
constexpr int fexp(double d) ISRISH;

/** @returns whether the difference of the two numbers is less than a power of two times the lesser of the two. */
template<typename floating> bool constexpr nearly(floating value, floating other, int bits = 32) {
  if (isSignal(value) && isSignal(other)) { //deals with nan's and inf's and also frequent cases such as comparing zero's.
    return true;
  }
  floating diff = value - other;
  if (diff == 0.0) { //frequent case, and handles many pathologies as well
    return true;
  }
  int f1 = fexp(value);
  int f2 = fexp(other);
  //if either is zero absolute compare the other to 2^-bits;
  if (value == 0.0) { //fexp on 0 isn't sanitizable.
    return (f2 + bits) < 0;
  }
  if (other == 0.0) {
    return (f1 + bits) < 0;
  }
  int cf = fexp(diff);
  cf += bits;
  return (cf <= f1) && (cf <= f2);
} // nearly

/** @returns The base 10 exponent of @param value. Note that the number of digits for values >0 is 1+ilog10().
 * For zero this returns -1, most logic will have problems if you don't check that. */
constexpr int ilog10(u32 value);

constexpr int ilog10(u64 value);

constexpr int ilog10(double value);

/** an integer power of 10. out of bounds arg gets you nothing but trouble ... */
constexpr u32 i32pow10(unsigned power);

constexpr unsigned digitsAbove(unsigned int value, unsigned numDigits);

/** an integer power of 10. out of bounds arg gets you nothing but trouble ... */
constexpr u64 i64pow10(unsigned power);

/** @param p19 is 10^19 times a fractional value. @param digits is the number of digits past the virtual radix point you are interested in.
 *  @returns a properly rounded int that has those digits of interest, but you may need to pad with leading zeroes. */
constexpr u64 keepDecimals(u64 p19, unsigned digits);

/** @param p19 is 10^19 times a fractional value. @param digits is the number of digits past the virtual radix point you are interested in.
 *  @returns a truncated int that has those digits of interest, but you may need to pad with leading zeroes. */
constexpr u64 truncateDecimals(u64 p19, unsigned digits);

/** filtering in case we choose to optimize this */
constexpr double dpow10(int exponent);

constexpr double dpow10(unsigned uexp);

template<typename mathy> constexpr double squared(mathy x) {
  return x * x;
}

constexpr double degree2radian(double theta);

/** n!/r!(n-r)! combinatorial function.
 * Was formerly named and documented as Pnr, but implementation was correct for Cnr and so was its usages.
 * has greater range than naive implementation of ratio of factorials.
 */
constexpr unsigned Cnr(unsigned n, unsigned r);

/** if @param a is greater than @param b set it to b and @return whether a change was made.
 *  if @param orequal is true then also return true if args are equal.
 *  if @param a is Nan then do the assign and return true */
template<typename S1, typename S2> bool depress(S1 &a, S2 b, bool orequal = false) {
  if (isNan(b)) {
    return false;
  }
  S1 b1 = S1(b); //so incomparable types gives us just one error.
  if (isNan(a) || a > b1) {
    a = b1;
    return true;
  }
  return orequal && (a == b1);
} // depress

/** if @param a is less than @param b set it to b and @return whether a change was made.
 *  if @param orequal is true then also return true if args are equal.
 *  if @param a is Nan then do the assign and return true */
template<typename S1, typename S2> bool elevate(S1 &a, S2 b, bool orequal = false) {
  if (isNan(b)) {
    return false;
  }
  S1 b1 = S1(b); //so incomparable types gives us just one error.
  if (isNan(a) || a < b1) {
    a = b1;
    return true;
  }
  return orequal && a == b1;
} // elevate

//using 'lesser' and 'greater' while we check if all of our compilers now have compatible min and max std functions.

//todo:2 see if compiler can use this for min of convertible types:
template<typename S1, typename S2> S1 lesser(S1 a, S2 b) {
  S1 b1 = S1(b); //so incomparable types gives us just one error.
  if (a < b1) {
    return a;
  } else {
    return b1;
  }
}

template<typename S1, typename S2> S1 greater(S1 a, S2 b) {
  S1 bb = S1(b);
  if (a > bb) {
    return a;
  } else {
    return bb;
  }
}

template<typename Scalar> void swap(Scalar &a, Scalar &b) noexcept { //todo:0 see if all users have upgraded to a c++ with swap in the std library, and if so kill this one.
  Scalar noxor; //don't trust that xor is non-corrupting for all scalars.
  noxor = a;
  a = b;
  b = noxor;
}

/** Things that are coded in assembler on some platforms, due to efficiency concerns.
 * In 2009 one version of the GCC compiler for ARM often produced horrible and sometimes incorrect code.
 * Time permitting these should be compiled from the C equivalents and compared to the hand coded assembler to see if we can abandon the assembler source due to compiler improvements.
 * extern "C" removed as we can use [[naked]] attribute and inline asm to code these.
 */

/* @return integer part of d, modify d to be its fractional part.
 */
constexpr int splitter(double &d){
  double eye=0;
  d = modf(d,&eye);  //todo:2 this can be done very efficiently via bit twiddling. "modf()" has an inconvenient argument order and return type.
  return int(eye);
}

constexpr unsigned splitteru(double &d){
  double eye=0;
  d = modf(d,&eye);  //todo:2 this can be done very efficiently via bit twiddling. "modf()" has an inconvenient argument order and return type.
  return unsigned(eye);
}


/** the time delay given by ticks is ambiguous, it depends upon processor clock. @72MHz CortexM3 1000 ticks is roughly one microsecond.*/
void nanoSpin(unsigned ticks);

/** rounded and overflow managed 'multiply by ratio' */
constexpr u32 muldivide(u32 arg, u32 num, u32 denom);

/** @returns static_cast<unsigned>(ceil( quantity * @param fractionThereof ))
 */
constexpr unsigned saturated(unsigned quantity, double fractionThereof);

/** fraction is a fractional multiplier, with numbits stating how many fractional bits it has.*/
constexpr u16 fractionallyScale(u16 number, u16 fraction, u16 numbits);

/** 1 + the integer part of log base 2 of the given number, pretty much is just "count the leading zeroes".
 * Note well that this will give 0 as the log of 0 rather than negative infinity, precheck the argument if you can't live with that.
 * mathematical definition: "number of right shifts necessary for an unsigned number to become 0"
 */
constexpr unsigned log2Exponent(u32 number);

/** @returns eff * 2^pow2  where pow2 is signed. This can be done rapidly via bitfiddling*/
constexpr float shiftScale(float eff, int pow2);

constexpr double flog(unsigned number);

/** @return the natural logarithm of the ratio of @param over @param under.
 * This is computable as the difference of their logs, but we wrap that here so that some fancy fiddling can reduce the number of logarithms executed. */
constexpr double logRatio(unsigned over, unsigned under);

//sane truncations:
constexpr u16 uround(float scaled);

constexpr s16 sround(float scaled);

/** NB: copyObject() and fillObject() can NOT be used with objects that contain polymorphic objects */
void copyObject(const void *source, void *target, u32 length);

void fillObject(void *target, u32 length, u8 fill);

//EraseThing only works on non-polymorphic types. On polymorphs it also kills the vtable!
#define EraseThing(thing) fillObject(&(thing), sizeof(thing), 0);
//see warning for EraseThing.
#define CopyThing(thing1,thing2) copyObject(thing1,thing2,min(sizeof(thing1),sizeof(thing2)))

//documenting accessible portions of microcontroller startup code:
void memory_copy(const void *source, void *target, void *sourceEnd);

void memory_set(void *target, void *targetEnd, u8 value);

/** split a number into its integer and fractional parts, a wrapper around ::modf.
 * it is named after the number of bins, such as array elements, needed to span the range from 0..@param d .
 * variants of splitter, allowing for greater range. @see splitter is optimized for numbers less than 32k
 *  @param d is replaced with its fractional part, the function @returns the integer part, @see standard math lib's modf for edge cases.
 */
template<typename Integrish, typename Floater> Integrish constexpr intbin(Floater &d) {
  double eye;
  d = modf(d, &eye);
  return Integrish(eye);
}
