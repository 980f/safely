#include "matrixinverter.h"
#include "minimath.h"
#include <cmath>
#include "logger.h"

MatrixInverter::MatrixInverter(int size):
  size(size),
  xs(size),
  inv(size),
  ignore(size) {
  //moved to constructor.
}

MatrixInverter::Matrix::Matrix(int size):
  std::vector< Column >(size) {
  forSize(i) {
    operator [](i).resize(size);
  }
}

void MatrixInverter::clear() {
  forSize(rw) {
    forSize(cl) {
      xs[rw][cl] = 0;
    }
  }
}

double MatrixInverter::compute() {
  dbg("Inverting matrix");
  if(size == 1) { //special case to expedite debug of non-special cases
    double norm = xs[0][0];
    inv[0][0] = ratio(1.0, norm);
    return norm;
  }
  if(size==2 ){//#optimized for the most frequent case, allowing this to be efficiently used for linear fits.
    if(ignore[1]){
      double norm = xs[0][0];
      inv[0][0] = ratio(1.0, norm);
      return norm;
    }
    if(ignore[0]){
      double norm = xs[1][1];
      inv[1][1] = ratio(1.0, norm);
      return norm;
    }
    double norm = xs[0][0]*xs[1][1]-xs[0][1]*xs[1][0];
    if(isNormal(norm)){
      forSize(rw) {
        forSize(cl) {
          dbg("In matrix inversion loop (quadratic efficiency)");
          inv[rw][cl] =  polarity(rw==cl) * ratio(xs[rw][cl],norm);
        }
      }
      return norm;
    }
    //else fall through to get identical failed state info as before.
  }
  forSize(rw) {
    forSize(cl) {
      inv[rw][cl] = xs[rw][cl]; //using separate before and after matrices for debuggability
      dbg("In second matrix inversion loop (quadratic efficiency)");
    }
  }
  double norm = 1.0;

  forSize(focus) {
    if(ignore[focus]) {
      continue;
    }

    double absmax = inv[focus][focus];
    if(absmax != 0) {
      norm *= absmax;
      double invmax = -1 / absmax;

#define inPlay(rw) !ignore[rw]&& rw!=focus

      forSize(rw) { //scale column by biggest entry
        if(inPlay(rw)) {
          inv[rw][focus] *= invmax; //row is multiplied by -1/diagonal element
        }
      }
      forSize(rw) {
        if(inPlay(rw)) {
          double d = inv[rw][focus];
          if(isNormal(d)) { //skip iteration if going to be adding multiple of 0. isnormal handles the -0.0's we seem to get.
            forSize(cl) {
              if(inPlay(cl)) {
                inv[rw][cl] += inv[focus][cl] * d; //subtract scaled row from other rows
                dbg("In third matrix inversion loop (quadratic efficiency)");
              }
            }
          }
        }
      }
      invmax = -invmax;
      forSize(cl) {
        if(inPlay(cl)) {
          inv[focus][cl] *= invmax; //row is scaled by 1/diagonal element
        }
      }
      inv[focus][focus] = invmax;
    } else {
      //determinant must be 0 as all remaining elements are 0
      return 0;
    }
  }
  return norm;
}

bool MatrixInverter::test() {
  Matrix one(size);
  forSize(cl) {
    if(!ignore[cl]) {
      forSize(rw) {
        if(!ignore[rw]) {
          double product = 0;
          forSize(pi) {
            if(!ignore[pi]) {
              product += xs[cl][pi] * inv[pi][rw];
            }
          }
          one[cl][rw] = product;
        }
      }
    }
  }
  return true;//could do some sort of check for identity.
}

#include "logger.h"
#include "stdio.h"
static bool showDump=false;
void MatrixInverter::dump(bool please) {
  if(showDump&&please) {
    dbg("Matrix Inverse");
    forSize(rw) {
      printf("\n%d", rw);
      forSize(cl) {
        printf(",%g", xs[rw][cl]);
      }
      printf("\t\t");
      forSize(cl) {
        printf(",%g", inv[rw][cl]);
      }
    }
    printf("\n");
    fflush(stdout);
  }
}
