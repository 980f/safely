#include "tokenstream.h"

using namespace std;
using namespace Glib;

Token::Token() :
  type(ERROR),
  number(){
}

Token &Token::parseError(std::istream &is){
//todo: abstract logger
  fprintf(stderr, "Parse error at %d\n", int(is.tellg()));
  type = ERROR;
  string.clear();
  return *this;
}

Token Token::getToken(istream &is){
  Token token;
  char c;

  while(is.good()) {
    c = is.get();
    if(!(isspace(c) || c == ',')) {//if an arraylike node fails to print an item the following items are shifted up.
      break;
    }
  }
  if(!is.good()) {
    token.type = _EOF;
    return token;
  }
  switch(c) {
  case '{':
    token.type = OPEN;
    return token;
  case '}':
    token.type = CLOSE;
    return token;
  case ':':
    token.type = COLON;
    return token;
  case '"':
    token.type = STRING;
    while(is.good()) {
      c = is.get();
      if(c == '"') {
        return token;
      }
      if(c == '\\') {
        if(!is.good()) {
          return token.parseError(is);
        }
        c = is.get();
        switch(c) {
        default:
          token.string += '\\';//without this swallows slash of unknown escape sequence
          token.string += c;
          break;
        case '\\':
        case '\''://some poeple just like escaping any kind of quote char.
        case '"':
          token.string += c;//escaped quote
          break;
        case 'b':
          token.string += '\b';
          break;
        case 'f':
          token.string += '\f';
          break;
        case 'n':
          token.string += '\n';
          break;
        case 'r':
          token.string += '\r';
          break;
        case 't':
          token.string += '\t';
          break;
        case 'u':
          Unichar uc = 0;
          for(int i = 0; i < 4; i++) {
            uc <<= 4;
            if(!is.good()) {
              return token.parseError(is);
            }
            c = is.get();
            if(c >= '0' && c <= '9') {
              uc |= c - '0';
            } else if(c >= 'A' && c <= 'F') {
              uc |= c - 'A' + 10;
            } else if(c >= 'a' && c <= 'f') {
              uc |= c - 'a' + 10;
            } else {
              return token.parseError(is);
            }
          }
          token.string += uc;
        } // switch
      } else {
        token.string += c;
      }
    }
    return token.parseError(is);
  default:
    token.type = NUMBER;
    token.string += c;
    // Read everything in until we find whitespace or , or } or the end
    while(is.good()) {
      c = is.get();
      if(isspace(c) || c == ',' || c == '}') {
        is.unget();
        break;
      }
      token.string += c;
    }
    const char *nptr = token.string.c_str();
    //todo:2 keyword recognition goes here, like 'true' and 'false' for booleans. or 'null'
    char *endptr(nullptr);
    token.number = strtod(nptr, &endptr);
    //todo:1 move NumberEditor parseFailed() to where it can be used here.
    if(!endptr || *endptr) {
      return token.parseError(is);
    }
    token.string.clear();//may drop this line so that unaltered numerical nodes can be written back out without infinitesmal changes due just to parsing.
    return token;
  } // switch
  return token.parseError(is);
} // Token::getToken

TokenStream::TokenStream(istream &is) :
  is(is){
}

SmartPtr< TokenStream > TokenStream::create(istream &is){
  return SmartPtr< TokenStream >(new TokenStream(is));
}

Token TokenStream::get(){
  next();
  return me;
}

SmartPtr< TokenStream > TokenStream::next(){
  if(!_next) {
    me = Token::getToken(is);
    _next = create(is);
  }
  return _next;
}
