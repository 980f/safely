#include <iomanip>

#include "iterate.h"
#include "delimitingoutputstream.h"

using namespace std;

DelimitingOutputStream::DelimitingOutputStream(std::ostream &os, bool withBom, bool crlfs) :
  os(os),lineTerminator(crlfs ? "\r\n" : "\n"){
  linestarted = false;
  bom(withBom);
}

void DelimitingOutputStream::separator(){
  if(linestarted) {
    os << ',';
  }
  linestarted = true;
}

DelimitingOutputStream & DelimitingOutputStream::endl(){
  os << lineTerminator;
  linestarted = false;
  return *this;
}

DelimitingOutputStream&DelimitingOutputStream::bom(bool doit){
  if (doit) {
    os.put(0xEF);
    os.put(0xBB);
    os.put(0xBF);
  }
  return *this;
}

DelimitingOutputStream & DelimitingOutputStream::gs(){
  return endl();
}

DelimitingOutputStream &DelimitingOutputStream::put(int val){
  separator();
  os << val;
  return *this;
}

DelimitingOutputStream &DelimitingOutputStream::put(unsigned val){
  separator();
  os << val;
  return *this;
}

DelimitingOutputStream &DelimitingOutputStream::put(float val, int sigfig){
  separator();
  os << setprecision(sigfig);
  os << val;
  return *this;
}

DelimitingOutputStream &DelimitingOutputStream::put(double val, int sigfig){
  separator();
  os << setprecision(sigfig);
  os << val;
  return *this;
}

DelimitingOutputStream &DelimitingOutputStream::put(const char *text){
  separator();
  os << '"';
  while(char c = *text++) {
    if(c == '"') {
      os << '"'; // quotes are escaped by MORE QUOTES!
    }
    os << c;
  }
  os << '"';
  return *this;
} // DelimitingOutputStream::put

DelimitingOutputStream &DelimitingOutputStream::put(const Text &text){
  return put(text.c_str());
}
