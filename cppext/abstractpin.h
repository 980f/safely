#ifndef ABSTRACTPIN_H
#define ABSTRACTPIN_H "(C) Andrew L. Heilveil, 2017"


class AbstractPin {
public:
  /* write bit */
  virtual void operator =(bool value) noexcept =0;
  /* read bit */
  virtual operator bool() noexcept=0;
  /* flip bit */
  virtual void toggle() noexcept{
    *this=1-*this;
  }

};

#endif // ABSTRACTPIN_H
