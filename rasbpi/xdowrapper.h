#ifndef XDOWRAPPER_H
#define XDOWRAPPER_H  "(C)2022 Andy Heilveil (github/980f)"

/** xdo.h is not sane for c++ programs, but this extern wrapping takes care of that */
extern "C" {
#include "xdo.h"
}

class XdoWrapper {

    xdo_t* wrapped;
public:
  XdoWrapper();
  ~XdoWrapper(){
      if(wrapped){
          xdo_free(wrapped);
          wrapped=nullptr;
      }
  }

  class Keystroker { //funny spelling due to lack of namespace in xdo.h
      XdoWrapper &xdo;
      Window windowID;
  public:
      Keystroker(XdoWrapper &parent):xdo(parent){
          windowID=CURRENTWINDOW;
      }

      /** sends one keystroke. simple ascii chars are their own keysym, but this thing needs a string not a char*/
      void sendXkey(const char *xkeysym){
          xdo_send_keysequence_window(xdo.wrapped,windowID,xkeysym,0);
      }

      void operator()(char sea){
          char stringer[2];
          stringer[0]=sea;
          stringer[1]=0;
          sendXkey(stringer);
      }

  };

  Keystroker keystroker(){
      return Keystroker(*this);
  }

};

#endif // XDOWRAPPER_H
