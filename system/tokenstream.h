#ifndef TOKENSTREAM_H
#define TOKENSTREAM_H

#include "smartptr.h"
#include <istream>
#include <glibmm/object.h>
#include <glibmm/ustring.h>

class Token {
  Token &parseError(std::istream &is);
public:
  enum Type {
    ERROR,
    _EOF,
    OPEN,
    CLOSE,
    COLON,
    NUMBER,
    STRING,
  };

  Type type;
  Ustring string;
  double number;

  Token();
  static Token getToken(std::istream &is);
}; // class Token

class TokenStream : public Glib::Object {
  std::istream &is;
  Token me;
  SmartPtr< TokenStream > _next;

  TokenStream(std::istream &is);
public:
  static SmartPtr< TokenStream > create(std::istream &is);
  Token get();
  SmartPtr< TokenStream > next();
}; // class TokenStream

#endif // TOKENSTREAM_H
