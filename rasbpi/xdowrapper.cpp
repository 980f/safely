#include "xdowrapper.h"

XdoWrapper::XdoWrapper():wrapped(nullptr){
    wrapped=xdo_new(nullptr);
}

XdoWrapper::~XdoWrapper(){
  xdo_free(wrapped);//tolerates null input
  wrapped=nullptr;
}

bool XdoWrapper::attach(const char *displayname){
  wrapped=xdo_new(displayname);
  return wrapped!=nullptr;
}

XdoWrapper::Windoe::Windoe(XdoWrapper &parent):xdo(parent){
  windowID=CURRENTWINDOW;
}

bool XdoWrapper::Windoe::attachClass(const char *classname){
  xdo_search_t criteria;
  criteria.title=criteria.winname=classname;//JIC
  criteria.winclass=criteria.winclassname=classname;
//newer xdolib:  criteria.winrole=nullptr;
  criteria.searchmask=SEARCH_CLASSNAME;
  criteria.require=xdo_search::SEARCH_ALL;

  criteria.limit=1;
  criteria.max_depth=2;
  criteria.only_visible=false;
  criteria.screen=0;//todo: confirm this is 'none' which is the same as all
  criteria.desktop=0;//presume 'all'
  criteria.pid=0; //better than garbage

  WindowSearch search(xdo);
  if(search(criteria)){
    windowID=search[0];
    return true;
  }
  return false;
}

bool XdoWrapper::Windoe::sendXkey(const char *xkeysym){
  if(xdo.wrapped){
    xdo.errcode = xdo_send_keysequence_window(xdo.wrapped,windowID,xkeysym,0);
    return xdo.errcode==0;
  }
  return true;//not a failure since user has already ignored total failure to connect
}

bool XdoWrapper::Windoe::operator()(char sea){
  char stringer[2];
  stringer[0]=sea;
  stringer[1]=0;
  return sendXkey(stringer);
}
