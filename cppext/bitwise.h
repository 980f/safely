#ifndef BITWISE_H
#define BITWISE_H

//todo: import from ezcpp repo

//for non-bit addressable items:
constexpr bool bit(int patter, unsigned int bitnumber){
  return (patter & (1 << bitnumber)) != 0;
}

constexpr unsigned int fieldMask(unsigned int width){
  return (1 << width) - 1;
}

constexpr unsigned int fieldMask(unsigned int offset,unsigned int width){
  return fieldMask(width)<<offset;
}

constexpr unsigned int extractField(unsigned int source, unsigned int offset, unsigned int width){
  return (source >> offset) & fieldMask(width);
}

/** use the following when offset or width are NOT constants, else you should be able to define bit fields in a struct and let the compiler to any inserting*/
inline unsigned int insertField(unsigned int target, unsigned int source, unsigned int offset, unsigned int width){
  unsigned int mask = fieldMask(width,offset);
  return (target & ~mask) | ((source << offset) & mask);
}


//////////// frequently used constructs ////////
class Boolish {
public:
  virtual bool operator = (bool truth) = 0;
  virtual operator bool(void) = 0;
};


#endif // BITWISE_H
