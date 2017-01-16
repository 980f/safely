#include "converter.h"
#include "cstr.h"

Converter::~Converter(){
  //#nada
}

unsigned Converter::length(const char *source)const{
return Cstr(source).length();
}

void Converter::operator()(const char *source, Indexer<char> &target){
  target.cat(source);
}


