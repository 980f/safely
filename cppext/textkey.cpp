#include "textkey.h"
#include "stdlib.h"
#include "string.h"


bool isTrivial(const char *string){
  return string==nullptr || *string==0;
}

bool nonTrivial(TextKey t){
  return t != nullptr && *t != 0;
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
  char *end(nullptr);  //setting value for debug purposes
  /*todo:1 set locale to one that doesn't have triplet separators */
  double d = strtod(rawText, &end);

  if(impure) {
    *impure = nonTrivial(end);
  }
  return d;
}
