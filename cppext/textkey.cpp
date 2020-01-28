#include "textkey.h"
#include "stdlib.h"
#include "string.h"
#include "minimath.h" //nan
#include "index.h"

bool isTrivial(const char *string){
  return string==nullptr || *string==0;
}

bool nonTrivial(TextKey t){
  return t != nullptr && *t != 0;
}

void denull(TextKey &t){
  if(t == nullptr) {
    t = ""; //hopefully compiler creates a shared one of these empty strings.
  }
}
bool same(TextKey a, TextKey b){
  if(a == b) {//same object
    return true;
  }
  if(a == nullptr) {
    return isTrivial(b);
  }
  if(b == nullptr) {
    return *a == 0;
  }
  return 0 == strcmp(a, b);
} // same

double toDouble(TextKey rawText, bool *impure){
  if(nonTrivial(rawText)){
    char *end(nullptr);  //setting value for debug purposes
    /*todo:1 set locale to one that doesn't have triplet separators */
    double d = strtod(rawText, &end);
    if(rawText==end){
      d=Nan;//getting stricter here
    }
    if(impure) {
      *impure = nonTrivial(end);
    }
    return d;
  } else {
    if(impure) {
      *impure = false;
    }
    return Nan;
  }
}

unsigned toIndex(TextKey rawText, bool *impure){
  if(nonTrivial(rawText)){
    char *end(nullptr);
    /*todo:1 set locale to one that doesn't have triplet separators */
    unsigned long d = strtoul(rawText, &end,10);
    if(impure) {
      *impure = nonTrivial(end);
    }
    if(rawText==end){
      return BadIndex;
    }
    return d>BadIndex?BadIndex:unsigned(d);//saturating truncation
  } else {
    if(impure) {
      *impure = false;
    }
    return BadIndex;
  }
}
