#ifndef UNIONIZER_H
#define UNIONIZER_H

/** testing template access to union members */
struct Unionizer {
  union {
    bool bee;
    int eye;
    unsigned ewe;
    double dee;
  } storage;
public:
  Unionizer();
  template <typename Numeric> Numeric &as(){
    return *reinterpret_cast<Numeric *>(&storage);
  }
  template <typename Numeric> operator Numeric &(){
    return *reinterpret_cast<Numeric *>(&storage);
  }
  static void testUsage();
};

#endif // UNIONIZER_H
