#ifndef NumericalValue_H
#define NumericalValue_H  "(C) 2017 Andrew L. Heilveil"

/** union of numerical types, with indicator as to what type of last assignment was.
The type is never automatically changed herein, data however is converted in place when type changes */
struct NumericalValue {
  union {
    bool bee;
    int eye;
    unsigned ewe;
    //more later
    double dee;
  } storage;

  enum Detail {
    Truthy,
    Whole,
    Counting,
    Floating
  };

public:
  NumericalValue();
  Detail is;
  /** @returns whether type is changed, and alters storage from old type to the new. */
  bool changeInto(Detail newis);

  /** @returns reference to value as if type Numeric */
  template <typename Numeric> Numeric &as() noexcept{
    return *reinterpret_cast<Numeric *>(&storage);
  }

  /** @returns reference to value as if type Numeric */
  template <typename Numeric> operator Numeric &()noexcept{
    return as<Numeric>();
  }

  /** @returns reference to value as if type Numeric */
  template <typename Numeric> const Numeric &as()const noexcept{
    return *reinterpret_cast<Numeric *>(&storage);
  }

  /** @returns reference to value as if type Numeric */
  template <typename Numeric> operator const Numeric &()const noexcept{
    return as<Numeric>();
  }

  /** @returns value converted to given type, without altering this. */
  template <typename Numeric> Numeric cast() const noexcept;

  /** @returns value as double, converting as needed */
  double value()const noexcept;
  /** @returns value as double, converting as needed */
  operator double()const noexcept {
    return value();
  }

  /** assign value from @param d converting if needed. */
  bool setto(double d);
  /** assign value from @param d converting if needed. */
  void operator =(double d);

  /** demo of essential syntax*/
  static void testUsage();
};

//explicit instantiations are made in numericalvalue.cpp, add one for each type that gives you a linker error.
template <typename Numeric> NumericalValue::Detail detail();

#endif // UNIONIZER_H
