#include "dyndeterminator.h"
#include "logger.h"
#include "minimath.h"
#include <cmath>

//iteration ranges:
#define forSize(si) for(unsigned si = size; si-->0;)
#define forP(si) for(unsigned si = p.size; si-->0;)
#define forTriangle(cl) for(unsigned cl = rw + 1; cl-->0;)
#define withOutDiagonal(cl) for(unsigned cl = rw; cl-->0;)

//allocate and 0 init.
LLSQcomputer::LLSQcomputer(unsigned numCoeff) :
  MatrixInverter(numCoeff),
  ys(size),
  solution(size){
  clear();
}

unsigned LLSQcomputer::numParams() const {
  unsigned ni = 0;
  for(auto it = ignore.rbegin(); it!=ignore.rend(); ++it) {
    if(*it == false) {//not ignored
      ++ni;
    }
  }
  return ni;
} // LLSQcomputer::numParams

bool LLSQcomputer::include(double Y, const Column &correlate){
  if(Y!=0.0 && !isNormal(Y)) {
    return false;
  }
  if(correlate.size()<size) {//allow oversized input, allows users to tack on tracer info.
    return false;
  }
  forSize(rw){
    double x = correlate[rw];
    if(!isNormal(x)) {
      return false;
    }
  }
//now we can add sample into the set, don't want to touch any values if any others are unusable.
  sumy2 += squared(Y);
  //correlates size must match this size
  forSize(rw) {
    double x = correlate[rw];
    ys[rw] += Y * x;
    forTriangle(cl){
      xs[rw][cl] += x * correlate[cl];
    }
  }
  ++numSamples;
  return true;
} // LLSQcomputer::include

void LLSQcomputer::clear(){
  MatrixInverter::clear();
  forSize(rw){
    solution[rw] = 0;
    ys[rw] = 0;
  }
  numSamples = 0;
  numFit = 0;
  sumy2 = 0.0;
}

void LLSQcomputer::applyIgnorance(){
  forSize(rw){
    if(ignore[rw]) {
      //not clearing Ys as we may use those for statistics.
      xs[rw][rw] = 1.0;
      withOutDiagonal(cl){
        xs[rw][cl] = 0;
      }
    }
  }
} // LLSQcomputer::applyIgnorance

void LLSQcomputer::fillinTriangle(){  //todo:2 move into matrixinverter and add a flag indicating symmetry
  forSize(rw){
    if(!ignore[rw]) { //trusting that clear() was called at a good time.
      withOutDiagonal(cl){
        xs[cl][rw] = xs[rw][cl]; //copy to upper triangle
      }
    }
  }
}

unsigned LLSQcomputer::compute(){
  fillinTriangle();
  double det = MatrixInverter::compute();
  //multiply Ys on left, inv on right:
  if(isNormal(det)) { //#we do wish to exclude 0 as well as strange stuff.
    forSize(rw){
      if(!ignore[rw]) {
        double product = 0;
        forSize(cl){
          if(!ignore[cl]) {
            product += ys[cl] * inv[cl][rw]; //left multiply causes apparent swap of index order of matrix
//or is it that when we dropped the pivoting we effectiveley transposed the inverse.?
          }
        }
        if(isNormal(product) || product == 0.0) {
          ++numFit;
          solution[rw] = product;
        } else {
          //where do we report on failed coefficient?
          dbg("failed to fit %d coeff",rw);
        }
      }
    }
  }
  return numFit;
} // LLSQcomputer::compute

double LLSQcomputer::sumx(unsigned which){
  return xs[0][which];
}

double LLSQcomputer::Lxx(unsigned which){
  return numSamples * xs[which][which] - squared(sumx(which));
}

double LLSQcomputer::Lxy(unsigned which){
  return numSamples * ys[which] - (sumx(which) * ys[0]);
}

double LLSQcomputer::Lyy(){
  return numSamples * sumy2 - squared(ys[0]);
}

double LLSQcomputer::Rsquared(unsigned which){  //todo:3 cache these and compute at time of inverse.
  if(which == 0) {
    //naive formula would return sum of Y
    return Nan;//need some investigation, no real meaning for this!
  }
  if(numSamples < numParams()) {
    return Nan; //refuse even if computation wouldn't blow.
  }
  return ratio(squared(Lxy(which)), Lxx(which) * Lyy());
}

double LLSQcomputer::varY(){
  return ratio(Lyy(), numSamples);
}

double LLSQcomputer::crossCorr(unsigned i, unsigned j){
  return ratio(squared(numSamples * xs[i][j] - sumx(i) * sumx(j)), Lxx(i) * Lxx(j));
}

double LLSQcomputer::multiRsquared(){
  double sum = 0;
  forSize(ci){
    if(!ignore[ci]) { //save some time.
      double term = solution[ci] * Lxy(ci);
      sum += term;
    }
  }
  return ratio(sum, Lyy());
}
