#include "linearmapping.h"

LinearMapping::LinearMapping(){}

LinearMapping& LinearMapping::operator = (const LinearMapping &other) {
  xaxis = other.xaxis;
  yaxis = other.yaxis;
  return *this;
}

/** x clamped then converted to y */
float LinearMapping::y(float x) const {
  return yaxis.start() + yaxis.width() * xaxis.fragment(xaxis.clamped(x));
}

/** y clamped then converted to x */
float LinearMapping::x(float y) const {
  return xaxis.start() + xaxis.width() * yaxis.fragment(yaxis.clamped(y));
}

float LinearMapping::yraw(float x) const {
  return yaxis.start() + yaxis.width() * xaxis.fragment(x);
}

float LinearMapping::xraw(float y) const {
  return xaxis.start() + xaxis.width() * yaxis.fragment(y);
}

void LinearMapping::init(float ymax, float ymin, float xmax, float xmin){
  yaxis.setto(ymax, ymin);
  xaxis.setto(xmax, xmin);
}
/** @returns whether settings are changed.*/
bool LinearMapping::setParams(ConstArgSet&args){
  also(yaxis.setParams(args));
  return also(xaxis.setParams(args));
}

void LinearMapping::getParams(ArgSet&args)const{
  yaxis.getParams(args);
  xaxis.getParams(args);
}

bool LinearMapping::seemsTrivial()const {
  return !(yaxis.nonTrivial() && xaxis.nonTrivial());
}
