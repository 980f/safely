#ifndef BITBANGER_H
#define BITBANGER_H "need to exclude other bitbanger headers that might be laying about"
//(C) 2017,2018 Andy Heilveil , github/980F



/** bit and bitfield setting and getting.*/

//Arduino has macros where we have inline functions, ours are safer and just as performant.
#ifdef ARDUINO
#undef bit
#undef bitClear
#undef bitRead
#undef bitSet
#undef bitWrite

#define bit(n)         (1u<<(n))
#define bitClear(x,n) clearBit(x,n)
#define bitRead(x,n)  bitFrom(x,n)
#define bitSet(x,n)   setBit(x,n)
#define bitWrite(x,n,v) assignBit(x,n,v)

#endif

/** @returns byte address argument as a pointer to that byte */
constexpr unsigned *atAddress(unsigned address) {
  return reinterpret_cast<unsigned *>(address);
}

/** graciously letting Arduino usurp our function name, will rewrite all 980F stuff to deal with it.*/
constexpr bool bitFrom(unsigned patter, unsigned bitnumber) {
  return (patter & (1u << bitnumber)) != 0;
}

constexpr bool isOdd(unsigned pattern) {
  return pattern & 1u;
}

constexpr bool isEven(unsigned pattern) {
  return !isOdd(pattern);
}

template<typename Scalar> bool setBit(volatile Scalar &patter, unsigned bitnumber) {
  return patter |= (1u << bitnumber);
}

template<typename Scalar> bool setBit(volatile Scalar *patter, unsigned bitnumber) {
  return *patter |= (1u << bitnumber);
}

inline bool setBitAt(unsigned addr, unsigned bitnumber) {
  return setBit(*atAddress(addr), bitnumber);
}

template<typename Scalar> bool clearBit(volatile Scalar &patter, unsigned bitnumber) {
  return patter &= ~(1u << bitnumber);
}

template<typename Scalar> bool clearBit(volatile Scalar *patter, unsigned bitnumber) {
  return *patter &= ~(1u << bitnumber);
}

inline bool clearBitAt(unsigned addr, unsigned bitnumber) {
  return clearBit(*atAddress(addr), bitnumber);
}

/** ensure a 0:1 transition occurs on given bit. */
inline void raiseBit(volatile unsigned &address, unsigned bit) {
  clearBit(address, bit);
  setBit(address, bit);
}

/** ensure a 0:1 transition occurs on given bit. */
inline void raiseBit(volatile unsigned *address, unsigned bit) {
  clearBit(address, bit);
  setBit(address, bit);
}

template<typename Scalar> bool assignBit(Scalar &pattern, unsigned bitnumber, bool one) {
  if (one) {
    setBit(pattern, bitnumber);
  } else {
    clearBit(pattern, bitnumber);
  }
  return one;
}

struct BitReference {
  unsigned &whole;//#renamed due to Arduino conflict, they have a global 'word' macro.
  unsigned mask;

  /** initialize from a memory address and bit therein. If address isn't aligned then bitnumber must be constrained to stay within the same word*/
  BitReference(unsigned memoryAddress, unsigned bitnumber) :
    whole(*atAddress(memoryAddress & ~3u)),
    mask(1u << (31u & ((memoryAddress << 3u) | bitnumber))) {
    //now it is an aligned 32 bit entity
  }

  bool operator=(bool set) const { // NOLINT(misc-unconventional-assign-operator,cppcoreguidelines-c-copy-assignment-signature)
    if (set) {
      whole |= mask;
    } else {
      whole &= ~mask;
    }
    return set;
  }

  operator bool() const { // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
    return (whole & mask) != 0;
  }
};

/** @returns splice of two values according to @param mask */
template<typename Scalar> constexpr Scalar insertField(const Scalar &target, unsigned source, unsigned mask) {
  return (target & ~mask) | (source & mask);
}

/** splices a value into another according to @param mask */
template<typename Scalar> Scalar mergeInto(Scalar &target, unsigned source, unsigned mask) {
  return target = insertField(target, source, mask);
}

/** splices a value into another according to @param mask */
template<typename Scalar> Scalar mergeInto(Scalar *target, unsigned source, unsigned mask) {
  return *target = insertField(*target, source, mask);
}

/** @returns bits @param msb through @param lsb set to 1.
   Default arg allows one to pass a width for lsb aligned mask of that many bits */
constexpr unsigned fieldMask(unsigned msb, unsigned lsb = 0) {
  return (1u << (msb + 1)) - (1u << lsb);
}

/** use the following when offset or width are NOT constants, else you should be able to define bit fields in a struct and let the compiler to any inserting*/
constexpr unsigned int insertField(unsigned target, unsigned source, unsigned msb, unsigned lsb) {
  return insertField(target, source << lsb, fieldMask(msb, lsb));
}

inline unsigned mergeField(volatile unsigned &target, unsigned source, unsigned msb, unsigned lsb) {
  return target = insertField(target, source, msb, lsb);
}

inline unsigned int extractField(unsigned int source, unsigned int msb, unsigned int lsb) {
  return (source & fieldMask(msb, lsb)) >> lsb;
}

/** @returns bits @param lsb for width @param width set to 1.
   Default arg allows one to pass a width for lsb aligned mask of that many bits */
constexpr unsigned bitMask(unsigned lsb, unsigned width = 1) {
  return (1u << (lsb + width)) - (1u << lsb);
}

