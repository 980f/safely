/**
// Created by andyh on 12/22/24.
// Copyright (c) 2024 Andy Heilveil, (github/980f). All rights reserved.
*/

#include "toker.h"

#include <cstring>

bool Tokenizer::hasNext(const char *delim, const char *whitespace) {
  if (!cursor || *cursor == 0) {
    return false;
  }
  if (whitespace) {
    auto skips = strspn(cursor, whitespace);
    cursor += skips;
    if (*cursor == 0) {
      return false; //ignore trailing 'whitespace', it is not an empty token.
    }
  }
  end = strpbrk(cursor, delim);
  if (end) {
    terminator = *end; //might be a null char, this guy doesn't care.
    //now erase trailing whitespace but not interior
    for (char *eraser = end; strchr(whitespace, *eraser);) {
      *eraser-- = 0;
    }
    //do NOT erase whitespace after the delimiter, the definition of subsequent whitespace might change contingent on the delim or token.
    if (terminator == 0) {
      end = nullptr; //for debug
    } else {
      *end = 0;
      if (terminator == '\r' && end[1] == '\n') {
        *++end = 0;
      }
    }
  }
  return true;
}

char *Tokenizer::fetch() {
  auto answer = cursor;
  cursor = end && *end ? ++end : nullptr;
  return answer;
}
