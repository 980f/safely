#ifndef _ENUMERATED_H_
#define _ENUMERATED_H_

#include <glibmm.h>
#include "uicore.h"
#include "sequence.h"

/** base class is abstract access to a labeling mechanism */
class Enumerated {

public:
  /** gets returned by token() for invalid values.*/
  static const TextKey InvalidToken;

  /** useful for presizing enum indexed storage:*/
  virtual int numTokens() const =0;

  /** @returns internal key for present integral value*/
  virtual TextKey token(int value) const=0;

  /** @returns the integral value associated with an internal key.*/
  virtual int valueOf(TextKey token) const=0;

  /** @returns user text */
  virtual Glib::ustring displayName(int value)const;

};

/** a finite set of symbolic names.
 *NB: This implementation presumes that the defining table is in code, not read in from a file! You can of course dynamically build an equivalent layout. */
class SequentialEnumeration: public Enumerated {
protected:
  int simpleTokens;
  const char **tokenNames;

public:
  /** use BEGIN_ENUM to call this, so that we can change the implementation again:*/
  SequentialEnumeration(const char *tokenNames[], int numTokens);
  ~SequentialEnumeration();
  /** useful for presizing enum indexed storage:*/
  int numTokens() const;

  /** @returns internal key for present integral value*/
  TextKey token(int ordinal) const;

  /** named for original use where ints were purely ascending from 0, @returns the integral value associated with an internal name.*/
  int valueOf(TextKey token) const;
};

/** A set of names associated with a set of numbers
    Note:  We assume that each name and number only occurs once in each array */
class DynamicEnumeration: public Enumerated {
protected:
  int *numbers;
  const char **names;
  int size;

public:
  DynamicEnumeration(const char *tokenNames[], int tokenNumbers[], int size);
  /** @returns the size of the arrays */
  int numTokens() const;
  TextKey token(int number) const;
  int valueOf(TextKey name) const;
  /** @returns a name associated with @param number.  @returns nullptr if not found. */
  TextKey enumToken(int number) const;
    /** @returns a number associated with @param name.  @returns -1 if not found. */
  int enumValue(TextKey name) const;
};


//wrap this in case we change the naming rules (again).
#define DECLARE_ENUM(name) extern SequentialEnumeration name

//the following should go only in the CPP file creating the symbols:
#define BEGIN_ENUM(name) const char *name ## _Tokens[] = {

#define END_ENUM(name) };\
  SequentialEnumeration name (name ## _Tokens, countof(name ## _Tokens))
#endif // _ENUMERATED_H_
