/**
// Created by andyh on 1/3/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/
#include <cstdint>
#include <cstring>

//////////////////////////////////
/// filters for platform specific variations

void nanoSpin(unsigned loops) {
  while (loops--) {}
}

//declared in minimath.h: (mcu versions calls in to startup code)
void copyObject(const void *source, void *target, uint32_t length) {
  memcpy(target, source, length);
}

//declared in minimath.h:
void fillObject(void *target, uint32_t length, uint8_t fill) {
  memset(target, fill, length);
}
