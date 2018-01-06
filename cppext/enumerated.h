#ifndef ENUMERATED_H_
#define ENUMERATED_H_

#include "textkey.h"

//namespace Safely {
/** base class is abstract access to a labeling mechanism */
<<<<<<< HEAD
  class Enumerated {
  protected:
    virtual ~Enumerated() = default;
  public:
    /** gets returned by token() for invalid values.*/
    static const TextKey InvalidToken;

    /** useful for presizing enum indexed storage:*/
=======
class Enumerated {
protected:
    virtual ~Enumerated() = default;
public:
  /** gets returned by token() for invalid values.*/
  static const TextKey InvalidToken;

  /** useful for presizing enum indexed storage:*/
>>>>>>> dp5qcu
    virtual unsigned numTokens() const = 0;

    /** @returns internal key for present integral value, InvalidToken if @param value isn't valid */
    virtual TextKey token(unsigned value) const = 0;

    /** @returns the integral value associated with an internal key. BadIndex if @paran token isn't valid */
    virtual unsigned valueOf(TextKey token) const = 0;
<<<<<<< HEAD
  }; // class Enumerated
=======

}; // class Enumerated
>>>>>>> dp5qcu

/** a finite set of symbolic names.
 * named for original use where ints were purely ascending from 0,
 * NB: This implementation presumes that the defining table of tokenNames outlives this enumeration. */
<<<<<<< HEAD
  class SequentialEnumeration : public Enumerated {
  protected:
    const unsigned quantity;
    const char **tokenNames;

  public:
    /** use BEGIN_ENUM to call this, so that we can change the implementation again:*/
    SequentialEnumeration(const char *tokenNames[], unsigned quantity);
    /** useful for presizing enum indexed storage:*/
    unsigned numTokens() const override;

    /** @returns internal key for present integral value*/
    TextKey token(unsigned ordinal) const override;

    /**  @returns the integral value associated with an internal name.*/
    unsigned valueOf(TextKey token) const override;
  }; // class SequentialEnumeration
=======
class SequentialEnumeration : public Enumerated {
protected:
    const unsigned quantity;
  const char **tokenNames;

public:
  /** use BEGIN_ENUM to call this, so that we can change the implementation again:*/
    SequentialEnumeration(const char *tokenNames[], unsigned quantity);
  /** useful for presizing enum indexed storage:*/
    unsigned numTokens() const override;

  /** @returns internal key for present integral value*/
    TextKey token(unsigned ordinal) const override;

  /**  @returns the integral value associated with an internal name.*/
    unsigned valueOf(TextKey token) const override;
}; // class SequentialEnumeration
>>>>>>> dp5qcu

// *INDENT-OFF*   unbalanced braces in the macros piss off uncrustify
//wrap this in case we change the naming rules (again).
#define DECLARE_ENUM(name) extern SequentialEnumeration name

//the following should go only in the CPP file creating the symbols:
#define BEGIN_ENUM(name) const char *name ## _Tokens[] = {

#define END_ENUM(name) }; \
  SequentialEnumeration name(name ## _Tokens, countof(name ## _Tokens))

//}//end namespace
//using Safely::SequentialEnumeration;

#endif // _ENUMERATED_H_
