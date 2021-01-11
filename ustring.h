#ifndef USTRING_H
#define USTRING_H

#include "eztypes.h"
#include "utf8.h" //
#include "textkey.h" //maybe base on Textpointer, or Indexer<u8>


/** wrapper for common subset of classes like Glib::ustring and std::string
 *  the implementation file here is a simple one, but may integrate with UTF* processing classes.
 * the 'std' implementations are very fat albeit very functional.
 *
 * must support cheap copy.
 */
class Ustring {
public:
  Ustring();
  /** copy content */
  Ustring(const char *content);
  /** copy other */
  Ustring(const Ustring &other);

  bool empty() const;
  /** character length, not byte length */
  unsigned length() const;
  /** nth unicode char, not byte */
  Unichar at(unsigned pointer) const;

  bool operator ==(const Ustring &other) const;
  /** pointer to raw storage, presumed to be null terminated */
  char * c_str() const;

  Ustring operator +(const char *more);
  bool operator !=(const char *more);
  Ustring &append(const char *more);
  Ustring &append(const Ustring &more);

  /** modify buffer*/
  void erase(unsigned uindex, int quantity);
  void insert(unsigned uindex,const Ustring &paste);

  /** format a number with its units. Not sure where precision comes from!*/
  static Ustring format(double,const char*space,const Ustring &uom);
  /** format a number, Not sure where precision comes from!*/
  static Ustring format(...);//NYI


private: //template varargs majick

  /** @param which is the ordinal of @param ref in the original function call. @param args is uninspected rest of arguments.
   *  arguments are process in order, the format string is looked at to find matching ordinal and format spec is replaced with formatted value.
   *  todo:1 field with limiter
   */
  template<typename NextArg, typename ... Args> static void compose_item(unsigned which, Ustring &format, NextArg&ref, const Args& ... args){
    try {
      for(unsigned pointer = 0; pointer < format.length(); ) {
        Unichar c = format.at(pointer++);
        if(c == '%') {
          Unichar d = format.at(pointer++);
          if(d - '0' == which) { //splice in ref
            pointer -= 2;  //2 = % and digit
            format.erase(pointer, 2);
            unsigned beforeaddition = format.length();
            format.insert(pointer, Ustring::format(ref));
            pointer += format.length() - beforeaddition; //else we try to replace things in the replaced text :)
            //by not returning here we allow for multiple substitutions of one argument.
          }
        }
      }
      if(sizeof ... (args) > 0) {//recurse
        compose_item(which + 1, format, args ...);
      }
    } catch(...) {
//      wtf("Stifled exception in L10n::compose");
    }
  } // compose_item

  /** Terminates template iteration */
  static void compose_item(unsigned /*which*/, Ustring & /*format*/){
    //#nada
  } // compose_item

public:

  /** @returns composition of args */
  template<typename ... Args> static Ustring compose(TextKey format, const Args ... args){
    Ustring result(format);

    compose_item(1, result, args ...);
    return result;
  }

}; // class Ustring

#endif // USTRING_H
