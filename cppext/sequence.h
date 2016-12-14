#ifndef SEQUENCE_H
#define SEQUENCE_H

/** interfaces like java iterators**/
template <typename Content> class Sequence {
public:
  virtual bool hasNext(void) = 0;//const removed so that hasNext's can do caching lookaheads
  virtual Content&next(void) = 0;

  virtual void skip(unsigned int qty=1){
    while(hasNext() && qty-- > 0) {
      next();
    }
  }
};

template <typename Content> class ReadonlySequence {
public:
  virtual bool hasNext(void) = 0;//const removed so that hasNext's can do caching lookaheads
  virtual Content next(void) = 0;

  virtual void skip(unsigned int qty=1){
    while(hasNext() && qty-- > 0) {
      next();
    }
  }
};


template <typename Content> class PeekableSequence : public Sequence <Content> {
public:
  virtual Content&peek(void) = 0; //un-const'ed so that preview with caching can be used.
  virtual void unget(void) = 0;
};

template <typename Content> class LatentSequence : public PeekableSequence <Content> {
public:
  virtual bool hasPrevious(void) const = 0;
  virtual Content&previous(void) = 0;
  virtual unsigned int used(void) const = 0;
};

#endif // SEQUENCE_H
