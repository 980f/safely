#ifndef ENUMERATED_H_
#define ENUMERATED_H_

#include "safely.h"
#include "textkey.h"

/** base class is abstract access to a labeling mechanism */
class Enumerated {
protected:
  virtual ~Enumerated();
public:
  /** gets returned by token() for invalid values.*/
  static const TextKey InvalidToken;

  /** useful for presizing enum indexed storage:*/
  virtual int numTokens_() const = 0;

  /** @returns internal key for present integral value*/
  virtual TextKey token(int value) const = 0;

  /** @returns the integral value associated with an internal key.*/
  virtual int valueOf(TextKey token) const = 0;

}; // class Enumerated

/** a finite set of symbolic names.
 * named for original use where ints were purely ascending from 0,
 * NB: This implementation presumes that the defining table of toenNames outlives this enumeration. */
class SequentialEnumeration : public Enumerated {
protected:
  int numTokens_;
  const char **tokenNames;

public:
  /** use BEGIN_ENUM to call this, so that we can change the implementation again:*/
  SequentialEnumeration(const char *tokenNames[], int numTokens_);
  /** useful for presizing enum indexed storage:*/
  int numTokens() const;

  /** @returns internal key for present integral value*/
  TextKey token(int ordinal) const;

  /**  @returns the integral value associated with an internal name.*/
  int valueOf(TextKey token) const;
}; // class SequentialEnumeration

// *INDENT-OFF*   unbalanced braces in the macros piss off uncrustify
//wrap this in case we change the naming rules (again).
#define DECLARE_ENUM(name) extern SequentialEnumeration name

//the following should go only in the CPP file creating the symbols:
#define BEGIN_ENUM(name) const char *name ## _Tokens[] = {

#define END_ENUM(name) }; \
  SequentialEnumeration name(name ## _Tokens, countof(name ## _Tokens))
#endif // _ENUMERATED_H_
