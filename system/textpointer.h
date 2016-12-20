#ifndef TEXTPOINTER_H
#define TEXTPOINTER_H

#include "cstr.h"

/** adds strdup'ing to Cstr functionality, i.e. makes a copy on construction and assignement vs Cstr which just looks at someone else's allocated memory.
 */
class Text : public Cstr {

public:
  /** creates an 'empty' one */
  Text();

  /** makes a copy of the @param given content if @param takeit is false, else presumes the caller is happy with this maintaining the lifetime. */
  Text(TextKey ptr,bool takeit);

  /** makes a copy of the @param given content */
  Text(TextKey ptr);


  Text(unsigned size);
public: //to detect compiler autogenerating these
  Text(Text &other);

public:

  /** deletes its copy of the content copied by the constructor */
  ~Text();

  /** useful for forcing a copy when constructing, the copy constructor is used for moving. */
  operator TextKey() const;

  /** deletes present content (if owned) and copies @param ptr content (if any).
   * @returns the @param pointer, not a pointer to self or the copy made.
   */
  TextKey operator =(TextKey other) override;

  /** take ownership of a buffer */
  void take(TextKey other);

  /** make a copy of @param other. If other points to the same memory as this ... we might screw up*/
  void copy(TextKey other);

  /** discard internal content (if any) */
  void clear() override;

}; // class TextPointer

#endif // TEXTPOINTER_H
