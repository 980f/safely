//"(C) Andrew L. Heilveil, 2017"
#include "converter.h"
#include "cstr.h"

Converter::~Converter(){
  //#nada
}

unsigned Converter::length(const char *source)const{
  return Cstr(source).length();
}

bool Converter::operator()(const char *source, Indexer<char> &target){
  return target.cat(source);
}


