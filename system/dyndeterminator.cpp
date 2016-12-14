#include <cmath>

#include "dyndeterminator.h"
#include "iterate.h"
#include "logger.h"
#include "minimath.h"

#define forP(si) for(int si=p.size;si-->0;)
#define forTriangle(cl) for(int cl=rw+1;cl-->0;)
#define withOutDiagonal(cl) for(int cl=rw;cl-->0;)

LLSQcomputer::LLSQcomputer(int numCoeff):
  MatrixInverter(numCoeff),
  ys(size),
  solution(size) {
  clear();
}

int LLSQcomputer::numParams() const {
  int ni = 0;
  ITERATE(Gater::const_iterator, it, ignore) {
    if(*it == false) {
      ++ni;
    }
  }
  return ni;
}

void LLSQcomputer::include(double Y, const Column &correlate) {
  ++numSamples;
  sumy2 += squared(Y);
  //correlates size must match this size
  forSize(rw) {
    double x = correlate[rw];
    ys[rw] += Y * x;
    forTriangle(cl) {
      xs[rw][cl] += x * correlate[cl];
    }
  }
}

void LLSQcomputer::clear() {
  MatrixInverter::clear();
  forSize(rw) {
    solution[rw] = 0;
    ys[rw] = 0;
  }
  numSamples = 0;
  numFit = 0;
  sumy2 = 0.0;
}

void LLSQcomputer::applyIgnorance() {
  forSize(rw) {
    if(ignore[rw]) {
      //not clearing Ys as we may use those for statistics.
      xs[rw][rw] = 1.0;
      withOutDiagonal(cl) {
        xs[rw][cl] = 0;
      }
    }
  }
}

void LLSQcomputer::fillinTriangle() { //todo:2 move into matrixinverter and add a flag indicating symmetry
  forSize(rw) {
    if(!ignore[rw]) { //trusting that clear() was called at a good time.
      withOutDiagonal(cl) {
        xs[cl][rw] = xs[rw][cl]; //copy to upper triangle
      }
    }
  }
}

int LLSQcomputer::compute() {
  fillinTriangle();
  double det = MatrixInverter::compute();
  //multiply Ys on left, inv on right:
  if(isNormal(det)) { //#we do wish to exclude 0 as well as strange stuff.
    forSize(rw) {
      if(!ignore[rw]) {
        double product = 0;
        forSize(cl) {
          if(!ignore[cl]) {
            product += ys[cl] * inv[cl][rw]; //when we dropped the pivoting we effectiveley transposed the inverse.
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
}

double LLSQcomputer::sumx(int which) {
  return xs[0][which];
}

double LLSQcomputer::Lxx(int which) {
  return numSamples * xs[which][which] - squared(sumx(which));
}

double LLSQcomputer::Lxy(int which) {
  return numSamples * ys[which] - (sumx(which) * ys[0]);
}

double LLSQcomputer::Rsquared(int which) { //todo:3 cache these and compute at time of inverse.
  if(which == 0) {
    //naive formula would return sum of Y
    return Nan;//need some investigation, no real meaning for this!
  }
  if(numSamples < numParams()) {//#207: allowing infinite uncertainty to report the slope, until we fix the coeff editor.
    return Nan; //refuse even if computation wouldn't blow.
  }
  return ratio(squared(Lxy(which)), Lxx(which) * Lyy());
}

double LLSQcomputer::Lyy() {
  return numSamples * sumy2 - squared(ys[0]);
}

double LLSQcomputer::varY() {
  return ratio(Lyy(), numSamples);
}

double LLSQcomputer::crossCorr(int i, int j) {
  return ratio(squared(numSamples * xs[i][j] - sumx(i) * sumx(j)), Lxx(i) * Lxx(j));
}

double LLSQcomputer::multiRsquared() {
  double sum = 0;
  forSize(ci) {
    if(!ignore[ci]) { //save some time.
      double term = solution[ci] * Lxy(ci);
      sum += term;
    }
  }
  return ratio(sum, Lyy());
}
