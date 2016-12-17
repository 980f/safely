//(C) 2011 Applied Rigaku Technologies. Proprietary and Confidential.
#include <glibmm/regex.h>
#include "filename.h"
//maydo: restore use of SafeStr to unhook this from Glib,
//limits.h PATH_MAX+1 is the size of the safestr.

using namespace Glib;

/** no internal slashes, conver to %5F */
ustring escape (const ustring &userinput){
  int len = userinput.length();
  char workspace[len + 1];
  workspace[len] = 0;//before we forget.
  for(int i = 0; i<len; ++i) {
    workspace[i] = userinput.at(i);
    if(workspace[i]=='/') {
      workspace[i] = '-';
    }
  }
  return ustring(workspace);
} // escape

FileName::FileName(){
  assign(root);
}

static bool isRooted(const ustring &simple){
  return !simple.empty() && simple.at(0)=='/';
}

FileName::FileName(const ustring &simple){
  if(!isRooted(simple)) {
    assign(root);
  }
  folder(simple);
}

FileName &FileName::dirname(void){
  if(lastChar()==gunichar('/')) {
    erase(length() - 1);
  }
  //it seems that rfinding the first '/' would work faster, in which case we just start the find one char earlier if the string ends in a slash.%%%5
  assign(Regex::create("[^/]*$")->replace(*this, 0, "", static_cast< RegexMatchFlags >(0)));
  return *this;
}

/** make sure string ends with given token*/
FileName &FileName::assure(char token){
  if(lastChar() != gunichar(token)) {
    append(&token, 1);
  }
  return *this;
}

FileName &FileName::folder(const ustring &s,bool escapeit){
  if(s.empty()) {
    return *this;
  }
  if(s.find(root)==0) {
    assign(escapeit ? escape(s) : s);
  } else {
    if(!isRooted(s)) {
      assure('/');
    }
    append(escapeit ? escape(s) : s);
  }
  return *this;
} // FileName::folder

FileName &FileName::ext(const ustring &s,bool escapeit){
  assure('.');
  append(escapeit ? escape(s) : s);
  return *this;
}

FileName &FileName::slash(){
  assign(root);
  return *this;
}

ustring FileName::relative() const {
  if(length() < root.length()) {
    return "";
  }
  return substr(root.length());
}

gunichar FileName::lastChar() const {
  return empty() ? 0 : at(length() - 1);
}

////////////////

NameStacker::NameStacker(FileName &namer, bool escapeit) :
  path(namer),
  oldpath(namer),
  escapeit(escapeit){
}

NameStacker::NameStacker(FileName &namer, const Glib::ustring &pushsome, bool escapeit) :
  path(namer),
  oldpath(namer),
  escapeit(escapeit){
  path.folder(pushsome, escapeit);
}

NameStacker::NameStacker(NameStacker &path) :
  path(path.path),
  oldpath(path.path),
  escapeit(path.escapeit){
}

NameStacker::NameStacker(NameStacker &path, const Glib::ustring &pushsome) :
  path(path.path),
  oldpath(path.path),
  escapeit(path.escapeit){
  this->path.folder(pushsome, escapeit);
}

NameStacker::operator FileName &(){
  return path;
}

NameStacker::~NameStacker(){
  path = oldpath;
}
