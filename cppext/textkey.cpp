#include "textkey.h"
#include "stdlib.h"
#include "string.h"

bool nonTrivial(TextKey t){
  return t != nullptr && *t != 0;
}

void denull(TextKey &t){
  if(t == 0) {
    t = ""; //hopefully compiler creates a shared one of these empty strings.
  }
}

bool same(TextKey a, TextKey b){
  if(a == b) {
    return true;
  }
  if(a == nullptr) {
    return *b == 0;
  }
  if(b == nullptr) {
    return *a == 0;
  }
  if(0 == strcmp(a, b)) {
    return true;
  } else {
    return false;
  }
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
