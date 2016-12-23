#ifndef SEQUENCE_H
#define SEQUENCE_H

/** interfaces like java iterators
 *
 * a predecessor implementation didn't have as many 'const's as they got in the way of some anticipatory caching. Such extensions will now have to do their anticipating
 * conditionally in the next() method, or any other method that affects the 'hasNext' concept.
 */
template<typename Content> class Sequence {
public:
  virtual bool hasNext(void) const = 0;
  virtual Content&next(void) = 0;

  virtual void skip(unsigned int qty = 1){
    while(hasNext() && qty-- > 0) {
      next();
    }
  }

}; // class Sequence

template<typename Content> class ReadonlySequence {
public:
  virtual bool hasNext(void) const = 0;
  virtual Content next(void) = 0;

  virtual void skip(unsigned int qty = 1){
    while(hasNext() && qty-- > 0) {
      next();
    }
  }

}; // class ReadonlySequence


template<typename Content> class PeekableSequence : public Sequence<Content> {
public:
  virtual Content&peek(void) const = 0; //const'ed, preview with caching wasn't worth the loss of being able to work on const references.
  virtual void unget(void) = 0;
};

template<typename Content> class LatentSequence : public PeekableSequence<Content> {
public:
  virtual bool hasPrevious(void) const = 0;
  virtual Content&previous(void) const = 0;
  virtual unsigned int used(void) const = 0;
};

#endif // SEQUENCE_H
