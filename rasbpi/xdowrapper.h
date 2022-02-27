#ifndef XDOWRAPPER_H
#define XDOWRAPPER_H  "(C)2022 Andy Heilveil (github/980f)"

/** xdo.h is not sane for c++ programs, but this extern wrapping takes care of that */
extern "C" {
#include "xdo.h"
}
#include "stdlib.h" //free

class XdoWrapper {
  xdo_t* wrapped;
  int errcode=0;
public:
  XdoWrapper();
  ~XdoWrapper();

  /** @returns whether a display has been connected. If not many functions will fail silently.
   * Note: failure to connect to a display leaves our internal pointer null */
  operator bool (){
    return wrapped!=nullptr;
  }

  /** for when constructor fails and you want to try again later */
  bool attach(const char *displayname);

  struct WindowSearch {
    XdoWrapper &xdo;
    WindowSearch(  XdoWrapper &xdo):xdo(xdo){}
    Window *data=nullptr;
    unsigned int count=0;
    ~WindowSearch(){
      free(data);//is calloc'd by xdo_search
      //in case of use after free;
      data=nullptr;
      count=0;
    }
    bool operator()(const xdo_search_t &criteria){
      xdo.errcode= xdo_search_windows(xdo.wrapped, &criteria, &data, &count);
      return //neweer lib: xdo.errcode==0&&
          count>0;
    }
    Window operator [](unsigned which){
      if(which<count){
        return data[which];
      }
      return 0;
    }
  };

  /** keystrokes etc. need a window as well as an xdo accessor */
  class Windoe {
      XdoWrapper &xdo;
      Window windowID;
  public:
      Windoe(XdoWrapper &parent);

      operator bool() const {
        return windowID!=0;  // a valule of zero is accepted by some commands as 'current_window' but if we have attached properlywe will have an actual window id number
      }

      void attachTo(Window windowID){
        this->windowID=windowID;
      }

      bool attachClass(const char *classname);

      /** sends one keystroke. simple ascii chars are their own keysym, but this thing needs a string not a char*/
      bool sendXkey(const char *xkeysym);
      /** sends one ascii character */
      bool operator()(char sea);

  };



  Windoe keystroker(){
    return Windoe(*this);
  }

};

#endif // XDOWRAPPER_H
