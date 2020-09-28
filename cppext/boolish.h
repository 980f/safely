#pragma once

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-unconventional-assign-operator"
#pragma ide diagnostic ignored "google-explicit-constructor"
#pragma ide diagnostic ignored "cppcoreguidelines-c-copy-assignment-signature"

//#define BOOLISH_H

/** allow on/off things to be passed around. This variation is modifiable. see BoolishRef for things that can be const even though they can be assigned to */
struct Boolish {
  //do NOT add a virtual destructor, it causes linker headaches.
  //The cost of not being able to delete one of these without getting to its concrete class is worth this limitation.
  /** @returns argument after setting the value */
  virtual bool operator =(bool) = 0; // NOLINT(cppcoreguidelines-c-copy-assignment-signature,misc-unconventional-assign-operator)
  /** @returns a boolean related to the object */
  virtual operator bool() const = 0; // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
  /** changes state of the boolean */
  virtual void toggle() {
    this->operator=(1 - bool(*this));
  }
};

/** on/off thing which is not changed while it alters some other boolean thing.
 * The NOLINT comments are the reasons this class exists, to make something implicitly appear as a boolean*/
struct BoolishRef {
  //do NOT add a virtual destructor, it causes linker headaches.
  //The cost of not being able to delete one of these without getting to its concrete class is worth this limitation.
  /** @returns argument after setting the value */
  virtual bool operator =(bool)const = 0; // NOLINT(cppcoreguidelines-c-copy-assignment-signature,misc-unconventional-assign-operator)
  virtual operator bool()const = 0; // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
  virtual void toggle()const {
    this->operator=(1 - bool(*this));
  }
};

/*bool flagged(const BoolishRef &varb) ISRISH; //mark as needing critical optimization */
inline bool flagged(BoolishRef &varb){
  bool was=varb;
  varb=false;
  return was;
}


/** a const-able reference to a non-const bool. */
class BoolRef: public BoolishRef {
    bool &ref;
  public:
    BoolRef(bool &ref): ref(ref) {} // NOLINT(hicpp-explicit-conversions)
    bool operator =(bool value)const override {
      return ref = value;
    }
    operator bool()const override {
      return ref;
    }
};

/** for when the read can't afford to regenerate what was last written */
class CachedBoolish: public Boolish {
    mutable bool bit;
  public:
    //do NOT add a virtual destructor, it causes linker headaches for microcontroller builds.
    //The cost of not being able to delete one of these without getting to its concrete class is worth this limitation.
    /** @see Boolish::operator= */
    bool operator =(bool on) override {
      bit=on;
      return on;
    };

    operator bool() const override {
      return bit;
    };
};

/** creating one of these set the bit (to @param polarity) deleting it (typically automatic at end of scope) clears the bit. */
template <const BoolishRef &lockbit, bool polarity = true> struct LockBit {

  LockBit() {
    lockbit = polarity;
  }

  ~LockBit() {
    lockbit = !polarity;;
  }
};

#pragma clang diagnostic pop
