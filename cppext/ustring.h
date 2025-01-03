#pragma once

#include "utf8.h" //
#include "textkey.h" //maybe base on Textpointer, or Indexer<uint8_t>


#ifdef SAFELY_debug_Ustring
#include "logger.h" //for wtf
#elifndef Safely_Have_Wtf
//perhaps: #define Safely_Have_Wtf
// void wtf(const char *fmt, ...){
//   // a place to breakpoint.
// }
#define wtf(...)
#endif

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
  static Ustring format(...);//NYI, add variants


private: //template varargs majick

  /** @param which is the ordinal of @param ref in the original function call. @param args is uninspected rest of arguments.
   *  arguments are processed in order, the format string is looked at to find matching ordinal and format spec is replaced with formatted value.
   *  Which is to say that the @param format string is altered via replacing its markers with rendered parameters. This is why the public entry point for this copies the given format string, then alters and returns that copy.
   *  The algorithm takes the args in order and replaces them within the 'format' string. It is not an error if an argument is not referenced.
   *
   *  todo: field with limiter
   */
  template<typename NextArg, typename ... Args> static void compose_item(unsigned which, Ustring &workspace, NextArg&ref, const Args& ... args){
    try {
      for(unsigned pointer = 0; pointer < workspace.length(); ) {
        Unichar c = workspace.at(pointer++);
        if(c == '%') {
          Unichar d = workspace.at(pointer++);
          if(d - '0' == which) { //splice in ref
            pointer -= 2;  //2 = % and digit
            workspace.erase(pointer, 2);
            unsigned beforeaddition = workspace.length();
            workspace.insert(pointer, Ustring::format(ref));
            //we could drop the next line if we want to allow strange usages, an earlier argument can insert markers for a later one, such as arg 3 being "%4%4" causing argument 4 to appear twice where %3 was.
            // the danger is that if the replacement string for an argument includes that argument we loop forever, or until we exhaust ram.
            pointer += workspace.length() - beforeaddition; //else we try to replace things in the replaced text :)
            //by not returning here we allow for multiple substitutions of one argument.
          }
        }
      }
      if(sizeof ... (args) > 0) {//recurse
        compose_item(which + 1, workspace, args ...);
      }
    } catch(...) {
     wtf("Stifled exception in Ustring::compose");
     //and we stop processing but return what we managed to do before the exception. If you have unsubstituted arg references then suspect the associated argument blows up when formatted.
    }
  } // compose_item

  /** Terminates template iteration. It is never called because of the sizeof ... bit in the that is, but the compiler can't figure that out. */
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