/** use the following when offset or width are NOT constants, else you should be able to define bit fields in a struct and let the compiler to any inserting*/
constexpr unsigned int insertBits(unsigned target, unsigned source, unsigned lsb, unsigned width) {
  return insertField(target, source << lsb, bitMask(lsb, width));
}

inline unsigned mergeBits(unsigned &target, unsigned source, unsigned lsb, unsigned width) {
  return mergeInto(target, source << lsb, bitMask(lsb, width));
}

inline unsigned mergeBits(unsigned *target, unsigned source, unsigned lsb, unsigned width) {
  return mergeInto(target, source << lsb, bitMask(lsb, width));
}

constexpr unsigned extractBits(unsigned source, unsigned lsb, unsigned width) {
  return (source & bitMask(lsb, width)) >> lsb;
}

/** for when the bits to pick are referenced multiple times and are compile time constant
 * trying to bind the item address as a template arg runs afoul of the address not being knowable at compile time.
 * while it is tempting to have a default of 1 for msb/width field, that is prone to users walking away from a partially edited field.
*/
template<unsigned lsb, unsigned msb, bool msbIsWidth = true> class BitFielder {
  enum {
    mask = msbIsWidth ? bitMask(lsb, msb) : fieldMask(msb, lsb) // aligned mask
  };
public:
  static unsigned extract(unsigned &item) {
    return (item & mask) >> lsb;
  }

  static unsigned mergeInto(unsigned &item, unsigned value) {
    unsigned merged = (item & ~mask) | ((value << lsb) & mask);
    item = merged;
    return merged;
  }
};

/** BitPicker access a bit in a word that might not accept struct bit field manipulation.
  It is very much like BitFielder, but takes advantage of knowing that a single bit is involved allowing for explicit use of 'bool' in the interface.*/

template<unsigned lsb> class BitPicker {
  enum {
    mask = bitMask(lsb) // aligned mask
  };
public:
  unsigned extract(unsigned &item) const {
    return (item & mask) >> lsb;
  }

  unsigned operator()(unsigned &&item) const {
    return extract(item);
  }

  bool operator()(unsigned &word, bool set) const {
    if (set) {
      word |= mask;
    } else {
      word &= ~mask;
    }
    return set;
  }
};

/** Create this object around a field of an actual data item.
   trying to bind the address as a template arg runs afoul of the address often not being knowable at compile time*/
template<unsigned lsb, unsigned msb, bool msbIsWidth = false> class BitField : public BitFielder<lsb, msb, msbIsWidth> {
  unsigned &item;
public:
  BitField(unsigned &item) : item(item) {
  }

  operator unsigned() const {
    return BitFielder<lsb, msb>::extract(item);
  }

  void operator=(unsigned value) const {
    BitFielder<lsb, msb>::mergeInto(item, value);
  }
};

/** for hard coded absolute (known at compile time) address and bit number */
template<unsigned memoryAddress, unsigned bitnumber> struct KnownBit {
  enum {
    word = memoryAddress & ~3,
    mask = (1 << (31 & ((memoryAddress << 3) | bitnumber)))
  };

  bool operator=(bool set) const {
    if (set) {
      *atAddress(word) |= mask;
    } else {
      *atAddress(word) &= ~mask;
    }
    return set;
  }

  operator bool() const {
    return (*atAddress(word) & mask) != 0;
  }
};


///////////////////////////////////////////
/// a group of discontiguous bits, used for bitmmasking


/** declarative part of 3 step template magic */
template<unsigned ... list> struct BitWad;

/** termination case of 3 step template magic */
template<unsigned pos> struct BitWad<pos> {
  enum {
    mask = 1 << pos
  };
public:
  inline static unsigned extract(unsigned varble) {
    return (mask & varble);
  }

  static bool exactly(unsigned varble, unsigned match) {
    return extract(varble) == extract(match); // added mask to second term to allow for lazy programming
  }

  static bool all(unsigned varble) {
    return extract(varble) == mask;
  }

  static bool any(unsigned varble) {
    return extract(varble) != 0;
  }

  static bool none(unsigned varble) {
    return extract(varble) == 0;
  }

  /* place bit from packed into position. A separate operation merges that with target data */
  static unsigned splatter(unsigned packed) {
    return (packed & 1) << pos;
  }

  template<typename Scalar> static Scalar mergeInto(Scalar &target, Scalar packed) {
    return mergeInto(target, splatter(packed), mask);
  }
};

/** assemble a bit field, without using stl. */
template<unsigned pos, unsigned ... poss> struct BitWad<pos, poss ...> {
  enum {
    mask = BitWad<pos>::mask | BitWad<poss ...>::mask
  };

public:
  inline static unsigned extract(unsigned varble) {
    return (mask & varble);
  }

  static bool exactly(unsigned varble, unsigned match) {
    return extract(varble) == extract(match); // added mask to second term to allow for lazy programming
  }

  static bool all(unsigned varble) {
    return extract(varble) == mask;
  }

  static bool any(unsigned varble) {
    return extract(varble) != 0;
  }

  static bool none(unsigned varble) {
    return extract(varble) == 0;
  }

  static unsigned splatter(unsigned packed) {
    return ((packed & 1) << pos) | BitWad<poss ...>::splatter(packed);
  }

  template<typename Scalar> static Scalar mergeInto(Scalar &target, Scalar packed) {
    return ::mergeInto(target, splatter(packed), mask);
  }
};

#define bits(...)  (BitWad<__VA_ARGS__>::mask)

#endif
