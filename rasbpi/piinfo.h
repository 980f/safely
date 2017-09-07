#ifndef PIINFO_H
#define PIINFO_H "(C) Andrew L. Heilveil, 2017"

/** figure out which type of py this code is running on */

class PiInfo {
public:
  /** Pi model types and version numbers. Numbered to match wiringPi */
enum Model {
  MODEL_A,
  MODEL_B,
  MODEL_AP,
  MODEL_BP,
  MODEL_2,
  ALPHA,
  MODEL_CM,
  MODEL_07,
  MODEL_3,
  MODEL_ZERO,
  MODEL_CM3,
  MODEL_NO11,
  MODEL_ZERO_W
  } model;

enum Version {
  VERSION_1,VERSION_1_1,VERSION_1_2,VERSION_2
} rev;

enum Maker {
  MAKER_SONY,MAKER_EGOMAN,MAKER_EMBEST,MAKER_UNKNOWN
} maker;

/** megabytes of ram */
  unsigned mem;
/** whether overvoltage and maybe overclocking are enabled */
  unsigned warranty;
  /** 0 don't know, 1=original, 2= more recent. Once the 'where used' is created we may change the encoding. */
  unsigned gpioLayout;
public:
  PiInfo();

};

#endif // PIINFO_H
