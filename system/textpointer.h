#ifndef TEXTPOINTER_H
#define TEXTPOINTER_H

#include "cstr.h"

/** adds strdup'ing to Cstr functionality, i.e. makes a copy on construction and assignement vs Cstr which just looks at someone else's allocated memory.
 */
class Text : public Cstr {

public:
  /** creates an 'empty' one */
  Text();

  /** makes a copy of the @param given content */
  Text(TextKey ptr);

  /** deletes its copy of the content copied by the constructor */
  ~Text();

  /** deletes present content (if owned) and copies @param ptr content (if any).
   * @returns the param pointer, not a pointer to self or the copy made.
   */
  TextKey operator =(TextKey other) override;

  /** discard internal content (if any) */
  void clear() override;

}; // class TextPointer

#endif // TEXTPOINTER_H
