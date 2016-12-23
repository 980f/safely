#ifndef BITWISE_H
#define BITWISE_H

//todo: import from ezcpp repo

//for non-bit addressable items:
inline bool bit(int patter, unsigned int bitnumber){
  return (patter & (1 << bitnumber)) != 0;
}

inline unsigned int fieldMask(unsigned int width){
  return (1 << width) - 1;
}

/** use the following when offset or width are NOT constants, else you should be able to define bit fields in a struct and let the compiler to any inserting*/
inline unsigned int insertField(unsigned int target, unsigned int source, unsigned int offset, unsigned int width){
  unsigned int mask = fieldMask(width);

  mask <<= offset;
  return (target & ~mask) | ((source << offset) & mask);
}

inline unsigned int extractField(unsigned int source, unsigned int offset, unsigned int width){
  unsigned int mask = fieldMask(width);

  return (source >> offset) & mask;
}

//////////// frequently used constructs ////////
class Boolish {
public:
  virtual bool operator = (bool truth) = 0;
  virtual operator bool(void) = 0;
};


#endif // BITWISE_H
